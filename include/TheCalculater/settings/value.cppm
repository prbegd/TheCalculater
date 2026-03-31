/**
 * @file value.cppm
 * @author prbegd
 * @date 2026-03-15
 *
 * Copyright © 2025 Cai Yaoxing
 * SPDX-License-Identifier: GPL-3.0-only
 * This file is part of TheCalculater.
 * See the file LICENSE in the project root or go to
 * <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
 */
module;
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/rational.hpp>

export module TheCalculater.settings.value;
import TheCalculater.math;
import std.compat;

namespace TheCalculater::settings {
    export struct Value;

    export using BooleanValue = bool;
    export using ListValue = std::vector<Value>;
    export using ObjectValue = std::vector<std::pair<std::string, Value>>;

    export enum class ValueType {
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
    export class StringValue {
    private:
        std::string value;

    public:
        explicit StringValue(std::string_view val)
            : value(val)
        { }
        explicit StringValue(const char* val)
            : value(val)
        { }
        StringValue() = default;

        [[nodiscard]] std::string string() const { return value; }
        [[nodiscard]] const std::string& stringRef() const { return value; }

        operator std::string() const { return value; }
        operator const std::string&() const { return value; }

        auto operator<=>(const StringValue& other) const = default;
    };

    export class IntegerValue {
    private:
        boost::multiprecision::cpp_int value;

    public:
        explicit IntegerValue(boost::multiprecision::cpp_int val)
            : value(std::move(val))
        { }
        explicit IntegerValue(const TheCalculater::math::Fraction& val)
            : value(val.numerator() / val.denominator())
        { }
        template <typename T>
        explicit IntegerValue(T val)
            requires(std::numeric_limits<T>::is_integer)
            : value(val)
        { }
        template <typename T>
        explicit IntegerValue(T val)
            requires(std::is_floating_point_v<T>)
            : IntegerValue(static_cast<int64_t>(val))
        { }

        IntegerValue() = default;

        static IntegerValue fromString(const std::string& str)
        {
            return IntegerValue(boost::multiprecision::cpp_int(str));
        }

        [[nodiscard]] boost::multiprecision::cpp_int cpp_int() const { return value; }
        [[nodiscard]] TheCalculater::math::Fraction fraction() const { return value; }
        [[nodiscard]] std::int8_t int8() const { return value.convert_to<std::int8_t>(); }
        [[nodiscard]] int16_t int16() const { return value.convert_to<int16_t>(); }
        [[nodiscard]] int32_t int32() const { return value.convert_to<int32_t>(); }
        [[nodiscard]] int64_t int64() const { return value.convert_to<int64_t>(); }
        [[nodiscard]] std::uint8_t uint8() const { return value.convert_to<std::uint8_t>(); }
        [[nodiscard]] uint16_t uint16() const { return value.convert_to<uint16_t>(); }
        [[nodiscard]] uint32_t uint32() const { return value.convert_to<uint32_t>(); }
        [[nodiscard]] uint64_t uint64() const { return value.convert_to<uint64_t>(); }

        [[nodiscard]] float fp32() const { return value.convert_to<float>(); }
        [[nodiscard]] double fp64() const { return value.convert_to<double>(); }
        /// @warning The bit-size of long double is platform dependent.
        [[nodiscard]] long double fp128() const { return value.convert_to<long double>(); }

        operator boost::multiprecision::cpp_int() const { return value; }
        operator TheCalculater::math::Fraction() const { return value; }
        operator std::int8_t() const { return value.convert_to<std::int8_t>(); }
        operator int16_t() const { return value.convert_to<int16_t>(); }
        operator int32_t() const { return value.convert_to<int32_t>(); }
        operator int64_t() const { return value.convert_to<int64_t>(); }
        operator std::uint8_t() const { return value.convert_to<std::uint8_t>(); }
        operator uint16_t() const { return value.convert_to<uint16_t>(); }
        operator uint32_t() const { return value.convert_to<uint32_t>(); }
        operator uint64_t() const { return value.convert_to<uint64_t>(); }

        operator float() const { return value.convert_to<float>(); }
        operator double() const { return value.convert_to<double>(); }
        operator long double() const { return value.convert_to<long double>(); }

        bool operator<(const IntegerValue& other) const { return value < other.value; }
        bool operator>(const IntegerValue& other) const { return value > other.value; }
        bool operator<=(const IntegerValue& other) const { return value <= other.value; }
        bool operator>=(const IntegerValue& other) const { return value >= other.value; }
        bool operator==(const IntegerValue& other) const { return value == other.value; }
        bool operator!=(const IntegerValue& other) const { return value != other.value; }

        [[nodiscard]] std::string toString() const { return value.str(); }
    };

    export class DecimalValue {
    private:
        TheCalculater::math::Fraction value;

    public:
        explicit DecimalValue(TheCalculater::math::Fraction val)
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
            : value(TheCalculater::math::makeFraction(val))
        { }
        DecimalValue() = default;

        static DecimalValue fromString(const std::string& str)
        {
            return DecimalValue(TheCalculater::math::makeFraction(str));
        }

        [[nodiscard]] TheCalculater::math::Fraction fraction() const { return value; }
        [[nodiscard]] float fp32() const { return value.numerator().convert_to<float>() / value.denominator().convert_to<float>(); }
        [[nodiscard]] double fp64() const { return value.numerator().convert_to<double>() / value.denominator().convert_to<double>(); }
        /// @warning The bit-size of long double is platform dependent.
        [[nodiscard]] long double fp128() const { return value.numerator().convert_to<long double>() / value.denominator().convert_to<long double>(); }

        [[nodiscard]] boost::multiprecision::cpp_int cpp_int() const { return value.numerator() / value.denominator(); }
        [[nodiscard]] std::int8_t int8() const { return static_cast<std::int8_t>(value.numerator().convert_to<int32_t>() / value.denominator().convert_to<int32_t>()); }
        [[nodiscard]] int16_t int16() const { return static_cast<int16_t>(value.numerator().convert_to<int32_t>() / value.denominator().convert_to<int32_t>()); }
        [[nodiscard]] int32_t int32() const { return value.numerator().convert_to<int32_t>() / value.denominator().convert_to<int32_t>(); }
        [[nodiscard]] int64_t int64() const { return value.numerator().convert_to<int64_t>() / value.denominator().convert_to<int64_t>(); }
        [[nodiscard]] std::uint8_t uint8() const { return value.numerator().convert_to<std::uint8_t>() / value.denominator().convert_to<std::uint8_t>(); }
        [[nodiscard]] uint16_t uint16() const { return value.numerator().convert_to<uint16_t>() / value.denominator().convert_to<uint16_t>(); }
        [[nodiscard]] uint32_t uint32() const { return value.numerator().convert_to<uint32_t>() / value.denominator().convert_to<uint32_t>(); }
        [[nodiscard]] uint64_t uint64() const { return value.numerator().convert_to<uint64_t>() / value.denominator().convert_to<uint64_t>(); }

        operator TheCalculater::math::Fraction() const { return value; }
        operator float() const { return value.numerator().convert_to<float>() / value.denominator().convert_to<float>(); }
        operator double() const { return value.numerator().convert_to<double>() / value.denominator().convert_to<double>(); }
        operator long double() const { return value.numerator().convert_to<long double>() / value.denominator().convert_to<long double>(); }

        operator boost::multiprecision::cpp_int() const { return value.numerator() / value.denominator(); }
        operator std::int8_t() const { return static_cast<std::int8_t>(value.numerator().convert_to<int32_t>() / value.denominator().convert_to<int32_t>()); }
        operator int16_t() const { return static_cast<int16_t>(value.numerator().convert_to<int32_t>() / value.denominator().convert_to<int32_t>()); }
        operator int32_t() const { return value.numerator().convert_to<int32_t>() / value.denominator().convert_to<int32_t>(); }
        operator int64_t() const { return value.numerator().convert_to<int64_t>() / value.denominator().convert_to<int64_t>(); }
        operator std::uint8_t() const { return value.numerator().convert_to<std::uint8_t>() / value.denominator().convert_to<std::uint8_t>(); }
        operator uint16_t() const { return value.numerator().convert_to<uint16_t>() / value.denominator().convert_to<uint16_t>(); }
        operator uint32_t() const { return value.numerator().convert_to<uint32_t>() / value.denominator().convert_to<uint32_t>(); }
        operator uint64_t() const { return value.numerator().convert_to<uint64_t>() / value.denominator().convert_to<uint64_t>(); }

        bool operator<(const DecimalValue& other) const { return value < other.value; }
        bool operator>(const DecimalValue& other) const { return value > other.value; }
        bool operator<=(const DecimalValue& other) const { return value <= other.value; }
        bool operator>=(const DecimalValue& other) const { return value >= other.value; }
        bool operator==(const DecimalValue& other) const { return value == other.value; }
        bool operator!=(const DecimalValue& other) const { return value != other.value; }

        [[nodiscard]] std::string toString() const { return value.numerator().str() + '/' + value.denominator().str(); }
    };

    export struct Value : std::variant<BooleanValue, ListValue, ObjectValue, StringValue,
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
} // namespace TheCalculater::settings
