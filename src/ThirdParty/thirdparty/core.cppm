/**
 * Copyright © 2026 Cai Yaoxing
 *
 * This file is part of TheCalculater.
 * TheCalculater is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * TheCalculater is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with TheCalculater. If not, see <https://www.gnu.org/licenses/>.
 */
// TODO(P3): Screw CMake. It has been six years after C++ Modules's release and their ****** team still hasn't support header units, which forced us to fill our project with a bunch of glue. XMake will take over of it.
export module thirdparty.core;
export import :boost;
export import :cli11;
export import :cstd;
export import :json5cpp;
export import :jsoncpp;
export import :posixapi;
export import :spdlog;
export import :winapi;