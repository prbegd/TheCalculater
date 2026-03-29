/**
 * @file exceptions.cppm
 * @author prbegd
 * @date 2026-03-15
 * 
 * Copyright © 2025 Cai Yaoxing
 * SPDX-License-Identifier: GPL-3.0-only
 * This file is part of TheCalculater.
 * See the file LICENSE in the project root or go to 
 * <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
 */
module;
#include "TheCalculater/macros.hpp"
#include <stdexcept>

export module TheCalculater.settings.exceptions;

namespace TheCalculater::settings {
    export THECALCULATER_DEFINE_EXCEPTION(BadSettingsException, std::logic_error);
    export THECALCULATER_DEFINE_EXCEPTION(SettingsKeyNotFoundException, BadSettingsException);
    export THECALCULATER_DEFINE_EXCEPTION(BadJsonSettingsValueException, BadSettingsException);
    export THECALCULATER_DEFINE_EXCEPTION(InvalidConfigTemplateException, BadSettingsException);
}