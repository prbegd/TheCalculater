/**
 * @file core.hpp
 * @author prbegd
 * @brief Core header file for TheCalculater.
 * @date 2025-07-28
 *
 * Copyright © 2025 Cai Yaoxing
 * SPDX-License-Identifier: GPL-3.0-only
 * This file is part of TheCalculater.
 * See the file LICENSE in the project root or go to
 * <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
 *
 */
#pragma once
#include "spdlog/logger.h"
#include <algorithm>
#include <atomic>
#include <memory>
#include <stdexcept>

#ifdef _WIN32
#ifdef THECALCULATERCOMMON_EXPORTS
#define THECALC_API __declspec(dllexport)
#else
#define THECALC_API __declspec(dllimport)
#endif
#else
#define THECALC_API __attribute__((visibility("default")))
#endif

#define THECALCULATER_DEFINE_EXCEPTION(name, base) \
    class name : public base {                     \
    public:                                        \
        explicit name(const std::string& message)  \
            : base(message)                        \
        { }                                        \
    }

namespace TheCalculater::core {
    enum class ErrorHandleType {
        Ignore,
        ThrowException,
        LogError,
        LogWarn
    };

    THECALCULATER_DEFINE_EXCEPTION(IOException, std::runtime_error);
    THECALCULATER_DEFINE_EXCEPTION(FileNotFoundException, IOException);
    THECALCULATER_DEFINE_EXCEPTION(WeakPointerExpiredException, std::runtime_error);

    template <typename T>
    struct EqualTo {
        using is_transparent = void;
        bool operator()(const T& lhs, const T& rhs) const { return lhs == rhs; }
    };

    template <typename T>
    struct Hash {
        using is_transparent = void;
        size_t operator()(const T& key) const { return std::hash<T> {}(key); }
    };

    // clang 我操死你全家 这已经是我第二次因为clang编译器不支持的特性而改方案了
#ifdef __cpp_lib_atomic_shared_ptr
    template <typename T>
    class AtomicSharedPtr : public std::atomic<std::shared_ptr<T>> { };
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

    template <char excepted>
    bool boolCharPred(char c) { return c == excepted; }

    THECALC_API void registerLogger(const std::shared_ptr<spdlog::logger>& logger);

    template <typename T, template <typename> typename C>
    T value_or(const C<T>& v, const T& d) { return v ? *v : d; }
    template <typename T>
    T value_or(const std::weak_ptr<T>& v, const T& d) { return !v.expired() ? *v.lock() : d; }

} // namespace TheCalculater::core