/**
 * @file throwEx.cpp
 * @author prbegd
 * @date 2026-03-28
 *
 * Copyright © 2026 Cai Yaoxing
 *
 * This file is part of TheCalculater.
 * TheCalculater is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * TheCalculater is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with TheCalculater. If not, see <https://www.gnu.org/licenses/>.
 */
module thecalculater.throwEx;
import thirdparty.core;

namespace thecalculater {
    ThrowExData::ThrowExData(boost::stacktrace::stacktrace trace, std::exception_ptr cause)
        : trace(std::move(trace)), cause(std::move(cause))
    { }
    ThrowExData::ThrowExData(boost::stacktrace::stacktrace trace)
        : trace(std::move(trace)), cause(nullptr)
    { }
} // namespace thecalculater