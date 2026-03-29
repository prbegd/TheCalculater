/**
 * @file hasher.cppm
 * @author prbegd
 * @date 2025-11-07
 *
 * Copyright © 2025 Cai Yaoxing
 * SPDX-License-Identifier: GPL-3.0-only
 * This file is part of TheCalculater.
 * See the file LICENSE in the project root or go to
 * <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
 *
 */
module;
#include <functional>
#include <string>
#include <string_view>

export module TheCalculater.util.hasher;

namespace TheCalculater::util {
    /**
     * @brief Base TransparentHash struct for hashing.
     *
     * @tparam T The type to hash.
     */
    export template <typename T>
    struct TransparentHash {
        static_assert(false, "Cannot use TransparentHash in this way. Maybe you can try TransparentHash<>");
    };
    // I don't think this is good.
    template <>
    struct TransparentHash<void> {
        using is_transparent = void;

        template <typename U>
        size_t operator()(const U& x) const noexcept
        {
            return std::hash<U> {}(x);
        }
    };
    /// Specialization for std::string_view to handle different types of strings.
    template <>
    struct TransparentHash<std::string_view> {
        using is_transparent = void;

        size_t operator()(std::string_view x) const noexcept
        {
            return std::hash<std::string_view> {}(x);
        }
        size_t operator()(const std::string& x) const noexcept
        {
            return std::hash<std::string_view> {}(x);
        }
        size_t operator()(const char* x) const noexcept
        {
            return std::hash<std::string_view> {}(x);
        }
    };
} // namespace TheCalculater::core::hasher