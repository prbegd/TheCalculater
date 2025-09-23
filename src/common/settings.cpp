/**
 * @file settings.cpp
 * @author prbegd
 * @brief Provides the implementation for settings related functionality.
 * @date 2025-08-08
 *
 * Copyright © 2025 Cai Yaoxing
 * SPDX-License-Identifier: GPL-3.0-only
 * This file is part of TheCalculater.
 * See the file LICENSE in the project root or go to
 * <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
 *
 */
#include "TheCalculater/settings.hpp"
#include "TheCalculater/core.hpp"
#include "TheCalculater/util.hpp"
#include "spdlog/spdlog.h"
#include "json/value.h"
#include <algorithm>
#include <boost/algorithm/string/split.hpp>
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
        using SettingsType = std::unordered_map<std::string, Value,
            core::Hash<std::string_view>, core::EqualTo<std::string_view>>;
        SettingsType settings;
        std::mutex settingsMutex;

        std::vector<std::string> modifiedKeysValue;
        std::mutex modifiedKeysMutex;

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
            // todoedit: parseValue already did validation so we may not need validation here.
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
            if (item.isIntegral())
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
            if (item.isIntegral())
                val = DecimalValue(item.asInt64());
            else if (item.isNumeric())
                val = DecimalValue(item.asDouble());
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

    }
    } // namespace ::_dParseValue

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
                result = { item.asBool() };
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
        /// Type of Json::Value::isXXX
        using IsMethodType = bool (Json::Value::*)() const;

        /**
         * @brief Read item property and store it in result.
         *
         * @tparam IsMethod Should be &Json::Value::isXXX. We use this to check whether the value is of the correct type. e.g. &Json::Value::isString.
         * @tparam TypeName The type name of the expected type, it's only used for logging.
         * @param result Output parameter. The value.
         * @param item The item to read from.
         * @param propName The name of the property to read.
         * @param required Whether the property is required. If it's not, we will just ignore it if it's missing.
         * @param itemName The name of the item, it's only used for logging.
         * @return Whether the operation succeeded.
         * @see readItemPropertyAs
         */
        template <IsMethodType IsMethod, core::ConstexprString TypeName>
        bool readItemProperty(std::optional<Json::Value>& result, const Json::Value& item, const std::string& propName, bool required, std::string_view itemName)
        {
            const auto& it = item.find(propName);
            if (!it) {
                if (required) {
                    SPDLOG_WARN("Invalid config template: {}: missing {} property '{}'.", itemName, TypeName.v, propName);
                    return false;
                } else
                    return true;
            }
            if (!(*it.*IsMethod)()) {
                SPDLOG_WARN("Invalid config template: {}: '{}' is not a {}.", itemName, propName, TypeName.v);
                return false;
            }
            result = *it;
            return true;
        }

        /// Type of Json::Value::asXXX
        template <typename T>
        using AsMethodType = T (Json::Value::*)() const;

        /**
         * @brief Read item property and convert it to ResType.
         *
         * @tparam ResType The type to convert the value to.
         * @tparam IsMethod Should be &Json::Value::isXXX. We use this to check whether the value is of the correct type. e.g. &Json::Value::isString.
         * @tparam TypeName The type name of the expected type, it's only used for logging.
         * @tparam AsMethodReturnType Auto-deduced.
         * @param asMethod  Should be &Json::Value::asXXX. We use this to convert the value. e.g. &Json::Value::asString.
         * @param result Output parameter. The converted value.
         * @param item The item to read from.
         * @param propName The name of the property to read.
         * @param required Whether the property is required. If it's not, we will just ignore it if it's missing.
         * @param itemName The name of the item, it's only used for logging.
         * @return Whether the operation succeeded.
         * @see readItemProperty
         */
        template <typename ResType, IsMethodType IsMethod, core::ConstexprString TypeName, typename AsMethodReturnType>
        bool readItemPropertyAs(AsMethodType<AsMethodReturnType> asMethod, std::optional<ResType>& result, const Json::Value& item, const std::string& propName, bool required, std::string_view itemName)
        {
            const auto& it = item.find(propName);
            if (!it) {
                if (required) {
                    SPDLOG_WARN("Invalid config template: {}: missing {} property '{}'.", itemName, TypeName.v, propName);
                    return false;
                } else
                    return true;
            }
            if (!(*it.*IsMethod)()) {
                SPDLOG_WARN("Invalid config template: {}: '{}' is not a {}.", itemName, propName, TypeName.v);
                return false;
            }
            result = (*it.*asMethod)();
            return true;
        }

        /// parse string to ValueType
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

        template <bool AllowTypeNamespaceOrButton = true>
        bool parseItem(PropertiesType& property, const Json::Value& item, const std::string& itemName, const std::string& parentName = {});

        bool parseItemOnce(std::unique_ptr<ItemProperty>& property, const Json::Value& item, const std::string& itemName);

        bool parseItemNamespaceEx(PropertiesType& property, const Json::Value& item, const std::string& itemName, std::optional<std::string>& name)
        {
            property[itemName] = std::make_unique<NamespaceItemProperty>(std::nullopt, std::move(name), std::nullopt, std::nullopt, std::nullopt, std::nullopt);
            std::optional<Json::Value> children;
            if (!readItemProperty<&Json::Value::isObject, "object">(children, item, "children", true, itemName))
                return false;
            auto tempMemberNames = children->getMemberNames();
            return std::all_of(tempMemberNames.begin(), tempMemberNames.end(), [&](const std::string& key) { return parseItem(property, (*children)[key], itemName + '.' + key); });
        }

        bool parseItemButtonEx(PropertiesType& property, const Json::Value& item, const std::string& itemName, std::optional<std::string>& name, std::optional<std::string>& description, std::optional<std::string>& note, std::optional<std::string>& warning, std::optional<std::string>& deprecated)
        {
            std::optional<std::string> text;
            if (!readItemPropertyAs<std::string, &Json::Value::isString, "string">(&Json::Value::asString, text, item, "text", true, itemName))
                return false;
            property[itemName] = std::make_unique<ButtonItemProperty>(std::nullopt, std::move(name), std::move(description), std::move(note), std::move(warning), std::move(deprecated), std::move(text.value()));
            return true;
        }

        template <typename T>
        bool createItemPropertyNumber(std::unique_ptr<ItemProperty>& propertyPtr, const Json::Value& item,
            const std::string& itemName, std::optional<std::string>& name, std::optional<std::string>& description, std::optional<std::string>& note, std::optional<std::string>& warning, std::optional<std::string>& deprecated)
        {
            std::optional<double> min;
            if (!readItemPropertyAs<double, &Json::Value::isNumeric, "double">(&Json::Value::asDouble, min, item, "deprecated", false, itemName))
                return false;
            std::optional<double> max;
            if (!readItemPropertyAs<double, &Json::Value::isNumeric, "double">(&Json::Value::asDouble, max, item, "deprecated", false, itemName))
                return false;
            propertyPtr = std::make_unique<T>(std::nullopt, std::move(name), std::move(description), std::move(note), std::move(warning), std::move(deprecated), min, max);
            return true;
        }
        bool createItemPropertyString(std::unique_ptr<ItemProperty>& propertyPtr, const Json::Value& item,
            const std::string& itemName, std::optional<std::string>& name, std::optional<std::string>& description, std::optional<std::string>& note, std::optional<std::string>& warning, std::optional<std::string>& deprecated)
        {
            std::vector<StringValue> enums;
            std::optional<Json::Value> enumsRaw;
            if (!readItemProperty<&Json::Value::isArray, "array">(enumsRaw, item, "enum", false, itemName))
                return false;
            if (enumsRaw)
                for (const auto& enumItem : *enumsRaw) {
                    if (!enumItem.isString()) {
                        SPDLOG_WARN("Invalid config template: {}: enum item is not a string.", itemName);
                        return false;
                    }
                    enums.emplace_back(enumItem.asString());
                }

            std::optional<std::regex> pattern;
            std::optional<std::string> patternRaw;
            if (!readItemPropertyAs<std::string, &Json::Value::isString, "string">(&Json::Value::asString, patternRaw, item, "pattern", false, itemName))
                return false;
            if (patternRaw)
                try {
                    pattern.emplace(*patternRaw);
                } catch (const std::regex_error& e) {
                    SPDLOG_WARN("Invalid config template: {}: invalid regex '{}': {}", itemName, *patternRaw, e.what());
                    return false;
                }
            propertyPtr = std::make_unique<StringItemProperty>(std::nullopt, name, description, note, warning, deprecated, pattern, enums);

            return true;
        }

        bool createItemPropertyList(std::unique_ptr<ItemProperty>& propertyPtr, const Json::Value& item,
            const std::string& itemName, std::optional<std::string>& name, std::optional<std::string>& description, std::optional<std::string>& note, std::optional<std::string>& warning, std::optional<std::string>& deprecated)
        {
            std::unique_ptr<ItemProperty> childType;
            std::optional<Json::Value> childTypeRaw;
            if (!readItemProperty<&Json::Value::isObject, "object">(childTypeRaw, item, "type", true, itemName))
                return false;

            if (!parseItemOnce(childType, *childTypeRaw, itemName + ".__childtype"))
                return false;

            propertyPtr = std::make_unique<ListItemProperty>(std::nullopt, name, description, note, warning, deprecated, std::move(childType));

            return true;
        }
        bool createItemPropertyObject(std::unique_ptr<ItemProperty>& propertyPtr, const Json::Value& item,
            const std::string& itemName, std::optional<std::string>& name, std::optional<std::string>& description, std::optional<std::string>& note, std::optional<std::string>& warning, std::optional<std::string>& deprecated)
        {
            PropertiesType objectProperties;
            std::optional<Json::Value> propertiesRaw;
            if (!readItemProperty<&Json::Value::isObject, "object">(propertiesRaw, item, "properties", true, itemName))
                return false;
            auto tempMemberNames = propertiesRaw->getMemberNames();
            if (!std::all_of(tempMemberNames.begin(), tempMemberNames.end(), [&](const std::string& key) { return parseItem<false>(objectProperties, (*propertiesRaw)[key], itemName + '.' + key); }))
                return false;

            propertyPtr = std::make_unique<ObjectItemProperty>(std::nullopt, name, description, note, warning, deprecated, std::move(objectProperties));

            return true;
        }
        bool createItemPropertyEnum(std::unique_ptr<ItemProperty>& propertyPtr, const Json::Value& item,
            const std::string& itemName, std::optional<std::string>& name, std::optional<std::string>& description, std::optional<std::string>& note, std::optional<std::string>& warning, std::optional<std::string>& deprecated)
        {
            std::vector<StringValue> values;
            std::optional<Json::Value> valuesRaw;
            if (!readItemProperty<&Json::Value::isArray, "array">(valuesRaw, item, "values", true, itemName))
                return false;
            for (const auto& enumItem : *valuesRaw) {
                if (!enumItem.isString()) {
                    SPDLOG_WARN("Invalid config template: {}: enum item is not a string.", itemName);
                    return false;
                }
                values.emplace_back(enumItem.asString());
            }
            propertyPtr = std::make_unique<EnumItemProperty>(std::nullopt, name, description, note, warning, deprecated, values);
            return true;
        }

        /// Parse validation fields and store them in the ItemProperty.
        bool createItemProperty(std::unique_ptr<ItemProperty>& propertyPtr, const Json::Value& item,
            const std::string& itemName, ValueType type, std::optional<std::string>& name, std::optional<std::string>& description, std::optional<std::string>& note, std::optional<std::string>& warning, std::optional<std::string>& deprecated)
        {
            switch (type) {
            case ValueType::Integer:
                return createItemPropertyNumber<IntegerItemProperty>(propertyPtr, item, itemName, name, description, note, warning, deprecated);
            case ValueType::Decimal:
                return createItemPropertyNumber<DecimalItemProperty>(propertyPtr, item, itemName, name, description, note, warning, deprecated);
            case ValueType::String:
                return createItemPropertyString(propertyPtr, item, itemName, name, description, note, warning, deprecated);
            case ValueType::Boolean:
                propertyPtr = std::make_unique<BooleanItemProperty>(std::nullopt, name, description, note, warning, deprecated);
                break;;
            case ValueType::List:
                return createItemPropertyList(propertyPtr, item, itemName, name, description, note, warning, deprecated);
            case ValueType::Object:;
                return createItemPropertyObject(propertyPtr, item, itemName, name, description, note, warning, deprecated);
            case ValueType::Enum:;
                return createItemPropertyEnum(propertyPtr, item, itemName, name, description, note, warning, deprecated);
            default:
                // How can this happen? Must be the cosmic ray.
                SPDLOG_WARN("Invalid config template: {}: unknown type '{}'.", itemName, static_cast<int>(type));
                return false;
            }
            return true;
        }

        // Only for parsing list.child_type for now
        // So we remove useless logics
        bool parseItemOnce(std::unique_ptr<ItemProperty>& property, const Json::Value& item, const std::string& itemName)
        {
            static const std::regex itemNameRegex(R"(^[a-zA-Z0-9_]+$)");
            {
                std::vector<std::string> splitRes;
                boost::algorithm::split(splitRes, itemName, core::boolCharPred<'.'>);
                if (!std::regex_match(splitRes.at(splitRes.size() - 1), itemNameRegex)) {
                    SPDLOG_WARN("Invalid config template: {}: invalid name.", itemName);
                    return false;
                }
            }
            ValueType type {};
            if (!parseItemValueType(type, item, itemName))
                return false;

            if (type == ValueType::Namespace) {
                SPDLOG_WARN("Invalid config template: {}: type 'namespace' is not allowed here.", itemName);
                return false;
            }

            if (type == ValueType::Button) {
                SPDLOG_WARN("Invalid config template: {}: type 'button' is not allowed here.", itemName);
                return false;
            }

            // dummy values
            std::optional<std::string> name;
            std::optional<std::string> description;
            std::optional<std::string> note;
            std::optional<std::string> warning;
            std::optional<std::string> deprecated;

            return createItemProperty(property, item, itemName, type, name, description, note, warning, deprecated);
        }
        // This template argument is used to parse object.properties for now.
        template <bool AllowTypeNamespaceOrButton>
        bool parseItem(PropertiesType& property, const Json::Value& item, const std::string& itemName, const std::string& parentName)
        {
            static const std::regex itemNameRegex(R"(^[a-zA-Z0-9_]+$)");
            {
                std::vector<std::string> splitRes;
                boost::algorithm::split(splitRes, itemName, core::boolCharPred<'.'>);
                if (!std::regex_match(splitRes.at(splitRes.size() - 1), itemNameRegex)) {
                    SPDLOG_WARN("Invalid config template: {}: invalid name.", itemName);
                    return false;
                }
            }
            std::string fullName = parentName.empty() ? itemName : (parentName + "." + itemName);
            
            ValueType type {};
            if (!parseItemValueType(type, item, itemName))
                return false;

            std::optional<std::string> name;
            if (!readItemPropertyAs<std::string, &Json::Value::isString, "string">(&Json::Value::asString, name, item, "name", false, itemName))
                return false;

            // Namespace only have field 'name' and 'children'
            if (type == ValueType::Namespace) {
                if constexpr (AllowTypeNamespaceOrButton)
                    return parseItemNamespaceEx(property, item, itemName, name);
                else {
                    SPDLOG_WARN("Invalid config template: {}: type 'namespace' is not allowed here.", itemName);
                    return false;
                }
            }

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

            // Button does not have 'default' field
            if (type == ValueType::Button) {
                if constexpr (AllowTypeNamespaceOrButton)
                    return parseItemButtonEx(property, item, itemName, name, description, note, warning, deprecated);
                else {
                    SPDLOG_WARN("Invalid config template: {}: type 'button' is not allowed here.", itemName);
                    return false;
                }
            }

            // We parse the default value after because we use the parseValue function to parse.
            // So we need to create the ItemProperty first. And because parseValue valids the value,
            // so we also need to parse validation fields.
            std::unique_ptr<ItemProperty> propertyPtr;
            if (!createItemProperty(propertyPtr, item, itemName, type, name, description, note, warning, deprecated))
                return false;

            const Json::Value* defaultValueRaw = item.find("default");
            if (!defaultValueRaw) {
                SPDLOG_WARN("Invalid config template: {}: missing 'default' value.", itemName);
                return false;
            }
            std::string defaultValueParseErr;
            Value defaultValue;

            // SPDLOG_DEBUG("My name is {}, and my default value is {}.", itemName, util::serialize5(*defaultValueRaw));

            if (!parseValue(defaultValue, *propertyPtr, *defaultValueRaw, defaultValueParseErr)) {
                SPDLOG_WARN("Invalid config template: {}: invalid 'default' value. {}", itemName, defaultValueParseErr);
                return false;
            }
            propertyPtr->defaultValue = std::move(defaultValue);

            property[itemName] = std::move(propertyPtr);

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
        auto tempMemberNames = value.getMemberNames();
        if (!std::all_of(tempMemberNames.begin(), tempMemberNames.end(), [&](const std::string& key) { 
            return parseItem(newProperties, value[key], key); 
        }))
            return false;

        // Add default values to settings
        SettingsType newSettings;
        for (const auto& [pKey, pValue] : newProperties) {
            if (pValue->type() == ValueType::Namespace || pValue->type() == ValueType::Button)
                continue;
            if (pValue->defaultValue)
                newSettings[pKey] = *pValue->defaultValue;
        }

        {
            std::lock_guard<std::mutex> lock(propertiesMutex);
            properties.merge(newProperties);
        }
        {
            std::lock_guard<std::mutex> lock(settingsMutex);
            settings.merge(newSettings);
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
