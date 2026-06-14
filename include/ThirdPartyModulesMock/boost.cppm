/**
 * @file boost.cppm
 * @author prbegd
 * @date 2026-04-12
 *
 * Copyright © 2026 Cai Yaoxing
 *
 * This file is part of TheCalculater.
 * TheCalculater is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * TheCalculater is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with TheCalculater. If not, see <https://www.gnu.org/licenses/>.
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
            using boost::multiprecision::backends::divide_unsigned_helper;
            using boost::multiprecision::backends::eval_add;
            using boost::multiprecision::backends::eval_bitwise_and;
            using boost::multiprecision::backends::eval_bitwise_or;
            using boost::multiprecision::backends::eval_bitwise_xor;
            using boost::multiprecision::backends::eval_divide;
            using boost::multiprecision::backends::eval_get_sign;
            using boost::multiprecision::backends::eval_left_shift;
            using boost::multiprecision::backends::eval_modulus;
            using boost::multiprecision::backends::eval_multiply;
            using boost::multiprecision::backends::eval_right_shift;
            using boost::multiprecision::backends::eval_subtract;
            using boost::multiprecision::backends::hash_value;
        } // namespace backends
        namespace details {
            using boost::multiprecision::detail::hash_value;
        }
        using boost::multiprecision::hash_value;

        using boost::multiprecision::cpp_int;

        using boost::multiprecision::abs;
        using boost::multiprecision::msb;
        using boost::multiprecision::pow;

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