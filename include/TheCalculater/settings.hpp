/**
 * @file settings.hpp
 * @author prbegd
 * @brief Provides settings for TheCalculater.
 * @date 2025-07-30
 *
 * Copyright © 2025 Cai Yaoxing
 * SPDX-License-Identifier: GPL-3.0-only
 * This file is part of TheCalculater.
 * See the file LICENSE in the project root or go to
 * <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
 *
 */
#pragma once
#include "TheCalculater/core.hpp"
#include "TheCalculater/math/detail.hpp"
#include <QString>
#include <cstdint>
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
    // * below is types
    struct Value;

    using BooleanValue = bool;
    using ListValue = std::vector<Value>;
    using ObjectValue = std::vector<std::pair<std::string, Value>>;

    THECALCULATER_DEFINE_EXCEPTION(BadSettingsException, std::logic_error);
    THECALCULATER_DEFINE_EXCEPTION(SettingsKeyNotFoundException, BadSettingsException);
    THECALCULATER_DEFINE_EXCEPTION(BadJsonSettingsValueException, BadSettingsException);
    THECALCULATER_DEFINE_EXCEPTION(InvalidConfigTemplateException, BadSettingsException);

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
        explicit StringValue(std::string_view val)
            : value(val)
        { }
        explicit StringValue(const QString& val)
            : value(val.toStdString())
        { }
        explicit StringValue(const char* val)
            : value(val)
        { }
        StringValue() = default;

        [[nodiscard]] std::string string() const { return value; }
        [[nodiscard]] QString qString() const { return QString::fromStdString(value); }

        operator std::string() const { return value; }
        operator QString() const { return QString::fromStdString(value); }

        auto operator<=>(const StringValue& other) const = default;
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

        bool operator<(const IntegerValue& other) const { return value < other.value; }
        bool operator>(const IntegerValue& other) const { return value > other.value; }
        bool operator<=(const IntegerValue& other) const { return value <= other.value; }
        bool operator>=(const IntegerValue& other) const { return value >= other.value; }
        bool operator==(const IntegerValue& other) const { return value == other.value; }
        bool operator!=(const IntegerValue& other) const { return value != other.value; }

        [[nodiscard]] std::string toString() const { return value.str(); }
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
        DecimalValue() = default;

        static DecimalValue fromString(std::string str)
        {
            return DecimalValue(TheCalculater::math::fraction_convertor::parseRational(std::move(str)));
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

        bool operator<(const DecimalValue& other) const { return value < other.value; }
        bool operator>(const DecimalValue& other) const { return value > other.value; }
        bool operator<=(const DecimalValue& other) const { return value <= other.value; }
        bool operator>=(const DecimalValue& other) const { return value >= other.value; }
        bool operator==(const DecimalValue& other) const { return value == other.value; }
        bool operator!=(const DecimalValue& other) const { return value != other.value; }

        [[nodiscard]] std::string toString() const { return value.numerator().str() + '/' + value.denominator().str(); }
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
        ItemProperty(const ItemProperty& other) = delete;
        ItemProperty& operator=(const ItemProperty& other) = delete;
        ItemProperty(ItemProperty&& other) = default;
        ItemProperty& operator=(ItemProperty&& other) = default;

        ItemProperty(const std::optional<Value>& defaultValue,
            const std::optional<std::string>& name,
            const std::optional<std::string>& description,
            const std::optional<std::string>& note,
            const std::optional<std::string>& warning,
            const std::optional<std::string>& deprecated)
            : defaultValue(defaultValue), name(name), description(description), note(note), warning(warning), deprecated(deprecated)
        { }
        ItemProperty()
            : defaultValue(std::nullopt), name(std::nullopt), description(std::nullopt), note(std::nullopt), warning(std::nullopt), deprecated(std::nullopt)
        { }

        // This field is valueless only if this object is obtained from ListItemProperty.childType or the type of this object is "button" or "namespace".
        std::optional<Value> defaultValue;
        std::optional<std::string> name; // translation key
        std::optional<std::string> description; // translation key, empty when type is "namespace"
        std::optional<std::string> note; // translation key, empty when type is "namespace"
        std::optional<std::string> warning; // translation key, empty when type is "namespace"
        std::optional<std::string> deprecated; // translation key, empty when type is "namespace"

        // fuck rtti.
        // The type of item. Not value type.
        [[nodiscard]] virtual ValueType type() const noexcept = 0;
        [[nodiscard]] virtual std::unique_ptr<ItemProperty> clone() const = 0;
    };

    using PropertiesType = std::unordered_map<std::string, std::unique_ptr<ItemProperty>,
        core::Hash<std::string_view>, core::EqualTo<std::string_view>>;

    // Can only get property "name"
    struct NamespaceItemProperty : ItemProperty {
        NamespaceItemProperty() = default;
        NamespaceItemProperty(const NamespaceItemProperty& other) = delete;
        NamespaceItemProperty& operator=(const NamespaceItemProperty& other) = delete;
        NamespaceItemProperty(NamespaceItemProperty&& other) = default;
        NamespaceItemProperty& operator=(NamespaceItemProperty&& other) = default;
        ~NamespaceItemProperty() override = default;
        using ItemProperty::ItemProperty;

        [[nodiscard]] ValueType type() const noexcept override { return ValueType::Namespace; }

        [[nodiscard]] std::unique_ptr<ItemProperty> clone() const override
        {
            return std::make_unique<NamespaceItemProperty>(std::nullopt, name, std::nullopt, std::nullopt, std::nullopt, std::nullopt);
        }
    };
    struct NumberItemProperty : ItemProperty {
        std::optional<double> min;
        std::optional<double> max;

        NumberItemProperty(const std::optional<Value>& defaultValue,
            const std::optional<std::string>& name,
            const std::optional<std::string>& description,
            const std::optional<std::string>& note,
            const std::optional<std::string>& warning,
            const std::optional<std::string>& deprecated,
            std::optional<double> min = {},
            std::optional<double> max = {})
            : ItemProperty(defaultValue, name, description, note, warning, deprecated), min(min), max(max)
        { }
        NumberItemProperty()
            : min({}), max({})
        { }
    };
    struct IntegerItemProperty : NumberItemProperty {
        IntegerItemProperty() = default;
        IntegerItemProperty(const IntegerItemProperty& other) = delete;
        IntegerItemProperty& operator=(const IntegerItemProperty& other) = delete;
        IntegerItemProperty(IntegerItemProperty&& other) = default;
        IntegerItemProperty& operator=(IntegerItemProperty&& other) = default;
        ~IntegerItemProperty() override = default;
        using NumberItemProperty::NumberItemProperty;

        [[nodiscard]] ValueType type() const noexcept override { return ValueType::Integer; }

        [[nodiscard]] std::unique_ptr<ItemProperty> clone() const override
        {
            return std::make_unique<IntegerItemProperty>(defaultValue, name, description, note, warning, deprecated, min, max);
        }
    };
    struct DecimalItemProperty : NumberItemProperty {
        DecimalItemProperty() = default;
        DecimalItemProperty(const DecimalItemProperty& other) = delete;
        DecimalItemProperty& operator=(const DecimalItemProperty& other) = delete;
        DecimalItemProperty(DecimalItemProperty&& other) = default;
        DecimalItemProperty& operator=(DecimalItemProperty&& other) = default;
        ~DecimalItemProperty() override = default;
        using NumberItemProperty::NumberItemProperty;

        [[nodiscard]] ValueType type() const noexcept override { return ValueType::Decimal; }

        [[nodiscard]] std::unique_ptr<ItemProperty> clone() const override
        {
            return std::make_unique<DecimalItemProperty>(defaultValue, name, description, note, warning, deprecated, min, max);
        }
    };
    struct StringItemProperty : ItemProperty {
        std::vector<StringValue> enums;
        std::optional<std::regex> pattern;

        StringItemProperty() = default;
        StringItemProperty(const StringItemProperty& other) = delete;
        StringItemProperty& operator=(const StringItemProperty& other) = delete;
        StringItemProperty(StringItemProperty&& other) = default;
        StringItemProperty& operator=(StringItemProperty&& other) = default;
        ~StringItemProperty() override = default;

        StringItemProperty(const std::optional<Value>& defaultValue,
            const std::optional<std::string>& name,
            const std::optional<std::string>& description,
            const std::optional<std::string>& note,
            const std::optional<std::string>& warning,
            const std::optional<std::string>& deprecated,
            std::optional<std::regex> pattern = {},
            const std::vector<StringValue>& enums0 = {})
            : ItemProperty(defaultValue, name, description, note, warning, deprecated), enums(enums0), pattern(std::move(pattern))
        { }

        [[nodiscard]] ValueType type() const noexcept override { return ValueType::String; }

        [[nodiscard]] std::unique_ptr<ItemProperty> clone() const override
        {
            return std::make_unique<StringItemProperty>(defaultValue, name, description, note, warning, deprecated, pattern, enums);
        }
    };
    struct ListItemProperty : ItemProperty {
        std::unique_ptr<ItemProperty> childType;

        ListItemProperty() = default;
        ListItemProperty(const ListItemProperty& other) = delete;
        ListItemProperty& operator=(const ListItemProperty& other) = delete;
        ListItemProperty(ListItemProperty&& other) = default;
        ListItemProperty& operator=(ListItemProperty&& other) = default;
        ~ListItemProperty() override = default;

        ListItemProperty(const std::optional<Value>& defaultValue,
            const std::optional<std::string>& name,
            const std::optional<std::string>& description,
            const std::optional<std::string>& note,
            const std::optional<std::string>& warning,
            const std::optional<std::string>& deprecated,
            std::unique_ptr<ItemProperty>&& childType)
            : ItemProperty(defaultValue, name, description, note, warning, deprecated), childType(std::move(childType))
        { }

        [[nodiscard]] ValueType type() const noexcept override { return ValueType::List; }
        [[nodiscard]] std::unique_ptr<ItemProperty> clone() const override
        {
            return std::make_unique<ListItemProperty>(defaultValue, name, description, note, warning, deprecated, childType->clone());
        }
    };
    struct ObjectItemProperty : ItemProperty {
        PropertiesType properties;

        ObjectItemProperty() = default;
        ObjectItemProperty(const ObjectItemProperty& other) = delete;
        ObjectItemProperty& operator=(const ObjectItemProperty& other) = delete;
        ObjectItemProperty(ObjectItemProperty&& other) = default;
        ObjectItemProperty& operator=(ObjectItemProperty&& other) = default;
        ~ObjectItemProperty() override = default;

        ObjectItemProperty(const std::optional<Value>& defaultValue,
            const std::optional<std::string>& name,
            const std::optional<std::string>& description,
            const std::optional<std::string>& note,
            const std::optional<std::string>& warning,
            const std::optional<std::string>& deprecated,
            PropertiesType&& properties)
            : ItemProperty(defaultValue, name, description, note, warning, deprecated), properties(std::move(properties))
        { }

        [[nodiscard]] ValueType type() const noexcept override { return ValueType::Object; }

        [[nodiscard]] std::unique_ptr<ItemProperty> clone() const override
        {
            auto copy = std::make_unique<ObjectItemProperty>(defaultValue, name, description, note, warning, deprecated, PropertiesType {});
            for (const auto& [key, value] : properties)
                copy->properties.emplace(key, value->clone());
            return copy;
        }
    };
    struct EnumItemProperty : ItemProperty {
        std::vector<StringValue> values;

        EnumItemProperty() = default;
        EnumItemProperty(const EnumItemProperty& other) = delete;
        EnumItemProperty& operator=(const EnumItemProperty& other) = delete;
        EnumItemProperty(EnumItemProperty&& other) = default;
        EnumItemProperty& operator=(EnumItemProperty&& other) = default;
        ~EnumItemProperty() override = default;

        EnumItemProperty(const std::optional<Value>& defaultValue,
            const std::optional<std::string>& name,
            const std::optional<std::string>& description,
            const std::optional<std::string>& note,
            const std::optional<std::string>& warning,
            const std::optional<std::string>& deprecated,
            const std::vector<StringValue>& values)
            : ItemProperty(defaultValue, name, description, note, warning, deprecated), values(values)
        {
            if (values.empty())
                throw std::invalid_argument("EnumItemProperty: values cannot be empty");
        }

        [[nodiscard]] ValueType type() const noexcept override { return ValueType::Enum; }
        [[nodiscard]] std::unique_ptr<ItemProperty> clone() const override
        {
            return std::make_unique<EnumItemProperty>(defaultValue, name, description, note, warning, deprecated, values);
        }
    };
    struct ButtonItemProperty : ItemProperty {
        std::string text; // translation key

        ButtonItemProperty() = default;
        ButtonItemProperty(const ButtonItemProperty& other) = delete;
        ButtonItemProperty& operator=(const ButtonItemProperty& other) = delete;
        ButtonItemProperty(ButtonItemProperty&& other) = default;
        ButtonItemProperty& operator=(ButtonItemProperty&& other) = default;
        ~ButtonItemProperty() override = default;
        ButtonItemProperty(const std::optional<Value>& defaultValue,
            const std::optional<std::string>& name,
            const std::optional<std::string>& description,
            const std::optional<std::string>& note,
            const std::optional<std::string>& warning,
            const std::optional<std::string>& deprecated,
            std::string text)
            : ItemProperty(defaultValue, name, description, note, warning, deprecated), text(std::move(text))
        { }

        [[nodiscard]] ValueType type() const noexcept override { return ValueType::Button; }
        [[nodiscard]] std::unique_ptr<ItemProperty> clone() const override
        {
            return std::make_unique<ButtonItemProperty>(defaultValue, name, description, note, warning, deprecated, text);
        }
    };

    struct BooleanItemProperty : ItemProperty {
        BooleanItemProperty() = default;
        BooleanItemProperty(const BooleanItemProperty& other) = delete;
        BooleanItemProperty& operator=(const BooleanItemProperty& other) = delete;
        BooleanItemProperty(BooleanItemProperty&& other) = default;
        BooleanItemProperty& operator=(BooleanItemProperty&& other) = default;
        ~BooleanItemProperty() override = default;
        using ItemProperty::ItemProperty;

        [[nodiscard]] ValueType type() const noexcept override { return ValueType::Boolean; }
        [[nodiscard]] std::unique_ptr<ItemProperty> clone() const override
        {
            return std::make_unique<BooleanItemProperty>(defaultValue, name, description, note, warning, deprecated);
        }
    };

    // * below is API functions

    /**
     * @brief Read a value from settings.
     *
     * @throw TheCalculater::settings::SettingsKeyNotFoundException If the key is not found.
     * @param key The key to read the value from.
     * @return Value The value read from settings.
     */
    THECALC_API Value read(std::string_view key);

    /**
     * @brief Read a boolean value from settings.
     *
     * @throw TheCalculater::settings::SettingsKeyNotFoundException If the key is not found.
     * @throw TheCalculater::settings::BadSettingsException If the value is not a boolean.
     * @param key The key to read the value from.
     * @return BooleanValue The boolean value read from settings.
     * @see read()
     */
    THECALC_API BooleanValue readBool(std::string_view key);
    /**
     * @brief Read a list value from settings.
     *
     * @throw TheCalculater::settings::SettingsKeyNotFoundException If the key is not found.
     * @throw TheCalculater::settings::BadSettingsException If the value is not a list.
     * @param key The key to read the value from.
     * @return ListValue The list value read from settings.
     * @see read()
     */
    THECALC_API ListValue readList(std::string_view key);
    /**
     * @brief Read an object value from settings.
     *
     * @throw TheCalculater::settings::SettingsKeyNotFoundException If the key is not found.
     * @throw TheCalculater::settings::BadSettingsException If the value is not an object.
     * @param key The key to read the value from.
     * @return ObjectValue The object value read from settings.
     * @see read()
     */
    THECALC_API ObjectValue readObject(std::string_view key);
    /**
     * @brief Read a string value from settings.
     *
     * @throw TheCalculater::settings::SettingsKeyNotFoundException If the key is not found.
     * @throw TheCalculater::settings::BadSettingsException If the value is not a string.
     * @param key The key to read the value from.
     * @return StringValue The string value read from settings.
     * @see read()
     */
    THECALC_API StringValue readString(std::string_view key);
    /**
     * @brief Read an integer value from settings.
     *
     * @throw TheCalculater::settings::SettingsKeyNotFoundException If the key is not found.
     * @throw TheCalculater::settings::BadSettingsException If the value is not a integer.
     * @param key The key to read the value from.
     * @return IntegerValue The integer value read from settings.
     * @see read()
     */
    THECALC_API IntegerValue readInteger(std::string_view key);
    /**
     * @brief Read a decimal value from settings.
     *
     * @throw TheCalculater::settings::SettingsKeyNotFoundException If the key is not found.
     * @throw TheCalculater::settings::BadSettingsException If the value is not a decimal.
     * @param key The key to read the value from.
     * @return DecimalValue The decimal value read from settings.
     * @see read()
     */
    THECALC_API DecimalValue readDecimal(std::string_view key);

    /**
     * @brief Write a value to settings.
     *
     * @throw TheCalculater::settings::SettingsKeyNotFoundException If the key is not found.
     * @throw TheCalculater::settings::BadSettingsException If value type is different from the actual type.
     * @param key The key to write the value to.
     * @param value The value to write.
     * @see saveModified()
     */
    THECALC_API void write(std::string_view key, const Value& value);

    /**
     * @brief Write a boolean value to settings.
     *
     * @throw TheCalculater::settings::SettingsKeyNotFoundException If the key is not found.
     * @throw TheCalculater::settings::BadSettingsException If the actual type is not a boolean.
     * @param key The key to write the value to.
     * @param value The value to write.
     * @see saveModified()
     * @see write()
     */
    THECALC_API void writeBool(std::string_view key, const BooleanValue& value);
    /**
     * @brief Write a list value to settings.
     *
     * @throw TheCalculater::settings::SettingsKeyNotFoundException If the key is not found.
     * @throw TheCalculater::settings::BadSettingsException If the actual type is not a list.
     * @param key The key to write the value to.
     * @param value The value to write.
     * @see saveModified()
     * @see write()
     */
    THECALC_API void writeList(std::string_view key, const ListValue& value);
    /**
     * @brief Write an object value to settings.
     *
     * @throw TheCalculater::settings::SettingsKeyNotFoundException If the key is not found.
     * @throw TheCalculater::settings::BadSettingsException If the actual type is not an object.
     * @param key The key to write the value to.
     * @param value The value to write.
     * @see saveModified()
     * @see write()
     */
    THECALC_API void writeObject(std::string_view key, const ObjectValue& value);
    /**
     * @brief Write a string value to settings.
     *
     * @throw TheCalculater::settings::SettingsKeyNotFoundException If the key is not found.
     * @throw TheCalculater::settings::BadSettingsException If the actual type is not a string.
     * @param key The key to write the value to.
     * @param value The value to write.
     * @see saveModified()
     * @see write()
     */
    THECALC_API void writeString(std::string_view key, const StringValue& value);
    /**
     * @brief Write an integer value to settings.
     *
     * @throw TheCalculater::settings::SettingsKeyNotFoundException If the key is not found.
     * @throw TheCalculater::settings::BadSettingsException If the actual type is not an integer.
     * @param key The key to write the value to.
     * @param value The value to write.
     * @see saveModified()
     * @see write()
     */
    THECALC_API void writeInteger(std::string_view key, const IntegerValue& value);
    /**
     * @brief Write a decimal value to settings.
     *
     * @throw TheCalculater::settings::SettingsKeyNotFoundException If the key is not found.
     * @throw TheCalculater::settings::BadSettingsException If the actual type is not a decimal.
     * @param key The key to write the value to.
     * @param value The value to write.
     * @see saveModified()
     * @see write()
     */
    THECALC_API void writeDecimal(std::string_view key, const DecimalValue& value);

    /**
     * @brief Get the property of a key in settings.
     *
     * @param key The key to read the property from.
     * @return ItemProperty& The property of the key.
     * @throw TheCalculater::settings::SettingsKeyNotFoundException If the key doesn't exists.
     */
    THECALC_API ItemProperty& property(std::string_view key);
    /**
     * @brief Get the type of a value in settings.
     *
     * @param key The key to read the type from.
     * @return ValueType The type of the value.
     * @see property()
     * @throw TheCalculater::settings::SettingsKeyNotFoundException If the key doesn't exists.
     */
    inline ValueType typeOf(std::string_view key) { return property(key).type(); }

    /**
     * @brief Get the default value of a key.
     *
     * @param key The key to read the default value from.
     * @return Value The default value of the key.
     * @see property()
     * @throw TheCalculater::settings::SettingsKeyNotFoundException If the key doesn't exists.
     * @throw std::bad_optional_access If the key doesn't have a default value.
     */
    inline Value defaultValue(std::string_view key) { return *property(key).defaultValue; }

    /**
     * @brief Get all the keys that have been modified (using write()) but not saved (to file) yet.
     *
     * @return std::vector<std::string> A list of all modified keys.
     */
    THECALC_API const std::vector<std::string>& modifiedKeys();
    /**
     * @brief Set settings file path.
     *
     * This function will specify where saveModified() and parseSettings() will save to by default.
     *
     * @param path The path to the settings file.
     */
    THECALC_API void setSettingsFilePath(std::string_view path);
    /**
     * @brief Get settings file path.
     *
     * @return std::weak_ptr<std::string> The path to the settings file. If no path is set, it will return an empty weak pointer.
     */
    THECALC_API std::weak_ptr<std::string> getSettingsFilePath();

    /**
     * @brief Save all modified settings to file.
     *
     * If one of the modified key is a number (integer or decimal), we use string form to save it.
     * This function will not overwrite original file settings.
     * This function will re-serialize the file, which will remove all comments.
     *
     * @warning If you don't call this function, your changes will be lost when the program exits.
     * @see modifiedKeys()
     * @throw TheCalculater::core::IOException If the file cannot be opened.
     */
    THECALC_API void saveModified(const std::string& fileName = core::value_or(getSettingsFilePath(), {}));
    /**
     * @brief Parse settings from json.
     *
     * This function must be called after loadConfigTemplate() is called. (Otherwise, the parsing will all fail!)
     *
     * @param json The JSON value to parse.
     * @param error A list of errors that occurred during parsing.
     */
    THECALC_API void parseSettings(const Json::Value& json, std::unordered_map<std::string, std::string>& errors);
    /// overload for default settings file path
    /// If the file doesn't exist, it will create a new file
    /// witt a pair of empty braces.
    /// @throw TheCalculater::core::IOException If the file cannot be opened.
    /// @throw std::runtime_error If the settings file path pointer is expired.
    THECALC_API void parseSettings(std::unordered_map<std::string, std::string>& errors);

    /**
     * @brief Parse a value in JSON to a Value.
     *
     * This function will check if the value is valid for the property.
     *
     * @param result The result value.
     * @param property The property of the value to parse.
     * @param item The JSON value to parse.
     * @throw TheCalculater::settings::BadJsonSettingsValueException If the value is invalid.
     */
    THECALC_API void parseValue(Value& result, const ItemProperty& property, const Json::Value& item);
    /**
     * @brief Format a Value to JSON.
     *
     * @param result The result JSON value.
     * @param item The value to format.
     * @return Whether the operation succeeded.
     */
    THECALC_API bool formatValue(Json::Value& result, const Value& item);

    /**
     * @brief Load a config template from stream.
     *
     * This is usually called before parseSettings().
     *
     * TODO: add unloadConfigTemplate() function.
     *
     * @see parseSettings()
     * @param value The config template to load.
     */
    THECALC_API void loadConfigTemplate(const Json::Value& value);

    /**
     * @brief Register a button clicked event listener.
     *
     * TODO: Consider to implement this function is SettingsWidget.
     *
     * @param listener The listener to register. Parameter std::string_view is the button path.
     *
     * ! Work In Progress
     */
    THECALC_API void registerButtonClickedEventListener(const std::function<void(std::string_view)>& listener);
    /**
     * @brief Register an item changed event listener.
     *
     * @param listener The listener to register. Parameter std::string_view is the item path (key), and Value is the new value.
     */
    THECALC_API void registerItemChangedEventListener(const std::function<void(std::string_view, const Value&)>& listener);

} // namespace TheCalculater::settings