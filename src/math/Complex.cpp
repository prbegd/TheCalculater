#include "TheCalculater/math/Complex.hpp"
#include <algorithm>

namespace TheCalculater::math {
    _fraction _fractionConvertor::parseDecimal(std::string_view str)
    {
        using boost::multiprecision::cpp_int;
        size_t pos = str.find('.');
        if (pos == std::string_view::npos) {
            return { cpp_int(std::string(str)), cpp_int(1) };
        }
        bool negative = str.starts_with('-');
        std::string numerator = std::string(
                                    str.substr(static_cast<size_t>(negative) /* negative ? 1 : 0 */, pos))
            + std::string(str.substr(pos + 1));
        cpp_int denominator = 1;
        for (size_t i = 0, n = str.size() - pos - 1; i < n; ++i)
            denominator *= 10;
        if (negative)
            numerator = '-' + numerator;
        return { cpp_int(numerator), denominator };
    }
    _fraction _fractionConvertor::parseFloat(double value)
    {
        std::ostringstream oss;
        oss << std::setprecision(getFloatPrecision()) << value;
        return parseDecimal(oss.str());
    }
    _fraction _fractionConvertor::parseRational(std::string_view str)
    {
        using boost::multiprecision::cpp_int;
        size_t pos = str.find('/');
        if (pos == std::string_view::npos)
            return parseDecimal(str);
        return { cpp_int(std::string(str.substr(0, pos))),
            cpp_int(std::string(str.substr(pos + 1))) };
    }
    void _fractionConvertor::parseString(std::string str, Complex& complex)
    {

    }
} // namespace TheCalculater::math