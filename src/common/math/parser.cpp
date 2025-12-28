/**
 * @file parser.cpp
 * @author prbegd
 * @date 2025-12-27
 *
 * Copyright © 2025 Cai Yaoxing
 * SPDX-License-Identifier: GPL-3.0-only
 * This file is part of TheCalculater.
 * See the file LICENSE in the project root or go to
 * <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
 */
#include "TheCalculater/math/parser.hpp"
#include <boost/regex.hpp>

namespace TheCalculater::math {
    namespace { namespace _d_parseFraction {
        Fraction decimalToFraction(const boost::smatch& match)
        {
            using boost::multiprecision::cpp_int;
            // match[3] is matched means that there is a repeating decimal part
            if (match[3].matched) {
                cpp_int nonRepeating(match[2].str());
                cpp_int repeating(match[3].str());
                const auto& nonRepeatingLength = match[2].length();
                const auto& repeatingLength = match[3].length();

                std::string integerStr = match[1].str();
                cpp_int integerPart(integerStr);
                Fraction decimalPart(pow(cpp_int(10), repeatingLength) * nonRepeating + repeating - nonRepeating,
                    (pow(cpp_int(10), repeatingLength) - 1) * pow(cpp_int(10), nonRepeatingLength));

                Fraction result = abs(integerPart) + decimalPart;
                return integerStr[0] == '-' ? -result : result;
            } else {
                return { cpp_int(match[1].str() + match[2].str()),
                    cpp_int(pow(cpp_int(10), match[2].length())) };
            }
        }
    }} // namespace ::_d_parseFraction
    Fraction parseFraction(const std::string& str)
    {
        static const boost::regex plainFractionRegex(R"(([+-]?\d+)/(\d+))");
        if (boost::smatch match; boost::regex_match(str, match, plainFractionRegex)) {
            return {
                boost::multiprecision::cpp_int(match[1].str()),
                boost::multiprecision::cpp_int(match[2].str())
            };
        }
        static const boost::regex plainDecimalRegex(R"(([+-]?\d+)(?:\.(\d*)(?:\{(\d+)\})?)?)");
        if (boost::smatch match; boost::regex_match(str, match, plainDecimalRegex)) {
            return _d_parseFraction::decimalToFraction(match);
        }
        static const boost::regex latexFractionRegex(R"(([+-]?)\\frac\{([+-]?\d+)\}\{([+-]?\d+)\})");
        if (boost::smatch match; boost::regex_match(str, match, latexFractionRegex)) {
            Fraction result = {
                boost::multiprecision::cpp_int(match[2].str()),
                boost::multiprecision::cpp_int(match[3].str())
            };
            return match[1] == "-" ? -result : result;
        }
        static const boost::regex latexDecimalRegex(R"(([+-]?\d+)(?:\.(\d*)(?:\\overline\{(\d+)\})?)?)");
        if (boost::smatch match; boost::regex_match(str, match, latexDecimalRegex)) {
            return _d_parseFraction::decimalToFraction(match);
        }
        throwEx(std::invalid_argument("Invalid fraction format: " + str));
        return {};
    }
} // namespace TheCalculater::math