#pragma once
#include <stacktrace>

namespace App {
namespace Debug {
    std::string formatStack(std::stacktrace &st);
}
}