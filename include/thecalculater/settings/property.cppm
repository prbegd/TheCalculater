/**
 * @file property.cppm
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
export module thecalculater.settings.property;
import thecalculater.settings.value;
import thecalculater.util;
import thirdparty.core;
import std;

namespace thecalculater::settings {
    export struct ItemProperty {
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

    export using PropertiesType = std::unordered_map<std::string, std::unique_ptr<ItemProperty>,
                                                     util::TransparentHash<std::string_view>, std::equal_to<>>;

    // Can only get property "name"
    export struct NamespaceItemProperty : ItemProperty {
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
    export struct NumberItemProperty : ItemProperty {
        std::optional<double> min;
        std::optional<double> max;

        NumberItemProperty(const std::optional<Value>& defaultValue,
                           const std::optional<std::string>& name,
                           const std::optional<std::string>& description,
                           const std::optional<std::string>& note,
                           const std::optional<std::string>& warning,
                           const std::optional<std::string>& deprecated,
                           std::optional<double> min = { },
                           std::optional<double> max = { })
            : ItemProperty(defaultValue, name, description, note, warning, deprecated), min(min), max(max)
        { }
        NumberItemProperty()
            : min({ }), max({ })
        { }
    };
    export struct IntegerItemProperty : NumberItemProperty {
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
    export struct DecimalItemProperty : NumberItemProperty {
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
    export struct StringItemProperty : ItemProperty {
        std::vector<StringValue> enums;
        std::optional<boost::regex> pattern;

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
                           std::optional<boost::regex> pattern = { },
                           const std::vector<StringValue>& enums0 = { })
            : ItemProperty(defaultValue, name, description, note, warning, deprecated), enums(enums0), pattern(std::move(pattern))
        { }

        [[nodiscard]] ValueType type() const noexcept override { return ValueType::String; }

        [[nodiscard]] std::unique_ptr<ItemProperty> clone() const override
        {
            return std::make_unique<StringItemProperty>(defaultValue, name, description, note, warning, deprecated, pattern, enums);
        }
    };
    export struct ListItemProperty : ItemProperty {
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
    export struct ObjectItemProperty : ItemProperty {
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
            auto copy = std::make_unique<ObjectItemProperty>(defaultValue, name, description, note, warning, deprecated, PropertiesType { });
            for (const auto& [key, value] : properties)
                copy->properties.emplace(key, value->clone());
            return copy;
        }
    };
    export struct EnumItemProperty : ItemProperty {
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
    export struct ButtonItemProperty : ItemProperty {
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

    export struct BooleanItemProperty : ItemProperty {
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

} // namespace thecalculater::settings
