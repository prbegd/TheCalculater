/**
 * @file shortcuts.cppm
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

export module TheCalculater.util.shortcuts;
import std;

namespace TheCalculater::util {
    /**
     * @brief Unary predicate to check if a value is equal to an expected value.
     *
     * @tparam T The type of the value to check.
     */
    export template <std::equality_comparable T>
    struct Expect {
        T expected;

        constexpr bool operator()(const T& other) const noexcept(noexcept(expected == other)) { return expected == other; }
    };

    export template <typename T, template <typename> typename C>
    T value_or(const C<T>& v, const T& d) noexcept(noexcept(*v)) { return v ? *v : d; }
    export template <typename T>
    T value_or(const std::weak_ptr<T>& v, const T& d) noexcept { return !v.expired() ? *v.lock() : d; }

    template <typename... Ts> struct overload : Ts... { using Ts::operator()...; }; // NOLINT
    template <typename... Ts> overload(Ts...) -> overload<Ts...>;

    export template <typename... TType, std::invocable<TType>... TVisitor>
    void match(const std::variant<TType...>& v, TVisitor&&... visitor)
    {
        std::visit(overload{ std::forward<TVisitor>(visitor)... }, v);
    }
} // namespace TheCalculater::util
