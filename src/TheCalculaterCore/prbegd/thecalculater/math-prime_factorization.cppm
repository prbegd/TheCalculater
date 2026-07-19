/**
 * Copyright © 2026 Cai Yaoxing
 *
 * This file is part of TheCalculater.
 * TheCalculater is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * TheCalculater is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with TheCalculater. If not, see <https://www.gnu.org/licenses/>.
 */
export module prbegd.thecalculater.math:prime_factorization;
import :concepts;
import std;

namespace thecalculater::math {
export template <integer_calculable T>
std::vector<T> primeFactors(T n)
{
    std::vector<T> factors;

    for (T p : { 2, 3 }) {
        while (n % p == 0) {
            factors.push_back(p);
            n /= p;
        }
    }

    T i = 5;
    while (i * i <= n) {
        while (n % i == 0) {
            factors.push_back(i);
            n /= i;
        }

        T next_i = i + 2;
        while (n % next_i == 0) {
            factors.push_back(next_i);
            n /= next_i;
        }

        i += 6;
    }

    if (n > 1) {
        factors.push_back(n);
    }

    return factors;
}
export template <integer_calculable T>
std::map<T, T> primeFactorization(T n)
{
    std::map<T, T> factors;

    const auto divideOut = [&](T p) {
        while (n % p == 0) {
            factors[p]++;
            n /= p;
        }
    };

    divideOut(2);
    divideOut(3);

    T i = 5;
    while (i * i <= n) {
        divideOut(i);
        divideOut(i + 2);
        i += 6;
    }

    if (n > 1) {
        factors[n]++;
    }

    return factors;
}
} // namespace thecalculater::math