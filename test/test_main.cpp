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

#include "spdlog/spdlog.h"

#include "TheCalculater/math/analytic_expression.hpp"
#include "TheCalculater/math/fraction.hpp"
#include "TheCalculater/settings.hpp"
#include "TheCalculater/util/json.hpp"
#include <QResource>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/rational.hpp>
#include <json/value.h>

using namespace TheCalculater::math;

int main(int argc, char* argv[])
{
    if (!QResource::registerResource("./resources.rcc")) {
        SPDLOG_CRITICAL("Failed to load resource file");
        return 1;
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
