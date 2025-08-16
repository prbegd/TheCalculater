/**
 * @file settings.hpp
 * @author prbegd
 * @brief Provides settings for TheCalculater.
 * @date 2025-07-30
 *
 * Copyright © 2025 prbegd & TheCalculater contributors
 * Licensed under the MIT License. See LICENSE in the project root for license information.
 *
 */

#include "TheCalculater/core.hpp"
#include "TheCalculater/math/detail.hpp"
#include "core.hpp"
#include <QString>
#include <cstdint>
#include <istream>
#include <limits>
#include <memory>
#include <optional>
#include <regex>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>

namespace Json {
    class Value;
}

namespace TheCalculater::settings {
    THECALC_API void dbginit();

    // * below is types
    struct Value;

    using BooleanValue = bool;
    using ListValue = std::vector<Value>;
    using ObjectValue = std::vector<std::pair<std::string, Value>>;

    THECALCULATER_DEFINE_EXCEPTION(BadSettingsException, std::logic_error);

    enum class ValueType {
        Namespace,
        Integer,
        Decimal,
        String,
        Boolean,
        List,
        Object,
        Enum,
        Button
    };

    /**
     * @brief A class representing a string value in settings.
     */
    class StringValue {
    private:
        std::string value;

    public:
        explicit StringValue(std::string val)
            : value(std::move(val))
        { }
        explicit StringValue(const QString& val)
            : value(val.toStdString())
        { }
        explicit StringValue(const char* val)
            : value(val)
        { }

        [[nodiscard]] std::string string() const { return value; }
        [[nodiscard]] QString qString() const { return QString::fromStdString(value); }

        operator std::string() const { return value; }
        operator QString() const { return QString::fromStdString(value); }
    };

    class IntegerValue {
    private:
        boost::multiprecision::cpp_int value;

    public:
        explicit IntegerValue(boost::multiprecision::cpp_int val)
            : value(std::move(val))
        { }
        explicit IntegerValue(const TheCalculater::math::_fraction& val)
            : value(val.numerator() / val.denominator())
        { }
        template <typename T>
        explicit IntegerValue(T val)
            requires(std::numeric_limits<T>::is_integer)
            : value(val)
        { }

        static IntegerValue fromString(const std::string& str)
        {
            return IntegerValue(boost::multiprecision::cpp_int(str));
        }

        [[nodiscard]] boost::multiprecision::cpp_int cpp_int() const { return value; }
        [[nodiscard]] TheCalculater::math::_fraction fraction() const { return value; }
        [[nodiscard]] int8_t int8() const { return value.convert_to<int8_t>(); }
        [[nodiscard]] int16_t int16() const { return value.convert_to<int16_t>(); }
        [[nodiscard]] int32_t int32() const { return value.convert_to<int32_t>(); }
        [[nodiscard]] int64_t int64() const { return value.convert_to<int64_t>(); }
        [[nodiscard]] uint8_t uint8() const { return value.convert_to<uint8_t>(); }
        [[nodiscard]] uint16_t uint16() const { return value.convert_to<uint16_t>(); }
        [[nodiscard]] uint32_t uint32() const { return value.convert_to<uint32_t>(); }
        [[nodiscard]] uint64_t uint64() const { return value.convert_to<uint64_t>(); }

        [[nodiscard]] float fp32() const { return value.convert_to<float>(); }
        [[nodiscard]] double fp64() const { return value.convert_to<double>(); }
        /// @warning The bit-size of long double is platform dependent.
        [[nodiscard]] long double fp128() const { return value.convert_to<long double>(); }

        operator boost::multiprecision::cpp_int() const { return value; }
        operator TheCalculater::math::_fraction() const { return value; }
        operator int8_t() const { return value.convert_to<int8_t>(); }
        operator int16_t() const { return value.convert_to<int16_t>(); }
        operator int32_t() const { return value.convert_to<int32_t>(); }
        operator int64_t() const { return value.convert_to<int64_t>(); }
        operator uint8_t() const { return value.convert_to<uint8_t>(); }
        operator uint16_t() const { return value.convert_to<uint16_t>(); }
        operator uint32_t() const { return value.convert_to<uint32_t>(); }
        operator uint64_t() const { return value.convert_to<uint64_t>(); }

        operator float() const { return value.convert_to<float>(); }
        operator double() const { return value.convert_to<double>(); }
        operator long double() const { return value.convert_to<long double>(); }
    };

    class DecimalValue {
    private:
        TheCalculater::math::_fraction value;

    public:
        explicit DecimalValue(TheCalculater::math::_fraction val)
            : value(std::move(val))
        { }
        explicit DecimalValue(const boost::multiprecision::cpp_int& val)
            : value(val)
        { }
        template <typename T>
        explicit DecimalValue(T val)
            requires(std::numeric_limits<T>::is_integer)
            : value(val)
        { }
        template <typename T>
        explicit DecimalValue(T val)
            requires(std::is_floating_point_v<T>)
            : value(TheCalculater::math::fraction_convertor::parseFloat(val))
        { }

        static DecimalValue fromString(std::string str)
        {
            return DecimalValue(TheCalculater::math::fraction_convertor::parseDecimal(std::move(str)));
        }

        [[nodiscard]] TheCalculater::math::_fraction fraction() const { return value; }
        [[nodiscard]] float fp32() const { return value.numerator().convert_to<float>() / value.denominator().convert_to<float>(); }
        [[nodiscard]] double fp64() const { return value.numerator().convert_to<double>() / value.denominator().convert_to<double>(); }
        /// @warning The bit-size of long double is platform dependent.
        [[nodiscard]] long double fp128() const { return value.numerator().convert_to<long double>() / value.denominator().convert_to<long double>(); }

        [[nodiscard]] boost::multiprecision::cpp_int cpp_int() const { return value.numerator() / value.denominator(); }
        [[nodiscard]] int8_t int8() const { return static_cast<int8_t>(value.numerator().convert_to<int32_t>() / value.denominator().convert_to<int32_t>()); }
        [[nodiscard]] int16_t int16() const { return static_cast<int16_t>(value.numerator().convert_to<int32_t>() / value.denominator().convert_to<int32_t>()); }
        [[nodiscard]] int32_t int32() const { return value.numerator().convert_to<int32_t>() / value.denominator().convert_to<int32_t>(); }
        [[nodiscard]] int64_t int64() const { return value.numerator().convert_to<int64_t>() / value.denominator().convert_to<int64_t>(); }
        [[nodiscard]] uint8_t uint8() const { return value.numerator().convert_to<uint8_t>() / value.denominator().convert_to<uint8_t>(); }
        [[nodiscard]] uint16_t uint16() const { return value.numerator().convert_to<uint16_t>() / value.denominator().convert_to<uint16_t>(); }
        [[nodiscard]] uint32_t uint32() const { return value.numerator().convert_to<uint32_t>() / value.denominator().convert_to<uint32_t>(); }
        [[nodiscard]] uint64_t uint64() const { return value.numerator().convert_to<uint64_t>() / value.denominator().convert_to<uint64_t>(); }

        operator TheCalculater::math::_fraction() const { return value; }
        operator float() const { return value.numerator().convert_to<float>() / value.denominator().convert_to<float>(); }
        operator double() const { return value.numerator().convert_to<double>() / value.denominator().convert_to<double>(); }
        operator long double() const { return value.numerator().convert_to<long double>() / value.denominator().convert_to<long double>(); }

        operator boost::multiprecision::cpp_int() const { return value.numerator() / value.denominator(); }
        operator int8_t() const { return static_cast<int8_t>(value.numerator().convert_to<int32_t>() / value.denominator().convert_to<int32_t>()); }
        operator int16_t() const { return static_cast<int16_t>(value.numerator().convert_to<int32_t>() / value.denominator().convert_to<int32_t>()); }
        operator int32_t() const { return value.numerator().convert_to<int32_t>() / value.denominator().convert_to<int32_t>(); }
        operator int64_t() const { return value.numerator().convert_to<int64_t>() / value.denominator().convert_to<int64_t>(); }
        operator uint8_t() const { return value.numerator().convert_to<uint8_t>() / value.denominator().convert_to<uint8_t>(); }
        operator uint16_t() const { return value.numerator().convert_to<uint16_t>() / value.denominator().convert_to<uint16_t>(); }
        operator uint32_t() const { return value.numerator().convert_to<uint32_t>() / value.denominator().convert_to<uint32_t>(); }
        operator uint64_t() const { return value.numerator().convert_to<uint64_t>() / value.denominator().convert_to<uint64_t>(); }
    };

    struct Value : std::variant<BooleanValue, ListValue, ObjectValue, StringValue,
                       IntegerValue, DecimalValue> {
        [[nodiscard]] bool isBool() const noexcept { return index() == 0; }
        [[nodiscard]] bool isList() const noexcept { return index() == 1; }
        [[nodiscard]] bool isObject() const noexcept { return index() == 2; }
        [[nodiscard]] bool isString() const noexcept { return index() == 3; }
        [[nodiscard]] bool isInteger() const noexcept { return index() == 4; }
        [[nodiscard]] bool isDecimal() const noexcept { return index() == 5; }

        [[nodiscard]] BooleanValue toBool() const { return std::get<BooleanValue>(*this); }
        [[nodiscard]] ListValue toList() const { return std::get<ListValue>(*this); }
        [[nodiscard]] ObjectValue toObject() const { return std::get<ObjectValue>(*this); }
        [[nodiscard]] StringValue toString() const { return std::get<StringValue>(*this); }
        [[nodiscard]] IntegerValue toInteger() const { return std::get<IntegerValue>(*this); }
        [[nodiscard]] DecimalValue toDecimal() const { return std::get<DecimalValue>(*this); }

        // The type of value. Not item type.
        [[nodiscard]] std::string type() const noexcept;
    };

    struct ItemProperty {
        virtual ~ItemProperty() = default;
        ItemProperty(const ItemProperty& other) = default;
        ItemProperty& operator=(const ItemProperty& other) = default;
        ItemProperty(ItemProperty&& other) = default;
        ItemProperty& operator=(ItemProperty&& other) = default;

        ItemProperty(const std::optional<Value>& defaultValue,
            const std::optional<std::string>& name,
            const std::optional<std::string>& description,
            const std::optional<std::string>& note,
            const std::optional<std::string>& warning,
            bool deprecated)
            : defaultValue(defaultValue), name(name), description(description), note(note), warning(warning), deprecated(deprecated)
        { }
        ItemProperty()
            : defaultValue(std::nullopt), name(std::nullopt), description(std::nullopt), note(std::nullopt), warning(std::nullopt), deprecated(false)
        { }

        // This field is valueless only if this object is obtained from ListItemProperty.childType or the type of this object is "button" or "namespace".
        std::optional<Value> defaultValue;
        std::optional<std::string> name; // translation key
        std::optional<std::string> description; // translation key, empty when type is "namespace"
        std::optional<std::string> note; // translation key, empty when type is "namespace"
        std::optional<std::string> warning; // translation key, empty when type is "namespace"
        bool deprecated; // always false when type is "namespace"

        // fuck rtti.
        // The type of item. Not value type.
        [[nodiscard]] virtual ValueType type() const noexcept = 0;
    };
    // Can only get property "name"
    struct NamespaceItemProperty : ItemProperty {
        [[nodiscard]] ValueType type() const noexcept override { return ValueType::Namespace; }
    };
    struct NumberItemProperty : ItemProperty {
        std::optional<double> min;
        std::optional<double> max;

        NumberItemProperty(const std::optional<Value>& defaultValue,
            const std::optional<std::string>& name,
            const std::optional<std::string>& description,
            const std::optional<std::string>& note,
            const std::optional<std::string>& warning,
            bool deprecated,
            std::optional<double> min = {},
            std::optional<double> max = {})
            : ItemProperty(defaultValue, name, description, note, warning, deprecated), min(min), max(max)
        { }
        NumberItemProperty()
            : min({}), max({})
        { }
    };
    struct IntegerItemProperty : NumberItemProperty {
        [[nodiscard]] ValueType type() const noexcept override { return ValueType::Integer; }
    };
    struct DecimalItemProperty : NumberItemProperty {
        [[nodiscard]] ValueType type() const noexcept override { return ValueType::Decimal; }
    };
    struct StringItemProperty : ItemProperty {
        std::vector<StringValue> enums;
        std::optional<std::regex> pattern;
        [[nodiscard]] ValueType type() const noexcept override { return ValueType::String; }

        StringItemProperty(const std::optional<Value>& defaultValue,
            const std::optional<std::string>& name,
            const std::optional<std::string>& description,
            const std::optional<std::string>& note,
            const std::optional<std::string>& warning,
            bool deprecated,
            std::optional<std::regex> pattern = {},
            const std::vector<StringValue>& enums0 = {})
            : ItemProperty(defaultValue, name, description, note, warning, deprecated), enums(enums0), pattern(std::move(pattern))
        { }
        StringItemProperty() = default;
    };
    struct ListItemProperty : ItemProperty {
        std::unique_ptr<ItemProperty> childType;
        [[nodiscard]] ValueType type() const noexcept override { return ValueType::List; }

        ListItemProperty(const std::optional<Value>& defaultValue,
            const std::optional<std::string>& name,
            const std::optional<std::string>& description,
            const std::optional<std::string>& note,
            const std::optional<std::string>& warning,
            bool deprecated,
            std::unique_ptr<ItemProperty> childType)
            : ItemProperty(defaultValue, name, description, note, warning, deprecated), childType(std::move(childType))
        { }
    };
    struct ObjectItemProperty : ItemProperty {
        std::unordered_map<std::string, std::unique_ptr<ItemProperty>> properties;
        [[nodiscard]] ValueType type() const noexcept override { return ValueType::Object; }

        ObjectItemProperty(const std::optional<Value>& defaultValue,
            const std::optional<std::string>& name,
            const std::optional<std::string>& description,
            const std::optional<std::string>& note,
            const std::optional<std::string>& warning,
            bool deprecated,
            const std::unordered_map<std::string, std::unique_ptr<ItemProperty>>& properties)
            : ItemProperty(defaultValue, name, description, note, warning, deprecated), properties(properties)
        { }
    };
    struct EnumItemProperty : ItemProperty {
        std::vector<StringValue> values;
        [[nodiscard]] ValueType type() const noexcept override { return ValueType::Enum; }

        EnumItemProperty(const std::optional<Value>& defaultValue,
            const std::optional<std::string>& name,
            const std::optional<std::string>& description,
            const std::optional<std::string>& note,
            const std::optional<std::string>& warning,
            bool deprecated,
            const std::vector<StringValue>& values)
            : ItemProperty(defaultValue, name, description, note, warning, deprecated), values(values)
        {
            if (values.empty())
                throw std::invalid_argument("EnumItemProperty: values cannot be empty");
        }
    };
    struct ButtonItemProperty : ItemProperty {
        std::string text; // translation key
        [[nodiscard]] ValueType type() const noexcept override { return ValueType::Button; }

        ButtonItemProperty(const std::optional<Value>& defaultValue,
            const std::optional<std::string>& name,
            const std::optional<std::string>& description,
            const std::optional<std::string>& note,
            const std::optional<std::string>& warning,
            bool deprecated,
            std::string text)
            : ItemProperty(defaultValue, name, description, note, warning, deprecated), text(std::move(text))
        { }
    };

    // * below is API functions

    /**
     * @brief Read a value from settings.
     *
     * @throw TheCalculater::settings::BadSettingsException If the key is not found.
     * @param key The key to read the value from.
     * @return Value The value read from settings.
     */
    THECALC_API Value read(std::string_view key);

    /**
     * @brief Read a boolean value from settings.
     *
     * @throw TheCalculater::settings::BadSettingsException If the key is not found or the value is not a boolean.
     * @param key The key to read the value from.
     * @return BooleanValue The boolean value read from settings.
     */
    THECALC_API BooleanValue readBool(std::string_view key);
    /**
     * @brief Read a list value from settings.
     *
     * @throw TheCalculater::settings::BadSettingsException If the key is not found or the value is not a list.
     * @param key The key to read the value from.
     * @return ListValue The list value read from settings.
     */
    THECALC_API ListValue readList(std::string_view key);
    /**
     * @brief Read an object value from settings.
     *
     * @throw TheCalculater::settings::BadSettingsException If the key is not found or the value is not a object.
     * @param key The key to read the value from.
     * @return ObjectValue The object value read from settings.
     */
    THECALC_API ObjectValue readObject(std::string_view key);
    /**
     * @brief Read a string value from settings.
     *
     * @throw TheCalculater::settings::BadSettingsException If the key is not found or the value is not a string.
     * @param key The key to read the value from.
     * @return StringValue The string value read from settings.
     */
    THECALC_API StringValue readString(std::string_view key);
    /**
     * @brief Read an integer value from settings.
     *
     * @throw TheCalculater::settings::BadSettingsException If the key is not found or the value is not a integer.
     * @param key The key to read the value from.
     * @return IntegerValue The integer value read from settings.
     */
    THECALC_API IntegerValue readInteger(std::string_view key);
    /**
     * @brief Read a decimal value from settings.
     *
     * @throw TheCalculater::settings::BadSettingsException If the key is not found or the value is not a decimal.
     * @param key The key to read the value from.
     * @return DecimalValue The decimal value read from settings.
     */
    THECALC_API DecimalValue readDecimal(std::string_view key);

    /**
     * @brief Write a value to settings.
     *
     * @throw TheCalculater::settings::BadSettingsException If the key is not found or the value type is different from the actual type
     * @param key The key to write the value to.
     * @param value The value to write.
     * @see saveModified()
     */
    THECALC_API void write(std::string_view key, const Value& value);

    /**
     * @brief Write a boolean value to settings.
     *
     * @throw TheCalculater::settings::BadSettingsException If the key is not found or the actual type is not a boolean.
     * @param key The key to write the value to.
     * @param value The value to write.
     * @see saveModified()
     */
    THECALC_API void writeBool(std::string_view key, const BooleanValue& value);
    /**
     * @brief Write a list value to settings.
     *
     * @throw TheCalculater::settings::BadSettingsException If the key is not found or the actual type is not a list.
     * @param key The key to write the value to.
     * @param value The value to write.
     * @see saveModified()
     */
    THECALC_API void writeList(std::string_view key, const ListValue& value);
    /**
     * @brief Write an object value to settings.
     *
     * @throw TheCalculater::settings::BadSettingsException If the key is not found or the actual type is not a object.
     * @param key The key to write the value to.
     * @param value The value to write.
     * @see saveModified()
     */
    THECALC_API void writeObject(std::string_view key, const ObjectValue& value);
    /**
     * @brief Write a string value to settings.
     *
     * @throw TheCalculater::settings::BadSettingsException If the key is not found or the actual type is not a string.
     * @param key The key to write the value to.
     * @param value The value to write.
     * @see saveModified()
     */
    THECALC_API void writeString(std::string_view key, const StringValue& value);
    /**
     * @brief Write an integer value to settings.
     *
     * @throw TheCalculater::settings::BadSettingsException If the key is not found or the actual type is not a integer.
     * @param key The key to write the value to.
     * @param value The value to write.
     * @see saveModified()
     */
    THECALC_API void writeInteger(std::string_view key, const IntegerValue& value);
    /**
     * @brief Write a decimal value to settings.
     *
     * @throw TheCalculater::settings::BadSettingsException If the key is not found or the actual type is not a decimal.
     * @param key The key to write the value to.
     * @param value The value to write.
     * @see saveModified()
     */
    THECALC_API void writeDecimal(std::string_view key, const DecimalValue& value);

    /**
     * @brief Get the property of a key in settings.
     *
     * @param key The key to read the property from.
     * @return ItemProperty& The property of the key.
     */
    THECALC_API ItemProperty& property(std::string_view key);
    /**
     * @brief Get the type of a value in settings.
     *
     * @param key The key to read the type from.
     * @return ValueType The type of the value.
     * @see property()
     */
    inline ValueType typeOf(std::string_view key) { return property(key).type(); }

    /**
     * @brief Get the default value of a key.
     *
     * @param key The key to read the default value from.
     * @return Value The default value of the key.
     * @see property()
     */
    inline Value defaultValue(std::string_view key) { return property(key).defaultValue.value(); }

    /**
     * @brief Get all the keys that have been modified (using write()) but not saved (to file) yet.
     *
     * @return std::vector<std::string> A list of all modified keys.
     */
    THECALC_API const std::vector<std::string>& modifiedKeys();
    /**
     * @brief Set settings file path.
     *
     * This function will specify where saveModified() and parseSettingsFromFile() will save to.
     *
     * @param path The path to the settings file.
     */
    THECALC_API void setSettingsFilePath(std::string_view path);
    /**
     * @brief Save all modified settings to file.
     *
     * For every key that has been modified, we do the following:
     * - If the key doesn't specify in file, we append this key and value to the end of the file.
     * - If the key specifies in file, we replace it with this new value.
     * - If the key is a number(integer or decimal), we use string form to save it.
     *
     * This function will save to the file specified by setSettingsFilePath(). If no path is specified, it will return false.
     *
     * @warning If you don't call this function, your changes will be lost when the program exits.
     * @see setSettingsFilePath()
     * @see modifiedKeys()
     * @return Whether the operation succeeded.
     */
    THECALC_API bool saveModified();
    /**
     * @brief Parse settings from a stream.
     *
     * This function will overwrite all existing settings.
     * This function must be called after loadConfigTemplate() is called. (Otherwise, the parsing will all fail!)
     *
     * @param stream the stream to read from.
     * @param error A list of errors that occurred during parsing.
     * @return Whether the operation succeeded.
     */
    THECALC_API bool parseSettings(const std::istream& stream, std::vector<std::string, std::string>& errors);
    /**
     * @brief Parse settings from file.
     *
     * This function is as same as readSettings(std::ifstream& stream) but it will open the file specified by setSettingsFilePath(). If no path is specified, it will return false.
     *
     * @see setSettingsFilePath()
     * @param error A list of errors that occurred during parsing.
     * @return Whether the operation succeeded.
     */
    THECALC_API bool parseSettingsFromFile(std::vector<std::string, std::string>& errors);

    /**
     * @brief Load a config template from stream.
     *
     * This must be called before parseSettings() or parseSettingsFromFile().
     *
     * @see parseSettings()
     * @see parseSettingsFromFile()
     * @param value The config template to load.
     * @return Whether the operation succeeded.
     */
    THECALC_API bool loadConfigTemplate(const Json::Value& value);

    /**
     * @brief Register a button clicked event listener.
     *
     * TODO: Consider to implement this function is SettingsWidget.
     *
     * @param listener The listener to register. Parameter std::string_view is the button path.
     */
    THECALC_API void registerButtonClickedEventListener(const std::function<void(std::string_view)>& listener);
    /**
     * @brief Register an item changed event listener.
     *
     * @param listener The listener to register. Parameter std::string_view is the item path (key), and Value is the new value.
     */
    THECALC_API void registerItemChangedEventListener(const std::function<void(std::string_view, const Value&)>& listener);

} // namespace TheCalculater::settings