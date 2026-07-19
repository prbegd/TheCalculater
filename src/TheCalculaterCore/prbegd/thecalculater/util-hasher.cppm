/**
 * Copyright © 2026 Cai Yaoxing
 *
 * This file is part of TheCalculater.
 * TheCalculater is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * TheCalculater is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with TheCalculater. If not, see <https://www.gnu.org/licenses/>.
 *
 */
module;

export module prbegd.thecalculater.util:hasher;
import std;

namespace thecalculater::util {
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
    std::size_t operator()(const U& x) const noexcept
    {
        return std::hash<U> { }(x);
    }
};
/// Specialization for std::string_view to handle different types of strings.
template <>
struct TransparentHash<std::string_view> {
    using is_transparent = void;

    std::size_t operator()(std::string_view x) const noexcept
    {
        return std::hash<std::string_view> { }(x);
    }
    std::size_t operator()(const std::string& x) const noexcept
    {
        return std::hash<std::string_view> { }(x);
    }
    std::size_t operator()(const char* x) const noexcept
    {
        return std::hash<std::string_view> { }(x);
    }
};
} // namespace thecalculater::util