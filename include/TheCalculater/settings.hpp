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

#include <QString>
#include <utility>

namespace TheeCalculater::settings {
    /**
     * @brief A class representing a string value in settings.
     */
    class StringValue {
    private:
        const std::string value;

    public:
        StringValue(std::string val)
            : value(std::move(val))
        { }

        [[nodiscard]] std::string string() const { return value; }
        [[nodiscard]] QString qString() const { return QString::fromStdString(value); }

        operator std::string() const { return value; }
        operator QString() const { return QString::fromStdString(value); }
    };
} // namespace TheeCalculater::settings