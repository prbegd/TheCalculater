#pragma once
#include <stacktrace>
#include <string>

namespace App::Debug {
    std::string formatStack(const std::stacktrace& stack);
}

#define CURRENT_STACKTRACE() App::Debug::formatStack(std::stacktrace::current())
