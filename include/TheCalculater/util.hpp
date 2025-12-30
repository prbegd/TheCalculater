/**
 * @file util.hpp
 * @author prbegd
 * @brief Core utility functions and types.
 * @date 2025-06-21
 *
 * Copyright © 2025 Cai Yaoxing
 * SPDX-License-Identifier: GPL-3.0-only
 * This file is part of TheCalculater.
 * See the file LICENSE in the project root or go to
 * <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
 *
 */
#pragma once
#include "boost/stacktrace/detail/location_from_symbol.hpp"
#include "boost/stacktrace/stacktrace.hpp"
#include "core.hpp"
#include <boost/exception/all.hpp>
#include <boost/stacktrace.hpp>
#include <exception>
#include <stdexcept>
#include <string_view>
#include <type_traits>
#include <utility>

class QByteArray;
namespace Json {
    class Value;
}

namespace TheCalculater {
    namespace util {

        

        /**
         * @brief Read data from a resources file.
         *
         * Use std::string_view to read content (instead of toStdString()) avoid unnecessary copying.
         *
         * @param fileName
         * @return THECALC_API
         */
        THECALC_API QByteArray readResourcesFile(const std::string_view& fileName);

        /**
         * @brief Format a stacktrace into a string.
         *
         * @tparam SkipFirstFrame Whether to skip the first frame in the stacktrace.
         * @param stk The stacktrace to format.
         * @return std::string A string representation of the stacktrace.
         */
        template <bool SkipFirstFrame = true>
        std::string formatStacktrace(const boost::stacktrace::stacktrace& stk = boost::stacktrace::stacktrace {})
        {
            std::ostringstream oss;
            for (size_t i = SkipFirstFrame ? 1 : 0; i < stk.size(); i++) {
                if (stk[i].empty())
                    continue;
                oss << "  #" << i << ' ' << stk[i].name();
                if (stk[i].source_line() != 0) {
                    oss << " at " << stk[i].source_file() << ':' << stk[i].source_line();
                } else {
                    boost::stacktrace::detail::location_from_symbol loc(stk[i].address());
                    if (!loc.empty())
                        oss << " in " << loc.name();
                }
                oss << " (" << stk[i].address() << ')';
                if (i < stk.size() - 1)
                    oss << '\n';
            }
            return oss.str();
        }
        /**
         * @brief Format an exception into a string.
         *
         * @param e The exception to format.
         * @return std::string A string representation of the exception.
         */
        THECALC_API std::string formatException(const std::exception& e);

        /**
         * @brief Unary predicate to check if a value is equal to an expected value.
         *
         * @tparam T The type of the value to check.
         */
        template <std::equality_comparable T>
        struct Expect {
            T expected;

            constexpr bool operator()(const T& other) const noexcept(noexcept(expected == other)) { return expected == other; }
        };

        template <typename T, template <typename> typename C>
        T value_or(const C<T>& v, const T& d) { return v ? *v : d; }
        template <typename T>
        T value_or(const std::weak_ptr<T>& v, const T& d) { return !v.expired() ? *v.lock() : d; }

        // clang 我操死你全家 这已经是我第二次因为clang编译器不支持的特性而改方案了
#ifdef __cpp_lib_atomic_shared_ptr
        template <typename T>
        using AtomicSharedPtr = std::atomic<std::shared_ptr<T>>;
#else
        template <typename T>
        class AtomicSharedPtr {
        private:
            std::shared_ptr<T> ptr;

        public:
            AtomicSharedPtr() = default;
            explicit AtomicSharedPtr(std::shared_ptr<T> p)
                : ptr(p)
            { }

            void store(const std::shared_ptr<T>& p, std::memory_order order = std::memory_order_seq_cst)
            {
                std::atomic_store_explicit(&ptr, p, order);
            }

            std::shared_ptr<T> load(std::memory_order order = std::memory_order_seq_cst) const
            {
                return std::atomic_load_explicit(&ptr, order);
            }
        };
#endif

        class StringViewStreamBuf : public std::streambuf {
        public:
            explicit StringViewStreamBuf(std::string_view sv)
            {
                setg(const_cast<char*>(sv.data()),
                    const_cast<char*>(sv.data()),
                    const_cast<char*>(sv.data()) + sv.size());
            }
        };
        // NOLINTNEXTLINE(fuchsia-multiple-inheritance)
        class IStringViewStream : public std::istream {
        public:
            explicit IStringViewStream(std::string_view sv)
                : std::istream(&buf_), buf_(sv)
            {
                exceptions(std::istream::badbit);
            }

        private:
            StringViewStreamBuf buf_;
        };

        template <size_t N>
        struct ConstexprString {
            constexpr ConstexprString(const char (&s)[N])
            {
                std::copy_n(s, N, v);
            }
            char v[N] {};
        };
    } // namespace util
    
} // namespace TheCalculater
