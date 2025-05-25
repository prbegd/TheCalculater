#pragma once
#include <boost/stacktrace/stacktrace.hpp>

namespace TheCalculater::dbgutil {
    void customTerminateHandler();
    std::string formatStacktrace(const boost::stacktrace::stacktrace& stk);
    void init();
}