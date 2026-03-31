/**
 * @file math_fraction_calculation_config_impl.cpp
 * @author prbegd
 * @date 2026-03-28
 *
 * Copyright © 2025 Cai Yaoxing
 * SPDX-License-Identifier: GPL-3.0-only
 * This file is part of TheCalculater.
 * See the file LICENSE in the project root or go to
 * <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
 */
module;
#include <boost/multiprecision/cpp_int.hpp>

module TheCalculater.math.fraction;
import TheCalculater.settings;
import TheCalculater.throwEx;
import std.compat;

namespace TheCalculater::math {
    const FractionCalculationConfig& FractionCalculationConfig::globalDefault()
    {
        static FractionCalculationConfig instance;
        static std::once_flag instanceInitializedFlag;
        std::call_once(instanceInitializedFlag, [] {
            try {
                FractionCalculationConfig config;
                config.pi = settings::readDecimal("calculating.pi");
                config.e = settings::readDecimal("calculating.e");
                config.approximation.useWhenNeeded = true;
                config.approximation.maxIterations = settings::readInteger("calculating.approximate_max_iterations");
                config.approximation.tolerance = Fraction { 1, pow(boost::multiprecision::cpp_int(10), settings::readInteger("calculating.approximate_tolerance")) };

                instance = config;
            } catch (const std::exception& e) {
                throwEx(std::runtime_error("Unable to create global default Fraction configuration (TheCalculater::math::FractionCalculationConfig) object. Is the settings initialized yet?"), { });
            }
        });
        return instance;
    }
} // namespace TheCalculater::math