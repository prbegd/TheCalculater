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
     * @brief Reads a value from settings.
     *
     * @throw TheCalculater::settings::BadSettingsException If the key is not found.
     * @param key The key to read the value from.
     * @return Value The value read from settings.
     */
    THECALC_API Value read(const std::string& key);

    /**
     * @brief Reads a boolean value from settings.
     *
     * @throw TheCalculater::settings::BadSettingsException If the key is not found or the value is not a boolean.
     * @param key The key to read the value from.
     * @return BooleanValue The boolean value read from settings.
     */
    THECALC_API BooleanValue readBool(const std::string& key);
    /**
     * @brief Reads a list value from settings.
     *
     * @throw TheCalculater::settings::BadSettingsException If the key is not found or the value is not a list.
     * @param key The key to read the value from.
     * @return ListValue The list value read from settings.
     */
    THECALC_API ListValue readList(const std::string& key);
    /**
     * @brief Reads an object value from settings.
     *
     * @throw TheCalculater::settings::BadSettingsException If the key is not found or the value is not a object.
     * @param key The key to read the value from.
     * @return ObjectValue The object value read from settings.
     */
    THECALC_API ObjectValue readObject(const std::string& key);
    /**
     * @brief Reads a string value from settings.
     *
     * @throw TheCalculater::settings::BadSettingsException If the key is not found or the value is not a string.
     * @param key The key to read the value from.
     * @return StringValue The string value read from settings.
     */
    THECALC_API StringValue readString(const std::string& key);
    /**
     * @brief Reads an integer value from settings.
     *
     * @throw TheCalculater::settings::BadSettingsException If the key is not found or the value is not a integer.
     * @param key The key to read the value from.
     * @return IntegerValue The integer value read from settings.
     */
    THECALC_API IntegerValue readInteger(const std::string& key);
    /**
     * @brief Reads a decimal value from settings.
     *
     * @throw TheCalculater::settings::BadSettingsException If the key is not found or the value is not a decimal.
     * @param key The key to read the value from.
     * @return DecimalValue The decimal value read from settings.
     */
    THECALC_API DecimalValue readDecimal(const std::string& key);

    /**
     * @brief Writes a value to settings.
     *
     * @throw TheCalculater::settings::BadSettingsException If the key is not found or the value type is different from the actual type
     * @param key The key to write the value to.
     * @param value The value to write.
     */
    THECALC_API void write(const std::string& key, Value value);

    /**
     * @brief Writes a boolean value to settings.
     *
     * @throw TheCalculater::settings::BadSettingsException If the key is not found or the actual type is not a boolean.
     * @param key The key to write the value to.
     * @param value The value to write.
     */
    THECALC_API void writeBool(const std::string& key, BooleanValue value);
    /**
     * @brief Writes a list value to settings.
     *
     * @throw TheCalculater::settings::BadSettingsException If the key is not found or the actual type is not a list.
     * @param key The key to write the value to.
     * @param value The value to write.
     */
    THECALC_API void writeList(const std::string& key, ListValue value);
    /**
     * @brief Writes an object value to settings.
     *
     * @throw TheCalculater::settings::BadSettingsException If the key is not found or the actual type is not a object.
     * @param key The key to write the value to.
     * @param value The value to write.
     */
    THECALC_API void writeObject(const std::string& key, ObjectValue value);
    /**
     * @brief Writes a string value to settings.
     *
     * @throw TheCalculater::settings::BadSettingsException If the key is not found or the actual type is not a string.
     * @param key The key to write the value to.
     * @param value The value to write.
     */
    THECALC_API void writeString(const std::string& key, StringValue value);
    /**
     * @brief Writes an integer value to settings.
     *
     * @throw TheCalculater::settings::BadSettingsException If the key is not found or the actual type is not a integer.
     * @param key The key to write the value to.
     * @param value The value to write.
     */
    THECALC_API void writeInteger(const std::string& key, IntegerValue value);
    /**
     * @brief Writes a decimal value to settings.
     *
     * @throw TheCalculater::settings::BadSettingsException If the key is not found or the actual type is not a decimal.
     * @param key The key to write the value to.
     * @param value The value to write.
     */
    THECALC_API void writeDecimal(const std::string& key, DecimalValue value);

    /**
     * @brief Gets the type of a value in settings.
     * 
     * @param key The key to read the type from.
     * @return ValueType The type of the value.
     */
    THECALC_API ValueType typeOf(const std::string& key);

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
        
        static IntegerValue fromString(const std::string&str)
        { return IntegerValue(boost::multiprecision::cpp_int(str)); }

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
        template<typename T>
        explicit DecimalValue(T val)
            requires(std::numeric_limits<T>::is_integer)
            : value(val)
        { }
        template<typename T>
        explicit DecimalValue(T val)
            requires(std::is_floating_point_v<T>)
            : value(TheCalculater::math::fraction_convertor::parseFloat(val))
        { }

        static DecimalValue fromString(std::string str)
        { return DecimalValue(TheCalculater::math::fraction_convertor::parseDecimal(std::move(str))); }

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