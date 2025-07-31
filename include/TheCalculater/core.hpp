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

    THECALC_API void registerLogger(const std::shared_ptr<spdlog::logger>& logger);
} // namespace TheCalculater::core