/**
 * Copyright © 2026 Cai Yaoxing
 *
 * This file is part of TheCalculater.
 * TheCalculater is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * TheCalculater is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with TheCalculater. If not, see <https://www.gnu.org/licenses/>.
 */
module prbegd.thecalculater.math;
import prbegd.thecalculater.settings;
import prbegd.thecalculater.util;
import thirdparty.core;
import std;

namespace thecalculater::math {
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
                throwext(std::runtime_error("Unable to create global default Fraction configuration (thecalculater::math::FractionCalculationConfig) object. Is the settings initialized yet?"));
            }
        });
        return instance;
    }
} // namespace thecalculater::math