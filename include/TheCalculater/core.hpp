/**
 * @file core.hpp
 * @author prbegd
 * @brief Core header file for TheCalculater.
 * @date 2025-07-28
 *
 * Copyright © 2025 prbegd & TheCalculater contributors
 * Licensed under the MIT License. See LICENSE in the project root for license information.
 *
 */
#pragma once
#include "spdlog/logger.h"
#include <atomic>
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

    THECALC_API void registerLogger(const std::shared_ptr<spdlog::logger>& logger);
} // namespace TheCalculater::core