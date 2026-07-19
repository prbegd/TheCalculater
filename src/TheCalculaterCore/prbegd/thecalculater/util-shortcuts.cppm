/**
 * Copyright © 2026 Cai Yaoxing
 *
 * This file is part of TheCalculater.
 * TheCalculater is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * TheCalculater is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with TheCalculater. If not, see <https://www.gnu.org/licenses/>.
 */
module;

export module prbegd.thecalculater.util:shortcuts;
import std;

namespace thecalculater::util {
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

template <typename... Ts>
struct overload : Ts... {
    using Ts::operator()...;
}; // NOLINT
template <typename... Ts>
overload(Ts...) -> overload<Ts...>;

export template <typename... TType, std::invocable<TType>... TVisitor>
void match(const std::variant<TType...>& v, TVisitor&&... visitor)
{
    std::visit(overload { std::forward<TVisitor>(visitor)... }, v);
}
} // namespace thecalculater::util
