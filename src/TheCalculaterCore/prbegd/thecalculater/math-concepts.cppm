/**
 * Copyright © 2026 Cai Yaoxing
 *
 * This file is part of TheCalculater.
 * TheCalculater is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * TheCalculater is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with TheCalculater. If not, see <https://www.gnu.org/licenses/>.
 */
module;

export module prbegd.thecalculater.math:concepts;
import std;

namespace thecalculater::math {
    /**
     * @brief Check if the type `T` behaves like an integer type.
     *
     * It does not check if the type `T` is an integer type.
     *
     * @tparam T The type to check.
     */
    export template <typename T>
    concept IsIntegerLike = requires(T a, T b) {
        { a + b } -> std::convertible_to<T>;
        { a - b } -> std::convertible_to<T>;
        { a * b } -> std::convertible_to<T>;
        { a / b } -> std::convertible_to<T>;
        { a % b } -> std::convertible_to<T>;
        { -a } -> std::convertible_to<T>;

        { a += b } -> std::same_as<T&>;
        { a -= b } -> std::same_as<T&>;
        { a *= b } -> std::same_as<T&>;
        { a /= b } -> std::same_as<T&>;
        { a %= b } -> std::same_as<T&>;

        { a == b } -> std::convertible_to<bool>;
        { a != b } -> std::convertible_to<bool>;
        { a < b } -> std::convertible_to<bool>;
        { a > b } -> std::convertible_to<bool>;
        { a <= b } -> std::convertible_to<bool>;
        { a >= b } -> std::convertible_to<bool>;

        requires std::constructible_from<T, int>;
        { a = 0 } -> std::same_as<T&>;
    } && !std::is_pointer_v<T>;
} // namespace thecalculater::math