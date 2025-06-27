#include "TheCalculater/math/detail.hpp"

namespace TheCalculater::math {
    namespace fraction_convertor {
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
            // TODO: change '15' to settings::readInt("calc.float_precision", true) after settings is implemented
            // for future myself: true means cache
            oss << std::setprecision(/* settings::readInt("calc.precision", true) */ 15) << value;
            return parseDecimal(oss.str());
        }
    } // namespace fraction_convertor

    _fraction pi()
    {
        static const _fraction value = fraction_convertor::parseDecimal(
            /* settings::readStr("calc.pi", true) */
            "3.14159265358979323846");
        return value;
    }

    _fraction sqrt(const _fraction& fra, int n)
    {
        using namespace boost::multiprecision;
        static const int max_iterations = /* settings::readInt("calc.max_iterations") */ 1000;
        if (fra < 0)
            throw std::invalid_argument("Cannot compute square root of a negative number.");

        if (fra == 0)
            return 0;

        _fraction threshold(1, pow(cpp_int(10), n));

        _fraction x_prev = fra;
        _fraction x_next;
        int iterations = 0;

        while (true) {
            x_next = (x_prev + fra / x_prev) / 2;

            _fraction diff = abs(x_next - x_prev);
            if (diff < threshold || iterations >= max_iterations) {
                break;
            }

            x_prev = x_next;
            ++iterations;
        }

        return x_next;
    }
    _fraction sin(const _fraction& fra, int iterations)
    {
        _fraction term = fra;
        _fraction result = term;
        _fraction x_sq = fra * fra;
        int sign = -1;

        for (int n = 1; n < iterations; ++n) {
            term = term * x_sq;
            term = term / ((2 * n) * (2 * n + 1));
            term = term * sign;

            result += term;
            sign *= -1;
        }
        return result;
    }
    _fraction cos(const _fraction& fra, int iterations)
    {
        _fraction term(1);
        _fraction result = term;
        _fraction x_sq = fra * fra;
        int sign = -1;

        for (int n = 1; n < iterations; ++n) {
            term = term * x_sq;
            term = term / ((2 * n - 1) * (2 * n));
            term = term * sign;

            result += term;
            sign *= -1;
        }
        return result;
    }
    _fraction arcsin(const _fraction& fra, int iterations)
    {
        if (fra < -1 || fra > 1) {
            throw std::domain_error("arcsin(x) is undefined for |x| > 1");
        }

        _fraction term = fra;
        _fraction result = term;
        _fraction x_sq = fra * fra;

        _fraction coeff(1);

        for (int n = 1; n < iterations; ++n) {
            coeff = coeff * _fraction((2 * n) - 1, 2 * n);

            term = term * x_sq;
            _fraction next = coeff * term / ((2 * n) + 1);

            result += next;
        }

        return result;
    }
    _fraction arctan(const _fraction& fra, int iterations)
    {
        if (fra > _fraction(1)) {
            return pi() / 2 - arctan(_fraction(1) / fra, iterations);
        }
        if (fra < _fraction(-1)) {
            return -pi() / 2 - arctan(_fraction(1) / fra, iterations);
        }

        _fraction term = fra;
        _fraction result = term;
        _fraction x_sq = fra * fra;
        int sign = -1;

        for (int n = 1; n < iterations; ++n) {
            term = term * x_sq;
            term = term * _fraction((2 * n) - 1, (2 * n) + 1);
            term = term * sign;

            result += term;
            sign *= -1;
        }

        return result;
    }
} // namespace TheCalculater::math