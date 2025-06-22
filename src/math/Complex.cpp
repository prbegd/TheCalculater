#include "TheCalculater/math/Complex.hpp"

namespace TheCalculater::math {
    _fraction _fractionConvertor::parseDecimal(std::string_view str)
    {
        using boost::multiprecision::cpp_int;
        size_t pos = str.find('.');
        if (pos == std::string_view::npos) {
            return {cpp_int(std::string(str)), cpp_int(1)};
        }
        std::string numerator = std::string(str.substr(0, pos)) + std::string(str.substr(pos + 1));
        cpp_int denominator = 1;
        for (size_t i = 0, n = str.size() - pos - 1; i < n; ++i) {
            denominator *= 10;
        }
        return {cpp_int(numerator), denominator};
    }
}