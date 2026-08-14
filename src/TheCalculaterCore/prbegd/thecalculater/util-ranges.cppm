/**
 * Copyright © 2026 Cai Yaoxing
 *
 * This file is part of TheCalculater.
 * TheCalculater is free software: you can redistribute it and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version. TheCalculater is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details. You should have received a copy of the GNU General Public License along
 * with TheCalculater. If not, see <https://www.gnu.org/licenses/>.
 */
export module prbegd.thecalculater.util:ranges;
import std;

export namespace thecalculater::util {
template <template <typename...> typename TContainer,
          typename... TInnerContainerArgs,
          typename... TOuterContainerRemainingArgs>
std::generator<TContainer<TInnerContainerArgs...>> cartesianProduct(
    TContainer<TContainer<TInnerContainerArgs...>, TOuterContainerRemainingArgs...> dims)
{
    if (dims.empty()) {
        co_yield TContainer<TInnerContainerArgs...> { };
        co_return;
    }

    for (const auto& dim : dims) {
        if (dim.empty()) {
            co_return;
        }
    }

    const std::size_t n = dims.size();
    std::vector<std::size_t> indices(n, 0);

    while (true) {
        TContainer<TInnerContainerArgs...> combo;
        if constexpr (requires { combo.reserve(n); }) {
            combo.reserve(n);
        }
        for (std::size_t i = 0; i < n; ++i) {
            combo.push_back(std::move(dims[i][indices[i]]));
        }

        co_yield std::move(combo);

        std::size_t pos = n;
        while (pos > 0) {
            --pos;
            if (++indices[pos] < dims[pos].size()) {
                break;
            }
            indices[pos] = 0;
            if (pos == 0) {
                co_return;
            }
        }
    }
}

template <typename TSourceContainer, typename TDestinationContainer>
    requires requires(TSourceContainer s, TDestinationContainer t) {
        { s.extract(s.begin()) } -> std::same_as<typename TSourceContainer::node_type>;
        { t.insert(t.end(), std::declval<typename TSourceContainer::value_type>()) };
    }
constexpr void extractSetElement(TSourceContainer& source, TDestinationContainer& destination)
{
    if constexpr (requires { destination.reserve(destination.size() + source.size()); }) {
        destination.reserve(destination.size() + source.size());
    }
    while (!source.empty()) {
        auto node = source.extract(source.begin());
        destination.insert(destination.end(), std::move(node.value()));
    }
}
} // namespace thecalculater::util