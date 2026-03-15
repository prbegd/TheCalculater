/**
 * @file logger_wrapper.cpp
 * @author prbegd
 * @date 2025-12-14
 * 
 * Copyright © 2025 Cai Yaoxing
 * SPDX-License-Identifier: GPL-3.0-only
 * This file is part of TheCalculater.
 * See the file LICENSE in the project root or go to 
 * <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
 */
#pragma once

#include <format>
namespace TheCalculater::core {
    class LoggerWrapper {
    public:
        using LogFunction = std::function<void(std::string_view)>;

        LoggerWrapper() = default;
        explicit LoggerWrapper(LogFunction logger)
            : logger_(std::move(logger))
        { }

        template <typename... Args>
        void operator()(std::format_string<Args...> format, Args&&... args) const
        {
            if (logger_)
                logger_(std::format(format, std::forward<Args>(args)...));
        }
        
    private:
        LogFunction logger_;
    };
}