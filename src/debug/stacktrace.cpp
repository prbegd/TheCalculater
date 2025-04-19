#include "debug/stacktrace.hpp"
#include <sstream>

namespace App::Debug {
    std::string formatStack(const std::stacktrace& stack)
    {
        std::ostringstream oss;
        for (size_t i = 0; i < stack.size(); ++i) {
            const auto& frame = stack[i];
            if (!frame)
                continue;
            oss << "  #" << i << " " << frame.description();
            if (frame.source_line() != 0)
                oss << " at " << frame.source_file() << ":" << frame.source_line();
            oss << "\n";
        }
        return oss.str();
    }
} // namespace App::Debug