/**
 * @file test_main.cpp
 * @author prbegd
 * @date 2025-12-21
 *
 * Copyright © 2025 Cai Yaoxing
 * SPDX-License-Identifier: GPL-3.0-only
 * This file is part of TheCalculater.
 * See the file LICENSE in the project root or go to
 * <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
 */
// TODO: Use catch2 v3 to replace this old-fashioned temporary solution.
#include <cstdint>
#define CATCH_CONFIG_RUNNER
#include "catch2/catch.hpp" // IWYU pragma: keep

#include "boost/stacktrace/detail/frame_decl.hpp"
#define BOOST_STACKTRACE_USE_BACKTRACE
#include "boost/core/demangle.hpp"
#include <backtrace.h>
#include <boost/stacktrace.hpp>
#include <iostream>

backtrace_state* state;
const backtrace_syminfo_callback callback = [](void* data, uintptr_t, const char* symname, uintptr_t, uintptr_t) {
    auto* result = static_cast<std::string*>(data);
    if (symname) {
        *result = boost::core::demangle(symname);
    }
};
const backtrace_error_callback error_callback = [](void*, const char* msg, int errnum) {
    std::cout << "ERROR: " << msg << " (Errno " << errnum << "')";
};

std::string name(const void* addr_ptr)
{
    auto addr = reinterpret_cast<uint64_t>(addr_ptr);

    std::string result;
    backtrace_syminfo(state, addr, callback, error_callback, &result);
    return result;
}

void foo()
{
    boost::stacktrace::stacktrace trace;

    for (unsigned i = 0; i < trace.size(); ++i) {
        std::cout << '#' << i << " " << name(trace[i].address()) << " (" << trace[i].address() << ")\n";
    }

    TheCalculater::settings::setSettingsFilePath("settings.json5");
    TheCalculater::settings::loadConfigTemplate(TheCalculater::util::parse(TheCalculater::util::readResourcesFile(":/resources/data/config_template.json5").constData()));
    std::unordered_map<std::string, std::string> errors;
    TheCalculater::settings::parseSettings(errors);
    if (!errors.empty()) {
        std::ostringstream oss;
        for (const auto& [key, value] : errors) {
            oss << "Key: '" << key << "' Error: '" << value << "'\n";
        }
        SPDLOG_ERROR("Errors parsing settings:\n{}", oss.str());
    }

    return Catch::Session().run(argc, argv);
}

int main(int, char* argv[])
{
    state = backtrace_create_state(argv[0], 0, nullptr, nullptr);
    if (!state) {
        std::cout << "backtrace_create_state failed\n";
        return 1;
    }

    foo();
    return 0;
}