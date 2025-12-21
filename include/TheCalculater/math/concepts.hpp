/**
 * @file concepts.hpp
 * @author prbegd
 * @date 2025-12-21
 * 
 * Copyright © 2025 Cai Yaoxing
 * SPDX-License-Identifier: GPL-3.0-only
 * This file is part of TheCalculater.
 * See the file LICENSE in the project root or go to 
 * <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
 */
#pragma once

#include <concepts>
#include <type_traits>
namespace TheCalculater::math {
    /**
     * @brief Check if the type T behaves like an integer type.
     * 
     * @tparam T The type to check.
     */
    template <typename T>
    concept IsIntegerLike = requires(T a, T b) {
        { a + b } -> std::same_as<T>;
        { a - b } -> std::same_as<T>;
        { a * b } -> std::same_as<T>;
        { a / b } -> std::same_as<T>;
        { a % b } -> std::same_as<T>;
        { -a } -> std::same_as<T>;

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
        
        { a++ } -> std::same_as<T&>;
        { a-- } -> std::same_as<T&>;
        { ++a } -> std::same_as<T&>;
        { --a } -> std::same_as<T&>;

        { a & b } -> std::same_as<T>;
        { a | b } -> std::same_as<T>;
        { a ^ b } -> std::same_as<T>;
        { ~a } -> std::same_as<T>;
        { a << b } -> std::same_as<T>;
        { a >> b } -> std::same_as<T>;

        { a &= b } -> std::same_as<T&>;
        { a |= b } -> std::same_as<T&>;
        { a ^= b } -> std::same_as<T&>;
        { a <<= b } -> std::same_as<T&>;
        { a >>= b } -> std::same_as<T&>;

        requires std::constructible_from<T, int>;
        { a = 0 } -> std::same_as<T&>;

        requires std::convertible_to<T, bool>;
    } && !std::is_pointer_v<T>;
}