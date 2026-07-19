/**
 * Copyright © 2026 Cai Yaoxing
 *
 * This file is part of TheCalculater.
 * TheCalculater is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * TheCalculater is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with TheCalculater. If not, see <https://www.gnu.org/licenses/>.
 */
export module prbegd.thecalculater.math:concepts;
import std;

namespace thecalculater::math {
export template <typename T>
concept calculable = requires(T a, T b) {
    { a + b } -> std::convertible_to<T>;
    { a - b } -> std::convertible_to<T>;
    { a * b } -> std::convertible_to<T>;
    { a / b } -> std::convertible_to<T>;
    { -a } -> std::convertible_to<T>;
    { a += b } -> std::same_as<T&>;
    { a -= b } -> std::same_as<T&>;
    { a *= b } -> std::same_as<T&>;
    { a /= b } -> std::same_as<T&>;
    { a = 0 } -> std::same_as<T&>;
} && std::totally_ordered<T> && std::constructible_from<T, int>;
export template <typename T>
concept integer_calculable = requires(T a, T b) {
    { a % b } -> std::convertible_to<T>;
    { a %= b } -> std::same_as<T&>;
} && calculable<T>;
} // namespace thecalculater::math