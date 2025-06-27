#pragma once
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/rational.hpp>

namespace TheCalculater::math {
    using _fraction = boost::rational<boost::multiprecision::cpp_int>;

    namespace fraction_convertor {
        template <typename T>
        _fraction convert(T value)
        {
            if constexpr (std::is_integral_v<T> || std::is_same_v<T, boost::multiprecision::cpp_int>) {
                return _fraction(value);
            } else if constexpr (std::is_same_v<T, _fraction>) {
                return value;
            } else if constexpr (std::is_floating_point_v<T>) {
                return parseFloat(value);
            } else {
                static_assert(std::is_void_v<T>, "Unsupported type");
                return 0;
            }
        }
        _fraction parseFloat(double value);
        _fraction parseDecimal(std::string str);

        /**
         * @brief parse a string to rational.
         *
         * @param str string to parse
         * @throw std::invalid_argument if the string is not a valid rational number
         */
        _fraction parseRational(std::string str);
    }; // namespace fraction_convertor

    /**
     * @brief compute the square root of a fraction.
     *
     * @param fraction the fraction to compute the square root of
     * @param n precision of the result
     * @return _fraction the square root of the fraction
     * @throw std::invalid_argument if the fraction is negative
     *
     * Uses Babylonian Algorithm to compute.
     *
     * TODO: change '15' to settings::readInt("calc.float_precision", true) after settings is implemented
     */

    _fraction sqrt(const _fraction& fraction, int n = /* settings::readInt("calc.float_precision", true) */ 15);
    /// Uses Taylor Series to compute.
    _fraction sin(const _fraction& fraction, int iterations = /* settings::readInt("calc.taylor_iterations", true) */15);
    /// Uses Taylor Series to compute.
    _fraction cos(const _fraction& fraction, int iterations = /* settings::readInt("calc.taylor_iterations", true) */15);
    _fraction tan(const _fraction& fraction, int iterations = /* settings::readInt("calc.taylor_iterations", true) */15) { return sin(fraction, iterations) / cos(fraction, iterations); }

    _fraction pi();
}