/**
 * @file Complex.cpp
 * @author prbegd
 * @brief Provides a Complex class for TheCalculater.
 * @date 2025-06-23
 *
 * Copyright © 2025 prbegd & TheCalculater contributors
 * Licensed under the MIT License. See LICENSE in the project root for license information.
 *
 */
#include "TheCalculater/math/Complex.hpp"

namespace TheCalculater::math::_fractionConvertor {

    static bool processNegative(std::string& str)
    {
        if (str.empty())
            return false;

        bool negative = (str[0] == '-');
        if (str[0] == '+' || str[0] == '-') {
            str.erase(0, 1);
        }
        return negative;
    }

    _fraction parseDecimal(std::string str)
    {
        using boost::multiprecision::cpp_int;

        if (str.empty())
            return { cpp_int(0), cpp_int(1) };

        bool negative = processNegative(str);

        size_t pos = str.find('.');
        if (pos == std::string::npos) {
            cpp_int num(str);
            if (negative)
                num = -num;
            return { num, cpp_int(1) };
        }

        if (str == ".")
            return { cpp_int(0), cpp_int(1) };
        if (pos == 0)
            str.insert(0, "0");
        if (pos == str.size() - 1)
            str.push_back('0');

        std::string integer = str.substr(0, pos);
        std::string fractional = str.substr(pos + 1);

        cpp_int num = cpp_int(integer) * cpp_int("1" + std::string(fractional.size(), '0'))
            + cpp_int(fractional.empty() ? "0" : fractional);

        if (negative)
            num = -num;
        cpp_int denom = cpp_int("1" + std::string(fractional.size(), '0'));

        return { num, denom };
    }
    _fraction parseRational(std::string str)
    {
        using boost::multiprecision::cpp_int;

        if (str.empty())
            return { cpp_int(0), cpp_int(1) };

        bool negative = processNegative(str);

        size_t pos = str.find('/');
        if (pos == std::string::npos)
            return parseDecimal(str);

        if (pos == 0 || pos == str.size() - 1)
            throw std::invalid_argument("Invalid rational format: " + str);

        std::string numStr = str.substr(0, pos);
        std::string denomStr = str.substr(pos + 1);

        cpp_int numerator = numStr.empty() ? cpp_int(0) : cpp_int(numStr);
        cpp_int denominator = denomStr.empty() ? cpp_int(1) : cpp_int(denomStr);

        if (denominator == 0)
            throw std::invalid_argument("Denominator cannot be zero: " + str);

        if (negative)
            numerator = -numerator;

        return { numerator, denominator };
    }
    _fraction parseFloat(double value)
    {
        std::ostringstream oss;
        oss << std::setprecision(_getFloatPrecision()) << value;
        return parseDecimal(oss.str());
    }
} // namespace TheCalculater::math