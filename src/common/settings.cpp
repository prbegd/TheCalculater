/**
 * @file settings.cpp
 * @author prbegd
 * @brief Provides the implementation for settings related functionality.
 * @date 2025-08-08
 *
 * Copyright © 2025 prbegd & TheCalculater contributors
 * Licensed under the MIT License. See LICENSE in the project root for license information.
 *
 */
#include "TheCalculater/settings.hpp"
#include "TheCalculater/util.hpp"
#include "json/value.h"
#include <algorithm>
#include <format>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace TheCalculater::settings {
    namespace {
        std::unordered_map<std::string, Value,
            core::Hash<std::string_view>, core::EqualTo<std::string_view>>
            settings;
        std::mutex settingsMutex;

        std::vector<std::string> modifiedKeysValue;
        std::mutex modifiedKeysMutex;

        using PropertiesType = std::unordered_map<std::string, std::unique_ptr<ItemProperty>,
            core::Hash<std::string_view>, core::EqualTo<std::string_view>>;
        PropertiesType properties;
        std::mutex propertiesMutex;

        std::vector<std::function<void(std::string_view, const Value&)>> itemChangedEventListeners;
        std::mutex itemChangedEventListenersMutex;

        core::AtomicSharedPtr<std::string> settingsFilePath(nullptr);
    }
    Value read(std::string_view key)
    {
        std::lock_guard<std::mutex> lock(settingsMutex);
        auto res = settings.find(key);
        if (res == settings.end()) {
            throw_with_trace(BadSettingsException(std::format("Key not found: {}", key)));
        }
        return res->second; // cppcheck-suppress derefInvalidIteratorRedundantCheck
    }
    BooleanValue readBool(std::string_view key)
    {
        auto val = read(key);
        if (!val.isBool())
            throw_with_trace(BadSettingsException(std::format("Value is not a boolean for key: {}", key)));
        return val.toBool();
    }
    ListValue readList(std::string_view key)
    {
        auto val = read(key);
        if (!val.isList())
            throw_with_trace(BadSettingsException(std::format("Value is not a list for key: {}", key)));
        return val.toList();
    }
    ObjectValue readObject(std::string_view key)
    {
        auto val = read(key);
        if (!val.isObject())
            throw_with_trace(BadSettingsException(std::format("Value is not a object for key: {}", key)));
        return val.toObject();
    }
    StringValue readString(std::string_view key)
    {
        auto val = read(key);
        if (!val.isString())
            throw_with_trace(BadSettingsException(std::format("Value is not a string for key: {}", key)));
        return val.toString();
    }
    IntegerValue readInteger(std::string_view key)
    {
        auto val = read(key);
        if (!val.isInteger())
            throw_with_trace(BadSettingsException(std::format("Value is not a integer for key: {}", key)));
        return val.toInteger();
    }
    DecimalValue readDecimal(std::string_view key)
    {
        auto val = read(key);
        if (!val.isDecimal())
            throw_with_trace(BadSettingsException(std::format("Value is not a decimal for key: {}", key)));
        return val.toDecimal();
    }

    // This function cannot create a new key.
    void write(std::string_view key, const Value& value)
    {
        {
            std::lock_guard<std::mutex> lock(settingsMutex);
            auto res = settings.find(key);
            if (res == settings.end()) {
                throw_with_trace(BadSettingsException(std::format("Key not found: {}", key)));
                return;
            }
            if (res->second.index() != value.index()) {
                throw_with_trace(BadSettingsException(std::format("Value type mismatch for key: {} (Excepted: {}, Actual: {})", key, res->second.type(), value.type())));
            }
            // TODO: Add validation for value here. (regex, min, max, etc.)
            res->second = value;
        }
        {
            std::lock_guard<std::mutex> lock(modifiedKeysMutex);
            if (std::find(modifiedKeysValue.begin(), modifiedKeysValue.end(), key) == modifiedKeysValue.end())
                modifiedKeysValue.emplace_back(key);
        }
        {
            std::lock_guard<std::mutex> lock(itemChangedEventListenersMutex);
            for (const auto& listener : itemChangedEventListeners) {
                listener(key, value);
            }
        }
    }
    void writeBool(std::string_view key, const BooleanValue& value)
    {
        write(key, { value });
    }
    void writeList(std::string_view key, const ListValue& value)
    {
        write(key, { value });
    }
    void writeObject(std::string_view key, const ObjectValue& value)
    {
        write(key, { value });
    }
    void writeString(std::string_view key, const StringValue& value)
    {
        write(key, { value });
    }
    void writeInteger(std::string_view key, const IntegerValue& value)
    {
        write(key, { value });
    }
    void writeDecimal(std::string_view key, const DecimalValue& value)
    {
        write(key, { value });
    }
    ItemProperty& property(std::string_view key)
    {
        std::lock_guard<std::mutex> lock(propertiesMutex);
        auto res = properties.find(key);
        if (res == properties.end()) {
            throw_with_trace(BadSettingsException(std::format("Key not found: {}", key)));
        }
        return *res->second; // cppcheck-suppress derefInvalidIteratorRedundantCheck
    }
    const std::vector<std::string>& modifiedKeys()
    {
        std::lock_guard<std::mutex> lock(modifiedKeysMutex);
        return modifiedKeysValue;
    }
    void setSettingsFilePath(std::string_view path)
    {
        auto newPath = std::make_shared<std::string>(path);
        settingsFilePath.store(newPath, std::memory_order_release);
    }

    namespace {
        /// details for parseValue
        namespace _dParseValue {
            template <typename T>
            bool validNumber(T& val, const NumberItemProperty& property, std::string& error)
            {
                if (property.max && val > T(*property.max)) {
                    error = std::format("Value is out of range. (Max: {}, Actual: {})", *property.max, val.toString());
                    return false;
                }
                if (property.min && val < T(*property.min)) {
                    error = std::format("Value is out of range. (Min: {}, Actual: {})", *property.min, val.toString());
                    return false;
                }
                return true;
            }

            bool parseValueInteger(Value& result, const ItemProperty& property, const Json::Value& item, std::string& error)
            {
                IntegerValue val;
                if (item.isInt())
                    val = IntegerValue(item.asInt64());
                else if (item.isString())
                    val = IntegerValue::fromString(item.asString());
                else {
                    error = "Value is not a integer";
                    return false;
                }
                std::string validErr;
                if (!validNumber<IntegerValue>(val, static_cast<const NumberItemProperty&>(property), validErr)) {
                    error = "Valid error:" + validErr;
                    return false;
                }
                result = { val };
                return true;
            }
            bool parseValueDecimal(Value& result, const ItemProperty& property, const Json::Value& item, std::string& error)
            {
                DecimalValue val;
                if (item.isDouble())
                    val = DecimalValue(item.asDouble());
                else if (item.isInt())
                    val = DecimalValue(item.asInt64());
                else if (item.isString())
                    val = DecimalValue::fromString(item.asString());
                else {
                    error = "Value is not a number";
                    return false;
                }
                std::string validErr;
                if (!validNumber<DecimalValue>(val, static_cast<const NumberItemProperty&>(property), validErr)) {
                    error = "Valid error:" + validErr;
                    return false;
                }
                result = { val };
                return true;
            }
            bool parseValueString(Value& result, const ItemProperty& property, const Json::Value& item, std::string& error)
            {
                StringValue val;
                if (item.isString())
                    val = StringValue(item.asString());
                else {
                    error = "Value is not a string";
                    return false;
                }
                const auto& strProperty = static_cast<const StringItemProperty&>(property);
                if (strProperty.pattern && !std::regex_search(val.string(), *strProperty.pattern)) {
                    error = "Value doesn't match the Regular Expression";
                    return false;
                }
                result = { val };
                return true;
            }
            bool parseValueList(Value& result, const ItemProperty& property, const Json::Value& item, std::string& error)
            {
                ListValue val;
                if (item.isArray()) {
                    const auto& listProperty = static_cast<const ListItemProperty&>(property);
                    for (unsigned i = 0; i < item.size(); ++i) {
                        Value child;
                        std::string childErr;
                        if (!parseValue(child, *listProperty.childType, item[i], childErr)) {
                            error = std::format("Error in child {}: {}", i, childErr);
                            return false;
                        }
                        val.push_back(std::move(child));
                    }
                } else {
                    error = "Value is not a list";
                    return false;
                }
                result = { val };
                return true;
            }
            bool parseValueObject(Value& result, const ItemProperty& property, const Json::Value& item, std::string& error)
            {
                ObjectValue val;
                if (item.isObject()) {
                    const auto& objProperty = static_cast<const ObjectItemProperty&>(property);
                    for (const auto& key : item.getMemberNames()) {
                        auto it = objProperty.properties.find(key);
                        if (it == objProperty.properties.end()) {
                            error = std::format("No such property in object: {}", key);
                            return false;
                        }
                        Value value;
                        std::string valueErr;
                        if (!parseValue(value, *(it->second), item[key], valueErr)) {
                            error = std::format("Error in property {}: {}", key, valueErr);
                            return false;
                        }
                        val.emplace_back(key, value);
                    }
                } else {
                    error = "Value is not a object";
                    return false;
                }
                result = { val };
                return true;
            }
            bool parseValueEnum(Value& result, const ItemProperty& property, const Json::Value& item, std::string& error)
            {
                StringValue val;
                if (item.isString())
                    val = StringValue(item.asString());
                else {
                    error = "Value is not a string";
                    return false;
                }
                const auto& enumProperty = static_cast<const EnumItemProperty&>(property);
                if (std::find(enumProperty.values.begin(), enumProperty.values.end(), val) == enumProperty.values.end()) {
                    error = "Value is not a valid enum";
                    return false;
                }
                result = { val };
                return true;
            }

        } // namespace _dParseValue
    } // namespace

    bool parseValue(Value& result, const ItemProperty& property, const Json::Value& item, std::string& error)
    {
        using namespace _dParseValue;

        switch (property.type()) {
        case ValueType::Integer:
            return parseValueInteger(result, property, item, error);
        case ValueType::Decimal:
            return parseValueDecimal(result, property, item, error);
        case ValueType::String:
            return parseValueString(result, property, item, error);
        case ValueType::Boolean:
            if (item.isBool())
                result = { BooleanValue(item.asBool()) };
            else {
                error = "Value is not a boolean";
                return false;
            }
            return true;
        case ValueType::List:
            return parseValueList(result, property, item, error);
        case ValueType::Object:
            return parseValueObject(result, property, item, error);
        case ValueType::Enum:
            return parseValueEnum(result, property, item, error);
        default:
            error = "Unknown property type or type is Namespace or Button, which has no value.";
            return false;
        }
    }

    namespace {
    namespace _dLoadConfigTemplate {
        using IsMethodType = bool (Json::Value::*)() const;
        template <IsMethodType IsMethod, core::ConstexprString TypeName>
        bool readItemProperty(std::optional<Json::Value>& result, const Json::Value& item, const std::string& propName, bool required, std::string_view itemName)
        {
            if (item.find(propName)) {
                if (required) {
                    SPDLOG_WARN("Invalid config template: {}: {} is missing {} property.", itemName, propName, TypeName.v);
                    return false;
                } else
                    return true;
            }
            const auto& val = item[propName];
            if (!(val.*IsMethod)()) {
                SPDLOG_WARN("Invalid config template: {}: {} is not a {}.", itemName, propName, TypeName.v);
                return false;
            }
            result = val;
            return true;
        }
        template <typename T>
        using AsMethodType = T (Json::Value::*)() const;
        template <typename ResType, IsMethodType IsMethod, core::ConstexprString TypeName, typename AsMethodReturnType>
        bool readItemPropertyAs(AsMethodType<AsMethodReturnType> asMethod, std::optional<ResType>& result, const Json::Value& item, const std::string& propName, bool required, std::string_view itemName)
        {
            if (item.find(propName)) {
                if (required) {
                    SPDLOG_WARN("Invalid config template: {}: {} is missing {} property.", itemName, propName, TypeName.v);
                    return false;
                } else
                    return true;
            }
            const auto& val = item[propName];
            if (!(val.*IsMethod)()) {
                SPDLOG_WARN("Invalid config template: {}: {} is not a {}.", itemName, propName, TypeName.v);
                return false;
            }
            result = (val.*asMethod)();
            return true;
        }

        bool parseItemValueType(ValueType& type, const Json::Value& item, std::string_view itemName)
        {
            std::optional<std::string> typeName;
            if (!readItemPropertyAs<std::string, &Json::Value::isString, "string">(&Json::Value::asString, typeName, item, "type", true, itemName))
                return false;
            const auto& val = typeName.value();
            if (val == "namespace")
                type = ValueType::Namespace;
            else if (val == "integer")
                type = ValueType::Integer;
            else if (val == "decimal")
                type = ValueType::Decimal;
            else if (val == "string")
                type = ValueType::String;
            else if (val == "boolean")
                type = ValueType::Boolean;
            else if (val == "list")
                type = ValueType::List;
            else if (val == "object")
                type = ValueType::Object;
            else if (val == "enum")
                type = ValueType::Enum;
            else if (val == "button")
                type = ValueType::Button;
            else {
                SPDLOG_WARN("Invalid config template: {}: unknown type '{}'.", itemName, val);
                return false;
            }

            return true;
        }

        bool parseItem(PropertiesType& property, const Json::Value& item, const std::string& itemName);

        bool parseItemNamespaceEx(PropertiesType& property, const Json::Value& item, const std::string& itemName, std::optional<std::string>& name)
        {
            property[itemName] = std::make_unique<NamespaceItemProperty>(std::nullopt, std::move(name), std::nullopt, std::nullopt, std::nullopt, std::nullopt);
            std::optional<Json::Value> children;
            if (!readItemProperty<&Json::Value::isObject, "object">(children, item, "children", true, itemName))
                return false;
            if (!std::all_of(children->getMemberNames().begin(), children->getMemberNames().end(), [&](const std::string& key) { return parseItem(property, (*children)[key], itemName + '.' + key); }))
                return false;
            return true;
        }

        bool parseItemButtonEx(PropertiesType& property, const Json::Value& item, const std::string& itemName, std::optional<std::string>& name, std::optional<std::string>& description, std::optional<std::string>& note, std::optional<std::string>& warning, std::optional<std::string>& deprecated)
        {
            std::optional<std::string> text;
            if (!readItemPropertyAs<std::string, &Json::Value::isString, "string">(&Json::Value::asString, text, item, "text", true, itemName))
                return false;
            property[itemName] = std::make_unique<ButtonItemProperty>(std::nullopt, std::move(name), std::move(description), std::move(note), std::move(warning), std::move(deprecated), std::move(text.value()));
            return true;
        }

        bool parseItem(PropertiesType& property, const Json::Value& item, const std::string& itemName)
        {
            ValueType type {};
            if (!parseItemValueType(type, item, itemName))
                return false;

            std::optional<std::string> name;
            if (!readItemPropertyAs<std::string, &Json::Value::isString, "string">(&Json::Value::asString, name, item, "name", true, itemName))
                return false;

            if (type == ValueType::Namespace)
                return parseItemNamespaceEx(property, item, itemName, name);

            std::optional<std::string> description;
            if (!readItemPropertyAs<std::string, &Json::Value::isString, "string">(&Json::Value::asString, description, item, "description", false, itemName))
                return false;
            std::optional<std::string> note;
            if (!readItemPropertyAs<std::string, &Json::Value::isString, "string">(&Json::Value::asString, note, item, "note", false, itemName))
                return false;
            std::optional<std::string> warning;
            if (!readItemPropertyAs<std::string, &Json::Value::isString, "string">(&Json::Value::asString, warning, item, "warning", false, itemName))
                return false;
            std::optional<std::string> deprecated;
            if (!readItemPropertyAs<std::string, &Json::Value::isString, "string">(&Json::Value::asString, deprecated, item, "deprecated", false, itemName))
                return false;

            if (type == ValueType::Button)
                return parseItemButtonEx(property, item, itemName, name, description, note, warning, deprecated);

            std::unique_ptr<ItemProperty> propertyPtr;
            switch (type) {
            case ValueType::Integer: {
                propertyPtr = std::make_unique<IntegerItemProperty>(std::nullopt, name, description, note, warning, deprecated);
                std::optional<double> min;
                if (!readItemPropertyAs<double, &Json::Value::isNumeric, "double">(&Json::Value::asDouble, min, item, "deprecated", false, itemName))
                    return false;
                std::optional<double> max;
                break;
            }
            case ValueType::Decimal:
                propertyPtr = std::make_unique<DecimalItemProperty>(std::nullopt, name, description, note, warning, deprecated);
                break;
            case ValueType::String:
                propertyPtr = std::make_unique<StringItemProperty>(std::nullopt, name, description, note, warning, deprecated);
                break;
            case ValueType::Boolean:
                propertyPtr = std::make_unique<BooleanItemProperty>(std::nullopt, name, description, note, warning, deprecated);
            case ValueType::List:
                break;
            case ValueType::Object:
                break;
            case ValueType::Enum:
                break;
            default:
                // How can this happen? Must be the cosmic ray.
                SPDLOG_WARN("Invalid config template: {}: unknown type '{}'. Ignored.", itemName, static_cast<int>(type));
                return true;
            }

            return true;
        }
    }

    } // namespace ::_dLoadConfigTemplate

    bool loadConfigTemplate(const Json::Value& value)
    {
        using namespace _dLoadConfigTemplate;

        if (value.empty()) {
            SPDLOG_WARN("Invalid config template: json is empty.");
            return false;
        }
        if (!value.isObject()) {
            SPDLOG_WARN("Invalid config template: json is not an object.");
            return false;
        }
        PropertiesType newProperties;
        if (!std::all_of(value.getMemberNames().begin(), value.getMemberNames().end(), [&](const std::string& key) { return parseItem(newProperties, value[key], key); }))
            return false;
        {
            std::lock_guard<std::mutex> lock(propertiesMutex);
            properties.merge(newProperties);
        }
        return true;
    }

    void registerItemChangedEventListener(const std::function<void(std::string_view, const Value&)>& listener)
    {
        std::lock_guard<std::mutex> lock(itemChangedEventListenersMutex);
        itemChangedEventListeners.emplace_back(listener);
    }

    void dbginit()
    {
        std::lock_guard<std::mutex> lock(settingsMutex);
        settings.insert({ "key", { StringValue("value") } });
    }

    std::string Value::type() const noexcept
    {
        switch (index()) {
        case 0:
            return "BooleanValue";
        case 1:
            return "ListValue";
        case 2:
            return "ObjectValue";
        case 3:
            return "StringValue";
        case 4:
            return "IntegerValue";
        case 5:
            return "DecimalValue";
        default:
            return "Nothing";
        }
    }
} // namespace TheCalculater::settings
