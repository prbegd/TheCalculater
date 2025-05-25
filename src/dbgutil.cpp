#include "TheCalculater/dbgutil.hpp"
#include "spdlog/spdlog.h"
#include <chrono>
#include <csignal>
#include <memory>


namespace TheCalculater::dbgutil {
    int currentSignal = 0; 


    static std::string currentISO8601TimeUTC()
    {
        auto now = std::chrono::system_clock::now();
        auto itt = std::chrono::system_clock::to_time_t(now);
    
        std::ostringstream ss;
        ss << std::put_time(gmtime(&itt), "%FT%TZ");
        return ss.str();
    }
    void customTerminateHandler()
    {
        // TODO: 把terminate信息更细节
        auto time = std::chrono::system_clock::now();
        SPDLOG_CRITICAL("Program Terminated!");
        spdlog::default_logger()->set_pattern("%v");

        spdlog::info("---------- Program Terminate Repert ----------\n");
        spdlog::info("Time: {}", currentISO8601TimeUTC());
        

        spdlog::shutdown();
        std::abort();
    }

    std::string formatStacktrace(const boost::stacktrace::stacktrace& stk)
    {
        std::ostringstream oss;
        for (size_t i = 0; i < stk.size(); i++) {
            if (stk[i].empty())
                continue;
            oss << "  #" << i << " " << stk[i].name();
            if (stk[i].source_line() != 0) {
                oss << " at " << stk[i].source_file() << ":" << stk[i].source_line();
            }
            oss << " (" << stk[i].address() << ")\n";
        }
        return oss.str();
    }

    void init()
    {
        std::set_terminate(customTerminateHandler);

        // std::signal(SIG)
    }
} // namespace TheCalculater::dbgutil