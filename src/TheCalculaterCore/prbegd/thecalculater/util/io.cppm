/**
 * @file io.cppm
 * @author prbegd
 * @date 2026-03-15
 *
 * Copyright © 2026 Cai Yaoxing
 *
 * This file is part of TheCalculater.
 * TheCalculater is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * TheCalculater is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with TheCalculater. If not, see <https://www.gnu.org/licenses/>.
 */
export module prbegd.thecalculater.util.io;
import std;

namespace thecalculater::util {
    export class StringViewStreamBuf : public std::streambuf {
    public:
        explicit StringViewStreamBuf(std::string_view sv)
        {
            setg(const_cast<char*>(sv.data()),
                 const_cast<char*>(sv.data()),
                 const_cast<char*>(sv.data()) + sv.size());
        }
    };
    // NOLINTNEXTLINE(fuchsia-multiple-inheritance)
    export class IStringViewStream : public std::istream {
    public:
        explicit IStringViewStream(std::string_view sv)
            : std::istream(&buf_), buf_(sv)
        {
            exceptions(std::istream::badbit);
        }

    private:
        StringViewStreamBuf buf_;
    };
} // namespace thecalculater::util
