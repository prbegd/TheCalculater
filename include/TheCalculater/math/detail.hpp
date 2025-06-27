#pragma once
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/rational.hpp>

namespace TheCalculater::math {
    using _fraction = boost::rational<boost::multiprecision::cpp_int>;
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