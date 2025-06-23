#include "TheCalculater/math/Complex.hpp"

namespace TheCalculater::math {

    static bool processNegative(std::string& str)
    {
        bool negative = false;
        if (str.starts_with('-')) {
            str.erase(0, 1);
            negative = true;
        } else if (str.starts_with('+'))
            str.erase(0, 1);
        return negative;
    }

    _fraction _fractionConvertor::parseDecimal(std::string str)
    {
        using boost::multiprecision::cpp_int;

        bool negative = processNegative(str);

        size_t pos = str.find('.');
        if (pos == std::string_view::npos)
            return { cpp_int(negative ? "-" + str : str), cpp_int(1) };
        std::string numerator = str.substr(0, pos) + str.substr(pos + 1);
        cpp_int denominator = 1;
        for (size_t i = 0, n = str.size() - pos - 1; i < n; ++i)
            denominator *= 10;

        return { cpp_int(negative ? "-" + numerator : numerator), denominator };
    }
    _fraction _fractionConvertor::parseFloat(double value)
    {
        std::ostringstream oss;
        oss << std::setprecision(getFloatPrecision()) << value;
        return parseDecimal(oss.str());
    }
    _fraction _fractionConvertor::parseRational(std::string str)
    {
        using boost::multiprecision::cpp_int;
        bool negative = processNegative(str);

        size_t pos = str.find('/');
        if (pos == std::string_view::npos)
            return parseDecimal(str);
        std::string numerator = str.substr(0, pos);
        std::string denominator = str.substr(pos + 1);
        return { cpp_int(negative ? "-" + numerator : numerator),
            cpp_int(denominator) };
    }
    void _fractionConvertor::parseString(std::string str, Complex& complex)
    {
    }
} // namespace TheCalculater::math