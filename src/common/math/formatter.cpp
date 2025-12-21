/**
 * @file formatter.cpp
 * @author prbegd
 * @date 2025-12-20
 *
 * Copyright © 2025 Cai Yaoxing
 * SPDX-License-Identifier: GPL-3.0-only
 * This file is part of TheCalculater.
 * See the file LICENSE in the project root or go to
 * <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
 */
#include "TheCalculater/math/formatter.hpp"
#include "TheCalculater/math/analytic_expression.hpp"
#include "TheCalculater/math/prime_factorization.hpp"
#include "boost/multiprecision/fwd.hpp"
#include <algorithm>
#include <cstddef>
#include <sstream>
#include <unordered_map>

namespace TheCalculater::math {
    namespace { namespace _d_format::fraction {
        bool isRepeatedDecimal(const Fraction& frac)
        {
            // Check if the denominator contains any prime factor other than 2 and 5.
            auto denoPrimeFactors = primeFactorization(frac.denominator());
            return std::any_of(denoPrimeFactors.begin(), denoPrimeFactors.end(), [](const auto& n) {
                return n != 2 && n != 5;
            });
        }
        std::string fractionWhenRepeatedDecimal(const Fraction& frac)
        {
            boost::multiprecision::cpp_int numerator = frac.numerator();
            const boost::multiprecision::cpp_int& denominator = frac.denominator();
            if (denominator == 1)
                return numerator.str();
            if (isRepeatedDecimal(frac))
                return numerator.str() += '/' + denominator.str();
            std::ostringstream result;

            if (numerator < 0) {
                result << '-';
                numerator = -numerator;
            }
            boost::multiprecision::cpp_int integerPart = numerator / denominator;
            boost::multiprecision::cpp_int remainder = numerator % denominator;
            result << integerPart.str() << '.';

            while (remainder != 0) {
                boost::multiprecision::cpp_int quotient = remainder * 10 / denominator;
                result << quotient.str();

                remainder = (remainder * 10) % denominator;
            }

            return result.str();
        }

        std::tuple<std::function<void()>, std::function<void()>> getRepeatingMarker(std::ostringstream& output, FormatType type)
        {
            switch (type) {
            case FormatType::LaTeX:
                return { [&output] { output << "\\overline{"; }, [&output] { output << "}"; } };
            case FormatType::PlainText:
                return { [&output] { output << "{"; }, [&output] { output << "}"; } };
            }
        }

        std::string alwaysDecimal(const Fraction& frac, FormatType type)
        {
            boost::multiprecision::cpp_int numerator = frac.numerator();
            const boost::multiprecision::cpp_int& denominator = frac.denominator();
            if (denominator == 1)
                return numerator.str();
            std::ostringstream result;

            const auto [markRepeatingBegin, markRepeatingEnd] = getRepeatingMarker(result, type);

            if (numerator < 0) {
                result << '-';
                numerator = -numerator;
            }
            boost::multiprecision::cpp_int integerPart = numerator / denominator;
            boost::multiprecision::cpp_int remainder = numerator % denominator;
            result << integerPart.str() << '.';

            std::unordered_map<boost::multiprecision::cpp_int, size_t> remainderPositions;
            std::string decimalPart;
            size_t position = 0;

            while (remainder != 0) {
                auto it = remainderPositions.find(remainder);
                if (it != remainderPositions.end()) {
                    size_t cycle_start = it->second;

                    std::string non_repeating = decimalPart.substr(0, cycle_start);
                    std::string repeating = decimalPart.substr(cycle_start);

                    result << non_repeating;
                    markRepeatingBegin();
                    result << repeating;
                    markRepeatingEnd();

                    break;
                }

                remainderPositions[remainder] = position++;

                remainder *= 10;
                boost::multiprecision::cpp_int quotient = remainder / denominator;
                remainder = remainder % denominator;

                decimalPart += quotient.str();
            }

            return result.str();
        }
    }} // namespace ::_d_format::fraction
    std::string format(const Fraction& frac, const FractionFormatOptions& options)
    {
        switch (options.style) {
        case FractionFormatOptions::Style::AlwaysFraction:
            return frac.numerator().str() += '/' + frac.denominator().str();
        case FractionFormatOptions::Style::FractionWhenDecimal: {
            if (frac.denominator() == 1)
                return frac.numerator().str();
            return frac.numerator().str() += '/' + frac.denominator().str();
        }
        case FractionFormatOptions::Style::FractionWhenRepeatedDecimal:
            return _d_format::fraction::fractionWhenRepeatedDecimal(frac);
        case FractionFormatOptions::Style::AlwaysDecimal:
            return _d_format::fraction::alwaysDecimal(frac, options.type);
        }
    }

    namespace { namespace _d_format::analytic_expression {

    }}
    std::string format(const AnalyticExpression& expr)
    {
    }
} // namespace TheCalculater::math