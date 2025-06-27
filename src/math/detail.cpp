#include "TheCalculater/math/detail.hpp"

namespace TheCalculater::math {
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
            term = term / _fraction((2 * n - 1) * (2 * n));
            term = term * _fraction(sign);

            result += term;
            sign *= -1;
        }
        return result;
    }
} // namespace TheCalculater::math