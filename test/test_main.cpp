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
#define CATCH_CONFIG_RUNNER
#include "catch2/catch.hpp" // IWYU pragma: keep

int main(int argc, char* argv[])
{
    return Catch::Session().run( argc, argv );
}