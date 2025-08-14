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
#include <format>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace TheCalculater::settings {
    namespace {
        std::unordered_map<std::string, Value,
            core::Hash<std::string_view>, core::EqualTo<std::string_view>>
            settings;
        std::mutex settingsMutex;

        std::vector<std::string> modifiedKeysValue;
        std::mutex modifiedKeysMutex;

        std::unordered_map<std::string, std::unique_ptr<ItemProperty>,
            core::Hash<std::string_view>, core::EqualTo<std::string_view>>
            properties;
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
            return {}; // fuck you CppCheck for not recognizing my throw function
        }
        return res->second;
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
        return *res->second;
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
