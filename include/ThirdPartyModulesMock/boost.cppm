/**
 * @file boost.cppm
 * @author prbegd
 * @date 2026-04-12
 *
 * Copyright © 2025 Cai Yaoxing
 * SPDX-License-Identifier: GPL-3.0-only
 * This file is part of TheCalculater.
 * See the file LICENSE in the project root or go to
 * <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
 */
module;
#include <boost/algorithm/string/split.hpp>
#include <boost/container_hash/hash.hpp>
#include <boost/core/demangle.hpp>
#include <boost/exception/all.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/rational.hpp>
#include <boost/regex.hpp>
#include <boost/stacktrace.hpp>
#include <boost/stacktrace/detail/location_from_symbol.hpp>

export module tpmm.boost;
import std;

// algorithm namespace
export namespace boost {
    namespace algorithm {
        using boost::algorithm::split;
    }

    // rational related
    using boost::bad_rational;
    using boost::rational;

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
    using boost::get_error_info;

    // hash
    using boost::hash_combine;
    using boost::hash_value;

    // multiprecision namespace
    namespace multiprecision {
        using boost::multiprecision::cpp_int_check_type;
        using boost::multiprecision::cpp_integer_type;

        namespace backends {
            using boost::multiprecision::backends::eval_left_shift;
            using boost::multiprecision::backends::eval_right_shift;
            using boost::multiprecision::backends::divide_unsigned_helper;
            using boost::multiprecision::backends::hash_value;
            using boost::multiprecision::backends::eval_get_sign;
            using boost::multiprecision::backends::eval_add;
            using boost::multiprecision::backends::eval_subtract;
            using boost::multiprecision::backends::eval_multiply;
            using boost::multiprecision::backends::eval_divide;
            using boost::multiprecision::backends::eval_modulus;
            using boost::multiprecision::backends::eval_bitwise_and;
            using boost::multiprecision::backends::eval_bitwise_or;
            using boost::multiprecision::backends::eval_bitwise_xor;
        }
        namespace details {
            using boost::multiprecision::detail::hash_value;
        }
        using boost::multiprecision::hash_value;

        using boost::multiprecision::cpp_int;

        using boost::multiprecision::abs;
        using boost::multiprecision::pow;
        using boost::multiprecision::msb;

        using boost::multiprecision::operator==;
        using boost::multiprecision::operator!=;
        using boost::multiprecision::operator<;
        using boost::multiprecision::operator>;
        using boost::multiprecision::operator<=;
        using boost::multiprecision::operator>=;
        using boost::multiprecision::operator+;
        using boost::multiprecision::operator-;
        using boost::multiprecision::operator*;
        using boost::multiprecision::operator/;
        using boost::multiprecision::operator%;
        using boost::multiprecision::operator<<;
        using boost::multiprecision::operator>>;
        using boost::multiprecision::operator&;
        using boost::multiprecision::operator|;
        using boost::multiprecision::operator^;
        using boost::multiprecision::operator~;
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