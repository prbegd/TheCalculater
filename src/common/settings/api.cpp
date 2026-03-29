/**
 * @file api.cpp
 * @author prbegd
 * @date 2026-03-28
 *
 * Copyright © 2025 Cai Yaoxing
 * SPDX-License-Identifier: GPL-3.0-only
 * This file is part of TheCalculater.
 * See the file LICENSE in the project root or go to
 * <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
 */
module;
#include "json5cpp/json5cpp.h"
#include "spdlog/spdlog.h"
#include "json/value.h"
#include <algorithm>
#include <boost/algorithm/string/split.hpp>
#include <boost/regex.hpp>
#include <format>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <fstream>

module TheCalculater.settings.api;
import TheCalculater.settings.exceptions;
import TheCalculater.util;
import TheCalculater.throwEx;

namespace TheCalculater::settings {
    namespace {
        using SettingsType = std::unordered_map<std::string, Value,
                                                util::TransparentHash<std::string_view>, std::equal_to<>>;
        SettingsType settings;
        std::mutex settingsMutex;
        std::vector<std::string> modifiedKeysValue;
        std::mutex modifiedKeysMutex;

        PropertiesType properties;
        std::mutex propertiesMutex;

        std::vector<std::function<void(std::string_view, const Value&)>> itemChangedEventListeners;
        std::mutex itemChangedEventListenersMutex;

        util::AtomicSharedPtr<std::string> settingsFilePath(nullptr);
    }
    Value read(std::string_view key)
    {
        std::lock_guard<std::mutex> lock(settingsMutex);
        auto res = settings.find(key);
        if (res == settings.end()) {
            throwEx(SettingsKeyNotFoundException(std::format("Key not found: {}", key)));
        }
        return res->second;
    }
    BooleanValue readBool(std::string_view key)
    {
        auto val = read(key);
        if (!val.isBool())
            throwEx(BadSettingsException(std::format("Value is not a boolean for key: {}", key)));
        return val.toBool();
    }
    ListValue readList(std::string_view key)
    {
        auto val = read(key);
        if (!val.isList())
            throwEx(BadSettingsException(std::format("Value is not a list for key: {}", key)));
        return val.toList();
    }
    ObjectValue readObject(std::string_view key)
    {
        auto val = read(key);
        if (!val.isObject())
            throwEx(BadSettingsException(std::format("Value is not a object for key: {}", key)));
        return val.toObject();
    }
    StringValue readString(std::string_view key)
    {
        auto val = read(key);
        if (!val.isString())
            throwEx(BadSettingsException(std::format("Value is not a string for key: {}", key)));
        return val.toString();
    }
    IntegerValue readInteger(std::string_view key)
    {
        auto val = read(key);
        if (!val.isInteger())
            throwEx(BadSettingsException(std::format("Value is not a integer for key: {}", key)));
        return val.toInteger();
    }
    DecimalValue readDecimal(std::string_view key)
    {
        auto val = read(key);
        if (!val.isDecimal())
            throwEx(BadSettingsException(std::format("Value is not a decimal for key: {}", key)));
        return val.toDecimal();
    }

    // This function cannot create a new key.
    void write(std::string_view key, const Value& value)
    {
        {
            std::lock_guard<std::mutex> lock(settingsMutex);
            auto res = settings.find(key);
            if (res == settings.end()) {
                throwEx(SettingsKeyNotFoundException(std::format("Key not found: {}", key)));
                return;
            }
            if (res->second.index() != value.index()) {
                throwEx(BadSettingsException(std::format("Value type mismatch for key: {} (Excepted: {}, Actual: {})", key, res->second.type(), value.type())));
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
        std::vector<std::function<void(std::string_view, const Value&)>> listeners;
        {
            std::lock_guard<std::mutex> lock(itemChangedEventListenersMutex);
            listeners = itemChangedEventListeners;
        }
        for (const auto& listener : listeners) {
            listener(key, value);
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
            throwEx(SettingsKeyNotFoundException(std::format("Key not found: {}", key)));
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
    std::weak_ptr<std::string> getSettingsFilePath()
    {
        return settingsFilePath.load(std::memory_order_acquire);
    }

    void saveModified(const std::string& fileName)
    {
        std::fstream file(fileName, std::ios::in);
        if (!file.is_open())
            throwEx(util::IOException(std::format("Cannot open file: {}", fileName)));
        if (modifiedKeys().empty()) return;
        // Read the file first instead just write all settings data
        // into file so we make sure only modified keys are saved.
        Json::Value json;
        std::string err;
        if (!Json5::parse(file, json, &err)) {
            SPDLOG_ERROR("Settings file is corrupted. New settings file will be created. Error: {}", err);
        }

        file.close();
        file.open(fileName, std::ios::out | std::ios::trunc);
        if (!file.is_open())
            throwEx(util::IOException(std::format("Cannot open file: {}", fileName)));

        for (const auto& key : modifiedKeys()) {
            auto val = read(key);
            Json::Value jVal;
            formatValue(jVal, val);
            json[key] = jVal;
        }

        file << util::serializeJson5(json);

        size_t modifiedKeysSize = modifiedKeys().size();
        {
            std::lock_guard<std::mutex> lock(modifiedKeysMutex);
            modifiedKeysValue.clear();
        }
        SPDLOG_INFO("Saved {} modified key(s).", modifiedKeysSize);
    }
    void parseSettings(const Json::Value& json, std::unordered_map<std::string, std::string>& errors)
    {
        if (!json.isObject())
            throwEx(std::invalid_argument("json is not an object"));
        const auto& keys = json.getMemberNames();
        for (const auto& jKey : keys) {
            const auto& jValue = json[jKey];

            auto res = properties.find(jKey);
            if (res == properties.end()) {
                errors[jKey] = "Key not found.";
                continue;
            }

            Value value;
            try {
                parseValue(value, property(jKey), jValue);
            } catch (const BadJsonSettingsValueException& e) {
                errors[jKey] = e.what();
                continue;
            }

            write(jKey, value);
        }
        SPDLOG_INFO("Parsed {} setting(s) and wrote.", keys.size());
    }
    void parseSettings(std::unordered_map<std::string, std::string>& errors)
    {
        auto pathPtr = getSettingsFilePath();
        if (pathPtr.expired())
            throwEx(util::WeakPointerExpiredException("Settings file path pointer is expired."));

        auto path = *pathPtr.lock();
        std::fstream file(path, std::ios::in | std::ios::out | std::ios::app);
        if (!file.is_open())
            throwEx(util::IOException(std::format("Cannot open file: {}", path)));
        if (file.peek() == std::fstream::traits_type::eof()) {
            file.clear();
            file << "{\n}";
            SPDLOG_INFO("Settings file is empty. Created new settings file.");
            return;
        }
        std::string jsonErr = "Is not a object.";
        Json::Value json;
        if (Json5::parse(file, json, &jsonErr) && json.isObject()) {
            parseSettings(json, errors);
            return;
        }
        file.close();
        // Clear the file and write a new empty object.
        file.open(path, std::ios::in | std::ios::out | std::ios::trunc);
        file << "{\n}";
        SPDLOG_WARN("Settings file is corrupted. Created new settings file. Error: {}", jsonErr);
    }

    namespace {
    namespace _d_parseValue {
        template <typename T>
        void validNumber(T& val, const NumberItemProperty& property)
        {
            if (property.max && val > T(*property.max)) {
                throwEx(BadJsonSettingsValueException(std::format("Value is out of range. (Max: {}, Actual: {})", *property.max, val.toString())));
            }
            if (property.min && val < T(*property.min)) {
                throwEx(BadJsonSettingsValueException(std::format("Value is out of range. (Min: {}, Actual: {})", *property.min, val.toString())));
            }
        }

        void parseValueInteger(Value& result, const ItemProperty& property, const Json::Value& item)
        {
            IntegerValue val;
            if (item.isIntegral())
                val = IntegerValue(item.asInt64());
            else if (item.isString())
                val = IntegerValue::fromString(item.asString());
            else {
                throwEx(BadJsonSettingsValueException("Value is not a integer"));
            }
            validNumber<IntegerValue>(val, static_cast<const NumberItemProperty&>(property));
            result = { val };
        }
        void parseValueDecimal(Value& result, const ItemProperty& property, const Json::Value& item)
        {
            DecimalValue val;
            if (item.isIntegral())
                val = DecimalValue(item.asInt64());
            else if (item.isNumeric())
                val = DecimalValue(item.asDouble());
            else if (item.isString())
                val = DecimalValue::fromString(item.asString());
            else {
                throwEx(BadJsonSettingsValueException("Value is not a number"));
            }
            validNumber<DecimalValue>(val, static_cast<const NumberItemProperty&>(property));
            result = { val };
        }
        void parseValueString(Value& result, const ItemProperty& property, const Json::Value& item)
        {
            StringValue val;
            if (item.isString())
                val = StringValue(item.asString());
            else {
                throwEx(BadJsonSettingsValueException("Value is not a string"));
            }
            const auto& strProperty = static_cast<const StringItemProperty&>(property);
            if (strProperty.pattern && !boost::regex_search(val.string(), *strProperty.pattern)) {
                throwEx(BadJsonSettingsValueException("Value doesn't match the Regular Expression"));
            }
            result = { val };
        }
        void parseValueList(Value& result, const ItemProperty& property, const Json::Value& item)
        {
            ListValue val;
            if (item.isArray()) {
                const auto& listProperty = static_cast<const ListItemProperty&>(property);
                val.reserve(item.size());
                for (unsigned i = 0; i < item.size(); ++i) {
                    Value child;
                    try {
                        parseValue(child, *listProperty.childType, item[i]);
                    } catch (const BadJsonSettingsValueException& e) {
                        throwEx(BadJsonSettingsValueException(std::format("Error in child {}: {}", i, e.what())));
                    }
                    val.push_back(std::move(child));
                }
            } else {
                throwEx(BadJsonSettingsValueException("Value is not a list"));
            }
            result = { std::move(val) };
        }
        void parseValueObject(Value& result, const ItemProperty& property, const Json::Value& item)
        {
            ObjectValue val;
            if (item.isObject()) {
                const auto& objProperty = static_cast<const ObjectItemProperty&>(property);
                for (const auto& key : item.getMemberNames()) {
                    auto it = objProperty.properties.find(key);
                    if (it == objProperty.properties.end()) {
                        throwEx(BadJsonSettingsValueException(std::format("No such property in object: {}", key)));
                    }
                    Value value;
                    try {
                        parseValue(value, *(it->second), item[key]);
                    } catch (const BadJsonSettingsValueException& e) {
                        throwEx(BadJsonSettingsValueException(std::format("Error in property {}: {}", key, e.what())), { });
                    }
                    val.emplace_back(key, value);
                }
            } else {
                throwEx(BadJsonSettingsValueException("Value is not an object"));
            }
            result = { std::move(val) };
        }
        void parseValueEnum(Value& result, const ItemProperty& property, const Json::Value& item)
        {
            StringValue val;
            if (item.isString())
                val = StringValue(item.asString());
            else {
                throwEx(BadJsonSettingsValueException("Value is not a string"));
            }
            const auto& enumProperty = static_cast<const EnumItemProperty&>(property);
            if (std::find(enumProperty.values.begin(), enumProperty.values.end(), val) == enumProperty.values.end()) {
                throwEx(BadJsonSettingsValueException("Value is not a valid enum"));
            }
            result = { val };
        }

    }
    } // namespace ::_d_parseValue

    void parseValue(Value& result, const ItemProperty& property, const Json::Value& item)
    {
        using namespace _d_parseValue;

        switch (property.type()) {
        case ValueType::Integer:
            return parseValueInteger(result, property, item);
        case ValueType::Decimal:
            return parseValueDecimal(result, property, item);
        case ValueType::String:
            return parseValueString(result, property, item);
        case ValueType::Boolean:
            if (item.isBool())
                result = { item.asBool() };
            else {
                throwEx(BadJsonSettingsValueException("Value is not a boolean"));
            }
            break;
        case ValueType::List:
            return parseValueList(result, property, item);
        case ValueType::Object:
            return parseValueObject(result, property, item);
        case ValueType::Enum:
            return parseValueEnum(result, property, item);
        default:
            throwEx(BadJsonSettingsValueException("Unknown property type or type is Namespace or Button, which has no value."));
        }
    }

    bool formatValue(Json::Value& result, const Value& item)
    {
        switch (item.index()) {
        case 0: // BooleanValue
            result = item.toBool();
            break;
        case 1: { // ListValue
            const auto& list = item.toList();
            for (unsigned i = 0; i < list.size(); i++) {
                Json::Value child;
                if (!formatValue(child, list[i])) {
                    return false;
                }
                result[i] = std::move(child);
            }
            break;
        }
        case 2: { // ObjectValue
            const auto& obj = item.toObject();
            for (const auto& [key, value] : obj) {
                Json::Value child;
                if (!formatValue(child, value)) {
                    return false;
                }
                result[key] = std::move(child);
            }
            break;
        }
        case 3: // StringValue
            result = item.toString().string();
            break;
        case 4: // IntegerValue
            // We format to string to prevent int64 overflow.
            result = item.toInteger().toString();
            break;
        case 5: // DecimalValue
            // We format to string to prevent precision loss.
            result = item.toDecimal().toString();
            break;
        default:
            return false;
        }

        return true;
    }

    namespace {
    namespace _d_loadConfigTemplate {
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
         * @see readItemPropertyAs
         */
        template <IsMethodType IsMethod, util::ConstexprString TypeName>
        void readItemProperty(std::optional<Json::Value>& result, const Json::Value& item, const std::string& propName, bool required, std::string_view itemName)
        {
            const auto& it = item.find(propName);
            if (!it) {
                if (required) {
                    throwEx(InvalidConfigTemplateException(std::format("{}: missing {} member '{}'.", itemName, TypeName.v, propName)));
                } else
                    return;
            }
            if (!(*it.*IsMethod)()) {
                throwEx(InvalidConfigTemplateException(std::format("Invalid config template: {}: Member '{}' is not a {}.", itemName, propName, TypeName.v)));
            }
            result = *it;
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
         * @see readItemProperty
         */
        template <typename ResType, IsMethodType IsMethod, util::ConstexprString TypeName, typename AsMethodReturnType>
        void readItemPropertyAs(AsMethodType<AsMethodReturnType> asMethod, std::optional<ResType>& result, const Json::Value& item, const std::string& propName, bool required, std::string_view itemName)
        {
            const auto& it = item.find(propName);
            if (!it) {
                if (required) {
                    throwEx(InvalidConfigTemplateException(std::format("Invalid config template: {}: missing {} member '{}'.", itemName, TypeName.v, propName)));
                } else
                    return;
            }
            if (!(*it.*IsMethod)()) {
                throwEx(InvalidConfigTemplateException(std::format("Invalid config template: {}: Member '{}' is not a {}.", itemName, propName, TypeName.v)));
            }
            result = (*it.*asMethod)();
        }

        /// parse string to ValueType
        void parseItemValueType(ValueType& type, const Json::Value& item, std::string_view itemName)
        {
            std::optional<std::string> typeName;
            readItemPropertyAs<std::string, &Json::Value::isString, "string">(&Json::Value::asString, typeName, item, "type", true, itemName);
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
                throwEx(InvalidConfigTemplateException(std::format("Invalid config template: {}:  unknown type '{}'.", itemName, val)));
            }
        }

        template <bool AllowTypeNamespaceOrButton = true>
        void parseItem(PropertiesType& property, const Json::Value& item, const std::string& itemName);

        void parseItemOnce(std::unique_ptr<ItemProperty>& property, const Json::Value& item, const std::string& itemName);

        void parseItemNamespaceEx(PropertiesType& property, const Json::Value& item, const std::string& itemName, std::optional<std::string>& name)
        {
            property[itemName] = std::make_unique<NamespaceItemProperty>(std::nullopt, std::move(name), std::nullopt, std::nullopt, std::nullopt, std::nullopt);
            std::optional<Json::Value> children;
            readItemProperty<&Json::Value::isObject, "object">(children, item, "children", true, itemName);
            for (const auto& key : children->getMemberNames()) {
                parseItem(property, (*children)[key], itemName + '.' += key);
            }
        }

        void parseItemButtonEx(PropertiesType& property, const Json::Value& item, const std::string& itemName, std::optional<std::string>& name, std::optional<std::string>& description, std::optional<std::string>& note, std::optional<std::string>& warning, std::optional<std::string>& deprecated)
        {
            std::optional<std::string> text;
            readItemPropertyAs<std::string, &Json::Value::isString, "string">(&Json::Value::asString, text, item, "text", true, itemName);
            property[itemName] = std::make_unique<ButtonItemProperty>(std::nullopt, std::move(name), std::move(description), std::move(note), std::move(warning), std::move(deprecated), std::move(text.value()));
        }

        template <typename T>
        void createItemPropertyNumber(std::unique_ptr<ItemProperty>& propertyPtr, const Json::Value& item,
                                      const std::string& itemName, std::optional<std::string>& name, std::optional<std::string>& description, std::optional<std::string>& note, std::optional<std::string>& warning, std::optional<std::string>& deprecated)
        {
            std::optional<double> min;
            readItemPropertyAs<double, &Json::Value::isNumeric, "double">(&Json::Value::asDouble, min, item, "min", false, itemName);
            std::optional<double> max;
            readItemPropertyAs<double, &Json::Value::isNumeric, "double">(&Json::Value::asDouble, max, item, "max", false, itemName);
            propertyPtr = std::make_unique<T>(std::nullopt, std::move(name), std::move(description), std::move(note), std::move(warning), std::move(deprecated), min, max);
        }
        void createItemPropertyString(std::unique_ptr<ItemProperty>& propertyPtr, const Json::Value& item,
                                      const std::string& itemName, std::optional<std::string>& name, std::optional<std::string>& description, std::optional<std::string>& note, std::optional<std::string>& warning, std::optional<std::string>& deprecated)
        {
            std::vector<StringValue> enums;
            std::optional<Json::Value> enumsRaw;
            readItemProperty<&Json::Value::isArray, "array">(enumsRaw, item, "enum", false, itemName);
            if (enumsRaw)
                for (const auto& enumItem : *enumsRaw) {
                    if (!enumItem.isString()) {
                        throwEx(InvalidConfigTemplateException(std::format("Invalid config template: {}: enum item is not a string.", itemName)));
                    }
                    enums.emplace_back(enumItem.asString());
                }

            std::optional<boost::regex> pattern;
            std::optional<std::string> patternRaw;
            readItemPropertyAs<std::string, &Json::Value::isString, "string">(&Json::Value::asString, patternRaw, item, "pattern", false, itemName);
            if (patternRaw)
                try {
                    pattern.emplace(*patternRaw);
                } catch (const boost::regex_error& e) {
                    throwEx(InvalidConfigTemplateException(std::format("Invalid config template: {}: invalid regex '{}': {}", itemName, *patternRaw, e.what())));
                }
            propertyPtr = std::make_unique<StringItemProperty>(std::nullopt, name, description, note, warning, deprecated, pattern, enums);
        }

        void createItemPropertyList(std::unique_ptr<ItemProperty>& propertyPtr, const Json::Value& item,
                                    const std::string& itemName, std::optional<std::string>& name, std::optional<std::string>& description, std::optional<std::string>& note, std::optional<std::string>& warning, std::optional<std::string>& deprecated)
        {
            std::unique_ptr<ItemProperty> childType;
            std::optional<Json::Value> childTypeRaw;
            readItemProperty<&Json::Value::isObject, "object">(childTypeRaw, item, "child_type", true, itemName);

            parseItemOnce(childType, *childTypeRaw, itemName + "._childtype");

            propertyPtr = std::make_unique<ListItemProperty>(std::nullopt, name, description, note, warning, deprecated, std::move(childType));
        }
        void createItemPropertyObject(std::unique_ptr<ItemProperty>& propertyPtr, const Json::Value& item,
                                      const std::string& itemName, std::optional<std::string>& name, std::optional<std::string>& description, std::optional<std::string>& note, std::optional<std::string>& warning, std::optional<std::string>& deprecated)
        {
            PropertiesType objectProperties;
            std::optional<Json::Value> propertiesRaw;
            readItemProperty<&Json::Value::isObject, "object">(propertiesRaw, item, "properties", true, itemName);

            for (const auto& key : propertiesRaw->getMemberNames())
                parseItem<false>(objectProperties, (*propertiesRaw)[key], itemName + '.' += key);

            propertyPtr = std::make_unique<ObjectItemProperty>(std::nullopt, name, description, note, warning, deprecated, std::move(objectProperties));
        }
        void createItemPropertyEnum(std::unique_ptr<ItemProperty>& propertyPtr, const Json::Value& item,
                                    const std::string& itemName, std::optional<std::string>& name, std::optional<std::string>& description, std::optional<std::string>& note, std::optional<std::string>& warning, std::optional<std::string>& deprecated)
        {
            std::vector<StringValue> values;
            std::optional<Json::Value> valuesRaw;
            readItemProperty<&Json::Value::isArray, "array">(valuesRaw, item, "values", true, itemName);
            for (const auto& enumItem : *valuesRaw) {
                if (!enumItem.isString()) {
                    throwEx(InvalidConfigTemplateException(std::format("Invalid config template: {}: enum item is not a string.", itemName)));
                }
                values.emplace_back(enumItem.asString());
            }
            propertyPtr = std::make_unique<EnumItemProperty>(std::nullopt, name, description, note, warning, deprecated, values);
        }

        /// Parse validation fields and store them in the ItemProperty.
        void createItemProperty(std::unique_ptr<ItemProperty>& propertyPtr, const Json::Value& item,
                                const std::string& itemName, ValueType type, std::optional<std::string>& name, std::optional<std::string>& description, std::optional<std::string>& note, std::optional<std::string>& warning, std::optional<std::string>& deprecated)
        {
            switch (type) {
            case ValueType::Integer:
                createItemPropertyNumber<IntegerItemProperty>(propertyPtr, item, itemName, name, description, note, warning, deprecated);
                break;
            case ValueType::Decimal:
                createItemPropertyNumber<DecimalItemProperty>(propertyPtr, item, itemName, name, description, note, warning, deprecated);
                break;
            case ValueType::String:
                createItemPropertyString(propertyPtr, item, itemName, name, description, note, warning, deprecated);
                break;
            case ValueType::Boolean:
                propertyPtr = std::make_unique<BooleanItemProperty>(std::nullopt, name, description, note, warning, deprecated);
                break;
            case ValueType::List:
                createItemPropertyList(propertyPtr, item, itemName, name, description, note, warning, deprecated);
                break;
            case ValueType::Object:;
                createItemPropertyObject(propertyPtr, item, itemName, name, description, note, warning, deprecated);
                break;
            case ValueType::Enum:;
                createItemPropertyEnum(propertyPtr, item, itemName, name, description, note, warning, deprecated);
                break;
            default:
                // How can this happen? Must be the cosmic ray.
                throwEx(InvalidConfigTemplateException(std::format("Invalid config template: {}: unknown type '{}'.", itemName, static_cast<int>(type))));
            }
        }

        // Only for parsing list.child_type for now
        // So we remove useless logics
        void parseItemOnce(std::unique_ptr<ItemProperty>& property, const Json::Value& item, const std::string& itemName)
        {
            static const boost::regex itemNameRegex(R"(^[a-zA-Z0-9_]+$)");
            {
                std::vector<std::string> splitRes;
                boost::algorithm::split(splitRes, itemName, util::Expect<char> { '.' });
                if (!boost::regex_match(splitRes.at(splitRes.size() - 1), itemNameRegex)) {
                    throwEx(InvalidConfigTemplateException(std::format("Invalid config template: {}: invalid name.", itemName)));
                }
            }
            ValueType type { };
            parseItemValueType(type, item, itemName);

            if (type == ValueType::Namespace) {
                throwEx(InvalidConfigTemplateException(std::format("Invalid config template: {}: type 'namespace' is not allowed here.", itemName)));
            }

            if (type == ValueType::Button) {
                throwEx(InvalidConfigTemplateException(std::format("Invalid config template: {}: type 'button' is not allowed here.", itemName)));
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
        void parseItem(PropertiesType& property, const Json::Value& item, const std::string& itemName)
        {
            static const boost::regex itemNameRegex(R"(^[a-zA-Z0-9_]+$)");
            std::vector<std::string> itemNameSplit;
            {
                boost::algorithm::split(itemNameSplit, itemName, util::Expect<char> { '.' });
                if (!boost::regex_match(itemNameSplit.at(itemNameSplit.size() - 1), itemNameRegex)) {
                    throwEx(InvalidConfigTemplateException(std::format("Invalid config template: {}: invalid name.", itemName)));
                }
            }

            ValueType type { };
            parseItemValueType(type, item, itemName);

            std::optional<std::string> name;
            readItemPropertyAs<std::string, &Json::Value::isString, "string">(&Json::Value::asString, name, item, "name", false, itemName);

            // Namespace only have field 'name' and 'children'
            if (type == ValueType::Namespace) {
                if constexpr (AllowTypeNamespaceOrButton)
                    return parseItemNamespaceEx(property, item, itemName, name);
                else {
                    throwEx(InvalidConfigTemplateException(std::format("Invalid config template: {}: type 'namespace' is not allowed here.", itemName)));
                }
            }

            std::optional<std::string> description;
            readItemPropertyAs<std::string, &Json::Value::isString, "string">(&Json::Value::asString, description, item, "description", false, itemName);
            std::optional<std::string> note;
            readItemPropertyAs<std::string, &Json::Value::isString, "string">(&Json::Value::asString, note, item, "note", false, itemName);
            std::optional<std::string> warning;
            readItemPropertyAs<std::string, &Json::Value::isString, "string">(&Json::Value::asString, warning, item, "warning", false, itemName);
            std::optional<std::string> deprecated;
            readItemPropertyAs<std::string, &Json::Value::isString, "string">(&Json::Value::asString, deprecated, item, "deprecated", false, itemName);

            // Button does not have 'default' field
            if (type == ValueType::Button) {
                if constexpr (AllowTypeNamespaceOrButton)
                    return parseItemButtonEx(property, item, itemName, name, description, note, warning, deprecated);
                else {
                    throwEx(InvalidConfigTemplateException(std::format("Invalid config template: {}: type 'button' is not allowed here.", itemName)));
                }
            }

            // We parse the default value after because we use the parseValue function to parse.
            // So we need to create the ItemProperty first. And because parseValue valids the value,
            // so we also need to parse validation fields.
            std::unique_ptr<ItemProperty> propertyPtr;
            createItemProperty(propertyPtr, item, itemName, type, name, description, note, warning, deprecated);

            const Json::Value* defaultValueRaw = item.find("default");
            if (!defaultValueRaw) {
                throwEx(InvalidConfigTemplateException(std::format("Invalid config template: {}: missing 'default' value.", itemName)));
            }
            Value defaultValue;

            try {
                parseValue(defaultValue, *propertyPtr, *defaultValueRaw);
            } catch (const BadJsonSettingsValueException& e) {
                throwEx(InvalidConfigTemplateException(std::format("Invalid config template: {}: invalid 'default' value. {}", itemName, e.what())));
            }
            propertyPtr->defaultValue = std::move(defaultValue);

            // itemName here is full name, e.g. my_plugin.objecttypesettings.prop1
            // but we don't want to use the full name to access property in the object
            // so we use the last part of itemNameSplit as key in the property map.
            if constexpr (!AllowTypeNamespaceOrButton) {
                property[itemNameSplit.back()] = std::move(propertyPtr);
                return;
            }

            property[itemName] = std::move(propertyPtr);
        }
    }

    } // namespace ::_d_loadConfigTemplate

    void loadConfigTemplate(const Json::Value& value)
    {
        using namespace _d_loadConfigTemplate;

        if (value.empty()) {
            throwEx(InvalidConfigTemplateException("json is empty."));
        }
        if (!value.isObject()) {
            throwEx(InvalidConfigTemplateException("json is not an object."));
        }
        PropertiesType newProperties;
        for (const auto& key : value.getMemberNames())
            parseItem(newProperties, value[key], key);

        // Add default values to settings
        SettingsType newSettings;
        for (const auto& [pKey, pValue] : newProperties) {
            if (pValue->type() == ValueType::Namespace || pValue->type() == ValueType::Button)
                continue;
            if (pValue->defaultValue)
                newSettings[pKey] = *pValue->defaultValue;
        }

        size_t propLoadedSize = newProperties.size();
        {
            std::lock_guard<std::mutex> lock(propertiesMutex);
            properties.merge(newProperties);
        }
        {
            std::lock_guard<std::mutex> lock(settingsMutex);
            settings.merge(newSettings);
        }
        SPDLOG_INFO("Loaded {} property(ies).", propLoadedSize);
    }

    void registerItemChangedEventListener(const std::function<void(std::string_view, const Value&)>& listener)
    {
        std::lock_guard<std::mutex> lock(itemChangedEventListenersMutex);
        itemChangedEventListeners.emplace_back(listener);
    }
} // namespace TheCalculater::settings