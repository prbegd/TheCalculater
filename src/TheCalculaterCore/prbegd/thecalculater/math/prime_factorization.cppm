/**
 * @file prime_factorization.cppm
 * @author prbegd
 * @date 2025-12-20
 *
 * Copyright © 2026 Cai Yaoxing
 *
 * This file is part of TheCalculater.
 * TheCalculater is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * TheCalculater is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with TheCalculater. If not, see <https://www.gnu.org/licenses/>.
 */
module;
#include "thecalculater/macros.hpp"

export module prbegd.thecalculater.math.prime_factorization;
import prbegd.thecalculater.math.concepts;
import std;

namespace thecalculater::math {
    export THECALCULATER_DEFINE_EXCEPTION(PrimeFactorizationException, std::domain_error);
    /**
     * @brief Prime factorization of a integer.
     *
     * @tparam T The type of the integer to be factored.
     * @param n The integer to be factored.
     * @return std::vector<T> A vector containing the prime factors of n.
     */
    export template <IsIntegerLike T>
    std::vector<T> primeFactorize(T n)
    {
        std::vector<T> factors;

        // We use 6k±1 pattern to check for prime factors.

        // Check for 2 and 3.
        for (T p : { 2, 3 }) {
            while (n % p == 0) {
                factors.push_back(p);
                n /= p;
            }
        }

        // Check for other primes using 6k±1 pattern.
        // We don't need to check if i is composite because we already made sure that i is not divisible by any smaller prime.
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

        // If n is still greater than 1, it must be a prime number.
        if (n > 1) {
            factors.push_back(n);
        }

        return factors;
    }
} // namespace thecalculater::math