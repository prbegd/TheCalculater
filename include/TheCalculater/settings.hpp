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
#include <QString>
#include <cstdint>
#include <istream>
#include <limits>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

namespace TheCalculater::settings {
    // * below is types
    struct Value;

    using BooleanValue = bool;
    using ListValue = std::vector<Value>;
    using ObjectValue = std::vector<std::pair<std::string, Value>>;
    class StringValue;
    class IntegerValue;
    class DecimalValue;

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

    // * below is API functions

    /**
     * @brief Read a value from settings.
     *
     * @throw TheCalculater::settings::BadSettingsException If the key is not found.
     * @param key The key to read the value from.
     * @return Value The value read from settings.
     */
    THECALC_API Value read(const std::string& key);

    /**
     * @brief Read a boolean value from settings.
     *
     * @throw TheCalculater::settings::BadSettingsException If the key is not found or the value is not a boolean.
     * @param key The key to read the value from.
     * @return BooleanValue The boolean value read from settings.
     */
    THECALC_API BooleanValue readBool(const std::string& key);
    /**
     * @brief Read a list value from settings.
     *
     * @throw TheCalculater::settings::BadSettingsException If the key is not found or the value is not a list.
     * @param key The key to read the value from.
     * @return ListValue The list value read from settings.
     */
    THECALC_API ListValue readList(const std::string& key);
    /**
     * @brief Read an object value from settings.
     *
     * @throw TheCalculater::settings::BadSettingsException If the key is not found or the value is not a object.
     * @param key The key to read the value from.
     * @return ObjectValue The object value read from settings.
     */
    THECALC_API ObjectValue readObject(const std::string& key);
    /**
     * @brief Read a string value from settings.
     *
     * @throw TheCalculater::settings::BadSettingsException If the key is not found or the value is not a string.
     * @param key The key to read the value from.
     * @return StringValue The string value read from settings.
     */
    THECALC_API StringValue readString(const std::string& key);
    /**
     * @brief Read an integer value from settings.
     *
     * @throw TheCalculater::settings::BadSettingsException If the key is not found or the value is not a integer.
     * @param key The key to read the value from.
     * @return IntegerValue The integer value read from settings.
     */
    THECALC_API IntegerValue readInteger(const std::string& key);
    /**
     * @brief Read a decimal value from settings.
     *
     * @throw TheCalculater::settings::BadSettingsException If the key is not found or the value is not a decimal.
     * @param key The key to read the value from.
     * @return DecimalValue The decimal value read from settings.
     */
    THECALC_API DecimalValue readDecimal(const std::string& key);

    /**
     * @brief Write a value to settings.
     *
     * @throw TheCalculater::settings::BadSettingsException If the key is not found or the value type is different from the actual type
     * @param key The key to write the value to.
     * @param value The value to write.
     * @see saveModified()
     */
    THECALC_API void write(const std::string& key, Value value);

    /**
     * @brief Write a boolean value to settings.
     *
     * @throw TheCalculater::settings::BadSettingsException If the key is not found or the actual type is not a boolean.
     * @param key The key to write the value to.
     * @param value The value to write.
     * @see saveModified()
     */
    THECALC_API void writeBool(const std::string& key, BooleanValue value);
    /**
     * @brief Write a list value to settings.
     *
     * @throw TheCalculater::settings::BadSettingsException If the key is not found or the actual type is not a list.
     * @param key The key to write the value to.
     * @param value The value to write.
     * @see saveModified()
     */
    THECALC_API void writeList(const std::string& key, ListValue value);
    /**
     * @brief Write an object value to settings.
     *
     * @throw TheCalculater::settings::BadSettingsException If the key is not found or the actual type is not a object.
     * @param key The key to write the value to.
     * @param value The value to write.
     * @see saveModified()
     */
    THECALC_API void writeObject(const std::string& key, ObjectValue value);
    /**
     * @brief Write a string value to settings.
     *
     * @throw TheCalculater::settings::BadSettingsException If the key is not found or the actual type is not a string.
     * @param key The key to write the value to.
     * @param value The value to write.
     * @see saveModified()
     */
    THECALC_API void writeString(const std::string& key, StringValue value);
    /**
     * @brief Write an integer value to settings.
     *
     * @throw TheCalculater::settings::BadSettingsException If the key is not found or the actual type is not a integer.
     * @param key The key to write the value to.
     * @param value The value to write.
     * @see saveModified()
     */
    THECALC_API void writeInteger(const std::string& key, IntegerValue value);
    /**
     * @brief Write a decimal value to settings.
     *
     * @throw TheCalculater::settings::BadSettingsException If the key is not found or the actual type is not a decimal.
     * @param key The key to write the value to.
     * @param value The value to write.
     * @see saveModified()
     */
    THECALC_API void writeDecimal(const std::string& key, DecimalValue value);

    /**
     * @brief Get the type of a value in settings.
     *
     * @param key The key to read the type from.
     * @return ValueType The type of the value.
     */
    THECALC_API ValueType typeOf(const std::string& key);
    /**
     * @brief Get the default value of a key.
     * 
     * @param key The key to read the default value from.
     * @return Value The default value of the key.
     */
    THECALC_API Value defaultValue(const std::string& key);

    /**
     * @brief Get all the keys that have been modified (using write()) but not saved (to file) yet.
     *
     * @return std::vector<std::string> A list of all modified keys.
     */
    THECALC_API std::vector<std::string> modifiedKeys();
    /**
     * @brief Set settings file path.
     *
     * This function will specify where saveModified() and readSettingsFromFile() will save to.
     *
     * @param path The path to the settings file.
     */
    THECALC_API void setSettingsFilePath(const std::string& path);
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
     * This function is as same as readSettings(std::ifstream& stream) but it will open the file specified by setSettingsFilePath().
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
     * @param stream The stream to read from.
     * @return Whether the operation succeeded.
     */
    THECALC_API bool loadConfigTemplate(const std::istream& stream);

    /**
     * @brief Register a button clicked event listener.
     *
     * @param listener The listener to register. Parameter std::string_view is the button path.
     */
    THECALC_API void registerButtonClickedEventListener(const std::function<void(std::string_view)>& listener);
    /**
     * @brief Register an item changed event listener.
     *
     * @param listener The listener to register. Parameter std::string_view is the item path (key), and Value is the new value.
     */
    THECALC_API void registerItemChangedEventListener(const std::function<void(std::string_view, Value)>& listener);

    // * below is detailed config value classes

    /**
     * @brief A class representing a string value in settings.
     */
    class StringValue {
    private:
        const std::string value;

    public:
        explicit StringValue(std::string val)
            : value(std::move(val))
        { }
        explicit StringValue(const QString& val)
            : value(val.toStdString())
        { }

        [[nodiscard]] std::string string() const { return value; }
        [[nodiscard]] QString qString() const { return QString::fromStdString(value); }

        operator std::string() const { return value; }
        operator QString() const { return QString::fromStdString(value); }
    };

    class IntegerValue {
    private:
        const boost::multiprecision::cpp_int value;

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
        const TheCalculater::math::_fraction value;

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
                       IntegerValue, DecimalValue> { };

} // namespace TheCalculater::settings