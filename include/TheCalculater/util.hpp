/**
 * @file util.hpp
 * @author prbegd
 * @brief Utility functions and types.
 * @date 2025-06-21
 *
 * Copyright © 2025 prbegd & TheCalculater contributors
 * Licensed under the MIT License. See LICENSE in the project root for license information.
 *
 */
#pragma once
#include <string>
#include <string_view>
#include <type_traits>

namespace TheCalculater::util {
    template <typename T>
    struct is_string : std::disjunction<
        std::is_same<char*, typename std::decay_t<T>>,
        std::is_same<const char*, typename std::decay_t<T>>,
        std::is_same<std::string, typename std::decay_t<T>>,
        std::is_same<std::string_view, typename std::decay_t<T>>
    > {};
}