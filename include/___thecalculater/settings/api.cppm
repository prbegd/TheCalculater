/**
 * @file api.cppm
 * @author prbegd
 * @date 2026-03-15
 *
 * Copyright © 2026 Cai Yaoxing
 *
 * This file is part of TheCalculater.
 * TheCalculater is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * TheCalculater is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with TheCalculater. If not, see <https://www.gnu.org/licenses/>.
 */
module;
#include "thecalculater/macros.hpp"

export module thecalculater.settings.api;
import thecalculater.settings.value;
import thecalculater.settings.property;
import thecalculater.util;
import tpmm.jsoncpp;
import std;

namespace thecalculater::settings {
    /**
     * @brief Read a value from settings.
     *
     * @throw thecalculater::settings::SettingsKeyNotFoundException If the key is not found.
     * @param key The key to read the value from.
     * @return Value The value read from settings.
     */
    export TCAPI Value read(std::string_view key);

    /**
     * @brief Read a boolean value from settings.
     *
     * @throw thecalculater::settings::SettingsKeyNotFoundException If the key is not found.
     * @throw thecalculater::settings::BadSettingsException If the value is not a boolean.
     * @param key The key to read the value from.
     * @return BooleanValue The boolean value read from settings.
     * @see read()
     */
    export TCAPI BooleanValue readBool(std::string_view key);
    /**
     * @brief Read a list value from settings.
     *
     * @throw thecalculater::settings::SettingsKeyNotFoundException If the key is not found.
     * @throw thecalculater::settings::BadSettingsException If the value is not a list.
     * @param key The key to read the value from.
     * @return ListValue The list value read from settings.
     * @see read()
     */
    export TCAPI ListValue readList(std::string_view key);
    /**
     * @brief Read an object value from settings.
     *
     * @throw thecalculater::settings::SettingsKeyNotFoundException If the key is not found.
     * @throw thecalculater::settings::BadSettingsException If the value is not an object.
     * @param key The key to read the value from.
     * @return ObjectValue The object value read from settings.
     * @see read()
     */
    export TCAPI ObjectValue readObject(std::string_view key);
    /**
     * @brief Read a string value from settings.
     *
     * @throw thecalculater::settings::SettingsKeyNotFoundException If the key is not found.
     * @throw thecalculater::settings::BadSettingsException If the value is not a string.
     * @param key The key to read the value from.
     * @return StringValue The string value read from settings.
     * @see read()
     */
    export TCAPI StringValue readString(std::string_view key);
    /**
     * @brief Read an integer value from settings.
     *
     * @throw thecalculater::settings::SettingsKeyNotFoundException If the key is not found.
     * @throw thecalculater::settings::BadSettingsException If the value is not a integer.
     * @param key The key to read the value from.
     * @return IntegerValue The integer value read from settings.
     * @see read()
     */
    export TCAPI IntegerValue readInteger(std::string_view key);
    /**
     * @brief Read a decimal value from settings.
     *
     * @throw thecalculater::settings::SettingsKeyNotFoundException If the key is not found.
     * @throw thecalculater::settings::BadSettingsException If the value is not a decimal.
     * @param key The key to read the value from.
     * @return DecimalValue The decimal value read from settings.
     * @see read()
     */
    export TCAPI DecimalValue readDecimal(std::string_view key);

    /**
     * @brief Write a value to settings.
     *
     * @throw thecalculater::settings::SettingsKeyNotFoundException If the key is not found.
     * @throw thecalculater::settings::BadSettingsException If value type is different from the actual type.
     * @param key The key to write the value to.
     * @param value The value to write.
     * @see saveModified()
     */
    export TCAPI void write(std::string_view key, const Value& value);

    /**
     * @brief Write a boolean value to settings.
     *
     * @throw thecalculater::settings::SettingsKeyNotFoundException If the key is not found.
     * @throw thecalculater::settings::BadSettingsException If the actual type is not a boolean.
     * @param key The key to write the value to.
     * @param value The value to write.
     * @see saveModified()
     * @see write()
     */
    export TCAPI void writeBool(std::string_view key, const BooleanValue& value);
    /**
     * @brief Write a list value to settings.
     *
     * @throw thecalculater::settings::SettingsKeyNotFoundException If the key is not found.
     * @throw thecalculater::settings::BadSettingsException If the actual type is not a list.
     * @param key The key to write the value to.
     * @param value The value to write.
     * @see saveModified()
     * @see write()
     */
    export TCAPI void writeList(std::string_view key, const ListValue& value);
    /**
     * @brief Write an object value to settings.
     *
     * @throw thecalculater::settings::SettingsKeyNotFoundException If the key is not found.
     * @throw thecalculater::settings::BadSettingsException If the actual type is not an object.
     * @param key The key to write the value to.
     * @param value The value to write.
     * @see saveModified()
     * @see write()
     */
    export TCAPI void writeObject(std::string_view key, const ObjectValue& value);
    /**
     * @brief Write a string value to settings.
     *
     * @throw thecalculater::settings::SettingsKeyNotFoundException If the key is not found.
     * @throw thecalculater::settings::BadSettingsException If the actual type is not a string.
     * @param key The key to write the value to.
     * @param value The value to write.
     * @see saveModified()
     * @see write()
     */
    export TCAPI void writeString(std::string_view key, const StringValue& value);
    /**
     * @brief Write an integer value to settings.
     *
     * @throw thecalculater::settings::SettingsKeyNotFoundException If the key is not found.
     * @throw thecalculater::settings::BadSettingsException If the actual type is not an integer.
     * @param key The key to write the value to.
     * @param value The value to write.
     * @see saveModified()
     * @see write()
     */
    export TCAPI void writeInteger(std::string_view key, const IntegerValue& value);
    /**
     * @brief Write a decimal value to settings.
     *
     * @throw thecalculater::settings::SettingsKeyNotFoundException If the key is not found.
     * @throw thecalculater::settings::BadSettingsException If the actual type is not a decimal.
     * @param key The key to write the value to.
     * @param value The value to write.
     * @see saveModified()
     * @see write()
     */
    export TCAPI void writeDecimal(std::string_view key, const DecimalValue& value);

    /**
     * @brief Get the property of a key in settings.
     *
     * @param key The key to read the property from.
     * @return ItemProperty& The property of the key.
     * @throw thecalculater::settings::SettingsKeyNotFoundException If the key doesn't exists.
     */
    export TCAPI ItemProperty& property(std::string_view key);
    /**
     * @brief Get the type of a value in settings.
     *
     * @param key The key to read the type from.
     * @return ValueType The type of the value.
     * @see property()
     * @throw thecalculater::settings::SettingsKeyNotFoundException If the key doesn't exists.
     */
    export inline ValueType typeOf(std::string_view key) { return property(key).type(); }

    /**
     * @brief Get the default value of a key.
     *
     * @param key The key to read the default value from.
     * @return Value The default value of the key.
     * @see property()
     * @throw thecalculater::settings::SettingsKeyNotFoundException If the key doesn't exists.
     * @throw std::bad_optional_access If the key doesn't have a default value.
     */
    export inline Value defaultValue(std::string_view key) { return *property(key).defaultValue; }

    /**
     * @brief Get all the keys that have been modified (using write()) but not saved (to file) yet.
     *
     * @return std::vector<std::string> A list of all modified keys.
     */
    export TCAPI const std::vector<std::string>& modifiedKeys();
    /**
     * @brief Set settings file path.
     *
     * This function will specify where saveModified() and parseSettings() will save to by default.
     *
     * @param path The path to the settings file.
     */
    export TCAPI void setSettingsFilePath(std::string_view path);
    /**
     * @brief Get settings file path.
     *
     * @return std::weak_ptr<std::string> The path to the settings file. If no path is set, it will return an empty weak pointer.
     */
    export TCAPI std::weak_ptr<std::string> getSettingsFilePath();

    /**
     * @brief Save all modified settings to file.
     *
     * If one of the modified key is a number (integer or decimal), we use string form to save it.
     * This function will not overwrite original file settings.
     * This function will re-serialize the file, which will remove all comments.
     *
     * @warning If you don't call this function, your changes will be lost when the program exits.
     * @see modifiedKeys()
     * @throw thecalculater::core::IOException If the file cannot be opened.
     */
    export TCAPI void saveModified(const std::string& fileName = util::value_or(getSettingsFilePath(), { }));
    /**
     * @brief Parse settings from json.
     *
     * This function must be called after loadConfigTemplate() is called. (Otherwise, the parsing will all fail!)
     *
     * @param json The JSON value to parse.
     * @param error A list of errors that occurred during parsing.
     * @throw std::invalid_argument If the JSON value is not an object.
     */
    export TCAPI void parseSettings(const Json::Value& json, std::unordered_map<std::string, std::string>& errors);
    /// overload for default settings file path
    /// If the file doesn't exist, it will create a new file
    /// witt a pair of empty braces.
    /// @throw thecalculater::core::IOException If the file cannot be opened.
    /// @throw thecalculater::core::WeakPointerExpiredException If the settings file path pointer is expired.
    export TCAPI void parseSettings(std::unordered_map<std::string, std::string>& errors);

    /**
     * @brief Parse a value in JSON to a Value.
     *
     * This function will check if the value is valid for the property.
     *
     * @param result The result value.
     * @param property The property of the value to parse.
     * @param item The JSON value to parse.
     * @throw thecalculater::settings::BadJsonSettingsValueException If the value is invalid.
     */
    export TCAPI void parseValue(Value& result, const ItemProperty& property, const Json::Value& item);
    /**
     * @brief Format a Value to JSON.
     *
     * @param result The result JSON value.
     * @param item The value to format.
     * @return Whether the operation succeeded.
     */
    export TCAPI bool formatValue(Json::Value& result, const Value& item);

    /**
     * @brief Load a config template from stream.
     *
     * This is usually called before parseSettings().
     *
     * TODO: add unloadConfigTemplate() function.
     *
     * @see parseSettings()
     * @param value The config template to load.
     * @throw thecalculater::settings::InvalidConfigTemplateException If the config template is invalid.
     */
    export TCAPI void loadConfigTemplate(const Json::Value& value);

    /**
     * @brief Register a button clicked event listener.
     *
     * TODO: Consider to implement this function is SettingsWidget.
     *
     * @param listener The listener to register. Parameter std::string_view is the button path.
     *
     * ! Work In Progress
     */
    export TCAPI void registerButtonClickedEventListener(const std::function<void(std::string_view)>& listener);
    /**
     * @brief Register an item changed event listener.
     *
     * @param listener The listener to register. Parameter std::string_view is the item path (key), and Value is the new value.
     */
    export TCAPI void registerItemChangedEventListener(const std::function<void(std::string_view, const Value&)>& listener);

} // namespace thecalculater::settings
