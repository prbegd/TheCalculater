/**
 * Copyright © 2026 Cai Yaoxing
 *
 * This file is part of TheCalculater.
 * TheCalculater is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * TheCalculater is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with TheCalculater. If not, see <https://www.gnu.org/licenses/>.
 */
// Refer to https://github.com/CLIUtils/CLI11/blob/main/src/modules/CLI11.cppm
// CLI11 2.6.2 Copyright (c) 2017-2026 University of Cincinnati, developed by Henry
// Schreiner under NSF AWARD 1414736. All rights reserved.
module;
#include <CLI/CLI.hpp>
#include <CLI/Timer.hpp>

export module thirdparty.core:cli11;

export namespace CLI {
namespace FailureMessage {
    using CLI::FailureMessage::help;
    using CLI::FailureMessage::simple;
} // namespace FailureMessage

using CLI::App;
using CLI::App_p;
using CLI::AppFormatMode;
using CLI::ArgumentMismatch;
using CLI::AsNumberWithUnit;
using CLI::AsSizeValue;
using CLI::BadNameString;
using CLI::Bound;
using CLI::callback_t;
using CLI::CallbackPriority;
using CLI::CallForAllHelp;
using CLI::CallForHelp;
using CLI::CallForVersion;
using CLI::CheckedTransformer;
using CLI::Config;
using CLI::config_extras_mode;
using CLI::ConfigBase;
using CLI::ConfigError;
using CLI::ConfigINI;
using CLI::ConfigItem;
using CLI::ConfigTOML;
using CLI::ConstructionError;
using CLI::ConversionError;
using CLI::deprecate_option;
using CLI::Error;
using CLI::ExcludesError;
using CLI::ExitCodes;
using CLI::ExtrasError;
using CLI::FileError;
using CLI::Formatter;
using CLI::FormatterBase;
using CLI::FormatterLambda;
using CLI::HorribleError;
using CLI::ignore_case;
using CLI::ignore_space;
using CLI::ignore_underscore;
using CLI::IncorrectConstruction;
using CLI::InvalidError;
using CLI::IsMember;
using CLI::MultiOptionPolicy;
using CLI::narrow;
using CLI::Option;
using CLI::Option_group;
using CLI::Option_p;
using CLI::OptionAlreadyAdded;
using CLI::OptionBase;
using CLI::OptionDefaults;
using CLI::OptionNotFound;
using CLI::ParseError;
using CLI::RequiredError;
using CLI::RequiresError;
using CLI::results_t;
using CLI::retire_option;
using CLI::RuntimeError;
using CLI::Success;
using CLI::to_path;
using CLI::Transformer;
using CLI::TransformPairs;
using CLI::TriggerOff;
using CLI::TriggerOn;
using CLI::TypeValidator;
using CLI::ValidationError;
using CLI::Validator_p;
using CLI::widen;

namespace enums {
    using CLI::enums::operator<<;
}

using CLI::operator<<;

// using CLI::AutoTimer;
using CLI::conditional_t;
using CLI::CustomValidator;
using CLI::enable_if_t;
using CLI::FileOnDefaultPath;
using CLI::is_bool;
using CLI::is_copyable_ptr;
using CLI::is_shared_ptr;
using CLI::IsMemberType;
using CLI::make_void;
using CLI::Range;
// using CLI::Timer;
using CLI::Validator;
using CLI::void_t;
} // namespace CLI

// export {
//     using ::operator<<;
// }