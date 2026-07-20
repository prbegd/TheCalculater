/**
 * Copyright © 2026 Cai Yaoxing
 *
 * This file is part of TheCalculater.
 * TheCalculater is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * TheCalculater is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with TheCalculater. If not, see <https://www.gnu.org/licenses/>.
 */
module;
#define BOOST_STACKTRACE_USE_BACKTRACE
#include <boost/algorithm/string.hpp>
#include <boost/container_hash/hash.hpp>
#include <boost/core/demangle.hpp>
#include <boost/exception/all.hpp>
#include <boost/multiprecision/gmp.hpp>
#include <boost/rational.hpp>
#include <boost/regex.hpp>
#include <boost/stacktrace.hpp>
#include <boost/stacktrace/detail/location_from_symbol.hpp>

export module thirdparty.core:boost;

// algorithm namespace
export namespace boost {
namespace algorithm {
    using boost::algorithm::split;
    using boost::algorithm::trim;
}

// rational related
using boost::bad_rational;

// regex related
using boost::cmatch;
using boost::regex;
using boost::regex_error;
using boost::regex_match;
using boost::regex_search;

// core namespace
namespace core {
    using boost::core::demangle;
}

// exception related
using boost::enable_error_info;
using boost::error_info;
using boost::exception;
using boost::get_error_info;

// hash
using boost::hash_combine;
using boost::hash_value;

// multiprecision namespace
// kinda desperate
namespace multiprecision {
    using boost::multiprecision::cpp_int_check_type;
    using boost::multiprecision::cpp_integer_type;

    namespace backends {
        using boost::multiprecision::backends::eval_abs;
        using boost::multiprecision::backends::eval_add;
        using boost::multiprecision::backends::eval_bit_flip;
        using boost::multiprecision::backends::eval_bit_set;
        using boost::multiprecision::backends::eval_bit_test;
        using boost::multiprecision::backends::eval_bit_unset;
        using boost::multiprecision::backends::eval_bitwise_and;
        using boost::multiprecision::backends::eval_bitwise_or;
        using boost::multiprecision::backends::eval_bitwise_xor;
        using boost::multiprecision::backends::eval_ceil;
        using boost::multiprecision::backends::eval_complement;
        using boost::multiprecision::backends::eval_convert_to;
        using boost::multiprecision::backends::eval_divide;
        using boost::multiprecision::backends::eval_eq;
        using boost::multiprecision::backends::eval_fabs;
        using boost::multiprecision::backends::eval_floor;
        using boost::multiprecision::backends::eval_fpclassify;
        using boost::multiprecision::backends::eval_frexp;
        using boost::multiprecision::backends::eval_gcd;
        using boost::multiprecision::backends::eval_get_sign;
        using boost::multiprecision::backends::eval_gt;
        using boost::multiprecision::backends::eval_ilogb;
        using boost::multiprecision::backends::eval_integer_modulus;
        using boost::multiprecision::backends::eval_integer_sqrt;
        using boost::multiprecision::backends::eval_is_zero;
        using boost::multiprecision::backends::eval_lcm;
        using boost::multiprecision::backends::eval_ldexp;
        using boost::multiprecision::backends::eval_left_shift;
        using boost::multiprecision::backends::eval_lsb;
        using boost::multiprecision::backends::eval_lt;
        using boost::multiprecision::backends::eval_modulus;
        using boost::multiprecision::backends::eval_msb;
        using boost::multiprecision::backends::eval_multiply_add;
        using boost::multiprecision::backends::eval_multiply;
        using boost::multiprecision::backends::eval_powm;
        using boost::multiprecision::backends::eval_qr;
        using boost::multiprecision::backends::eval_real;
        using boost::multiprecision::backends::eval_right_shift;
        using boost::multiprecision::backends::eval_scalbn;
        using boost::multiprecision::backends::eval_set_imag;
        using boost::multiprecision::backends::eval_set_real;
        using boost::multiprecision::backends::eval_signbit;
        using boost::multiprecision::backends::eval_sqrt;
        using boost::multiprecision::backends::eval_subtract;
        using boost::multiprecision::backends::eval_trunc;
        using boost::multiprecision::backends::hash_value;
        using boost::multiprecision::backends::gmp_int;
        using boost::multiprecision::backends::gmp_rational;
    } // namespace backends
    namespace details {
        using boost::multiprecision::detail::abs_funct;
        using boost::multiprecision::detail::abs;
        using boost::multiprecision::detail::expression;
        using boost::multiprecision::detail::function;
        using boost::multiprecision::detail::hash_value;
        using boost::multiprecision::detail::unsigned_abs;
    }
    namespace default_ops {
        using boost::multiprecision::default_ops::assign_components_imp;
        using boost::multiprecision::default_ops::assign_components_imp2;
        using boost::multiprecision::default_ops::assign_components;
        using boost::multiprecision::default_ops::assign_from_string_view;
        using boost::multiprecision::default_ops::check_in_range;
        using boost::multiprecision::default_ops::eval_abs;
        using boost::multiprecision::default_ops::eval_acos;
        using boost::multiprecision::default_ops::eval_acosh;
        using boost::multiprecision::default_ops::eval_add;
        using boost::multiprecision::default_ops::eval_asin;
        using boost::multiprecision::default_ops::eval_asinh;
        using boost::multiprecision::default_ops::eval_atan;
        using boost::multiprecision::default_ops::eval_atan2;
        using boost::multiprecision::default_ops::eval_atanh;
        using boost::multiprecision::default_ops::eval_bit_flip;
        using boost::multiprecision::default_ops::eval_bit_set;
        using boost::multiprecision::default_ops::eval_bit_test;
        using boost::multiprecision::default_ops::eval_bit_unset;
        using boost::multiprecision::default_ops::eval_bitwise_and_default;
        using boost::multiprecision::default_ops::eval_bitwise_and;
        using boost::multiprecision::default_ops::eval_bitwise_or_default;
        using boost::multiprecision::default_ops::eval_bitwise_or;
        using boost::multiprecision::default_ops::eval_bitwise_xor_default;
        using boost::multiprecision::default_ops::eval_bitwise_xor;
        using boost::multiprecision::default_ops::eval_ceil;
        using boost::multiprecision::default_ops::eval_complement;
        using boost::multiprecision::default_ops::eval_conj;
        using boost::multiprecision::default_ops::eval_convert_to;
        using boost::multiprecision::default_ops::eval_cos;
        using boost::multiprecision::default_ops::eval_decrement;
        using boost::multiprecision::default_ops::eval_divide_default;
        using boost::multiprecision::default_ops::eval_divide;
        using boost::multiprecision::default_ops::eval_fabs;
        using boost::multiprecision::default_ops::eval_fdim;
        using boost::multiprecision::default_ops::eval_floor;
        using boost::multiprecision::default_ops::eval_fmax;
        using boost::multiprecision::default_ops::eval_fmin;
        using boost::multiprecision::default_ops::eval_fmod;
        using boost::multiprecision::default_ops::eval_fpclassify;
        using boost::multiprecision::default_ops::eval_frexp;
        using boost::multiprecision::default_ops::eval_get_sign;
        using boost::multiprecision::default_ops::eval_hypot;
        using boost::multiprecision::default_ops::eval_ilogb;
        using boost::multiprecision::default_ops::eval_imag;
        using boost::multiprecision::default_ops::eval_increment;
        using boost::multiprecision::default_ops::eval_integer_sqrt;
        using boost::multiprecision::default_ops::eval_karatsuba_sqrt;
        using boost::multiprecision::default_ops::eval_ldexp;
        using boost::multiprecision::default_ops::eval_left_shift;
        using boost::multiprecision::default_ops::eval_logb;
        using boost::multiprecision::default_ops::eval_lsb;
        using boost::multiprecision::default_ops::eval_modf;
        using boost::multiprecision::default_ops::eval_modulus_default;
        using boost::multiprecision::default_ops::eval_modulus;
        using boost::multiprecision::default_ops::eval_msb;
        using boost::multiprecision::default_ops::eval_multiply_add;
        using boost::multiprecision::default_ops::eval_multiply_default;
        using boost::multiprecision::default_ops::eval_multiply_subtract;
        using boost::multiprecision::default_ops::eval_multiply;
        using boost::multiprecision::default_ops::eval_nearbyint;
        using boost::multiprecision::default_ops::eval_proj;
        using boost::multiprecision::default_ops::eval_qr;
        using boost::multiprecision::default_ops::eval_real;
        using boost::multiprecision::default_ops::eval_remainder;
        using boost::multiprecision::default_ops::eval_remquo;
        using boost::multiprecision::default_ops::eval_right_shift;
        using boost::multiprecision::default_ops::eval_rint;
        using boost::multiprecision::default_ops::eval_round;
        using boost::multiprecision::default_ops::eval_scalbln;
        using boost::multiprecision::default_ops::eval_scalbn;
        using boost::multiprecision::default_ops::eval_set_real;
        using boost::multiprecision::default_ops::eval_signbit;
        using boost::multiprecision::default_ops::eval_sqrt;
        using boost::multiprecision::default_ops::eval_subtract_default;
        using boost::multiprecision::default_ops::eval_subtract;
        using boost::multiprecision::default_ops::eval_trunc;
        using boost::multiprecision::default_ops::eval_set_imag;
        using boost::multiprecision::default_ops::is_arg_nan;
        using boost::multiprecision::default_ops::last_chance_eval_convert_to;
    } // namespace default_ops

    using boost::multiprecision::abs;
    using boost::multiprecision::acos;
    using boost::multiprecision::acosh;
    using boost::multiprecision::add;
    using boost::multiprecision::arg;
    using boost::multiprecision::asin;
    using boost::multiprecision::asinh;
    using boost::multiprecision::atan;
    using boost::multiprecision::atan2;
    using boost::multiprecision::atanh;
    using boost::multiprecision::cbrt;
    using boost::multiprecision::ceil;
    using boost::multiprecision::changesign;
    using boost::multiprecision::conj;
    using boost::multiprecision::copysign;
    using boost::multiprecision::cos;
    using boost::multiprecision::cosh;
    using boost::multiprecision::double_limb_type;
    using boost::multiprecision::erf;
    using boost::multiprecision::erfc;
    using boost::multiprecision::exp;
    using boost::multiprecision::exp2;
    using boost::multiprecision::expm1;
    using boost::multiprecision::fabs;
    using boost::multiprecision::fdim;
    using boost::multiprecision::floor;
    using boost::multiprecision::fma;
    using boost::multiprecision::fmax;
    using boost::multiprecision::fmin;
    using boost::multiprecision::fmod;
    using boost::multiprecision::fpclassify;
    using boost::multiprecision::frexp;
    using boost::multiprecision::gcd;
    using boost::multiprecision::gmp_int;
    using boost::multiprecision::gmp_rational;
    using boost::multiprecision::hash_value;
    using boost::multiprecision::hypot;
    using boost::multiprecision::ilogb;
    using boost::multiprecision::imag;
    using boost::multiprecision::iround;
    using boost::multiprecision::isfinite;
    using boost::multiprecision::isinf;
    using boost::multiprecision::isnan;
    using boost::multiprecision::isnormal;
    using boost::multiprecision::itrunc;
    using boost::multiprecision::lcm;
    using boost::multiprecision::ldexp;
    using boost::multiprecision::lgamma;
    using boost::multiprecision::llrint;
    using boost::multiprecision::llround;
    using boost::multiprecision::lltrunc;
    using boost::multiprecision::log;
    using boost::multiprecision::log10;
    using boost::multiprecision::log1p;
    using boost::multiprecision::log2;
    using boost::multiprecision::logb;
    using boost::multiprecision::lrint;
    using boost::multiprecision::lround;
    using boost::multiprecision::ltrunc;
    using boost::multiprecision::modf;
    using boost::multiprecision::mpq_rational;
    using boost::multiprecision::mpz_int;
    using boost::multiprecision::msb;
    using boost::multiprecision::multiply;
    using boost::multiprecision::nearbyint;
    using boost::multiprecision::nextafter;
    using boost::multiprecision::nexttoward;
    using boost::multiprecision::norm;
    using boost::multiprecision::number;
    using boost::multiprecision::polar;
    using boost::multiprecision::pow;
    using boost::multiprecision::proj;
    using boost::multiprecision::real;
    using boost::multiprecision::remainder;
    using boost::multiprecision::remquo;
    using boost::multiprecision::rint;
    using boost::multiprecision::round;
    using boost::multiprecision::scalbln; 
    using boost::multiprecision::scalbn;
    using boost::multiprecision::sign;
    using boost::multiprecision::signbit;
    using boost::multiprecision::sin;
    using boost::multiprecision::sinh;
    using boost::multiprecision::sqrt;
    using boost::multiprecision::subtract;
    using boost::multiprecision::tan;
    using boost::multiprecision::tanh;
    using boost::multiprecision::tgamma;
    using boost::multiprecision::trunc;

    using boost::multiprecision::operator-;
    using boost::multiprecision::operator!=;
    using boost::multiprecision::operator*;
    using boost::multiprecision::operator/;
    using boost::multiprecision::operator&;
    using boost::multiprecision::operator%;
    using boost::multiprecision::operator^;
    using boost::multiprecision::operator+;
    using boost::multiprecision::operator<;
    using boost::multiprecision::operator<<;
    using boost::multiprecision::operator<=;
    using boost::multiprecision::operator==;
    using boost::multiprecision::operator>;
    using boost::multiprecision::operator>=;
    using boost::multiprecision::operator>>;
    using boost::multiprecision::operator|;
    using boost::multiprecision::operator~;

    using boost::multiprecision::denominator;
    using boost::multiprecision::numerator;
} // namespace multiprecision

// stacktrace namespace
namespace stacktrace {
    using boost::stacktrace::stacktrace;
    namespace detail {
        using boost::stacktrace::detail::location_from_symbol;
    }
}

using boost::operator==;
using boost::operator!=;
using boost::operator<;
using boost::operator>;
using boost::operator<=;
using boost::operator>=;
using boost::operator+;
using boost::operator-;
using boost::operator*;
using boost::operator/;
using boost::operator<<;
using boost::operator>>;

using boost::abs;
} // namespace boost