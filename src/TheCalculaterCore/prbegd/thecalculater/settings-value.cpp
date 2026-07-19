/**
 * Copyright © 2026 Cai Yaoxing
 *
 * This file is part of TheCalculater.
 * TheCalculater is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * TheCalculater is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with TheCalculater. If not, see <https://www.gnu.org/licenses/>.
 */
module;

module prbegd.thecalculater.settings;
import std;

namespace thecalculater::settings {
std::string Value::type() const noexcept
{
    switch (index()) {
    case 0:
        return "BooleanValue";
    case 1:
        return "ListValue";
    case 2:
        return "ObjectValue";
    case 3:
        return "StringValue";
    case 4:
        return "IntegerValue";
    case 5:
        return "DecimalValue";
    default:
        return "Nothing";
    }
}
} // namespace thecalculater::settings