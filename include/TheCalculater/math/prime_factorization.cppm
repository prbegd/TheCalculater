/**
 * @file prime_factorization.cppm
 * @author prbegd
 * @date 2025-12-20
 *
 * Copyright © 2025 Cai Yaoxing
 * SPDX-License-Identifier: GPL-3.0-only
 * This file is part of TheCalculater.
 * See the file LICENSE in the project root or go to
 * <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
 */
module;
#include "TheCalculater/macros.hpp"

export module TheCalculater.math.prime_factorization;
import TheCalculater.math.concepts;
import std;

namespace TheCalculater::math {
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
} // namespace TheCalculater::math