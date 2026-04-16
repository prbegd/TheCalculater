/**
 * @file io.cppm
 * @author prbegd
 * @date 2026-03-15
 *
 * Copyright © 2025 Cai Yaoxing
 * SPDX-License-Identifier: GPL-3.0-only
 * This file is part of TheCalculater.
 * See the file LICENSE in the project root or go to
 * <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
 */
export module TheCalculater.util.io;
import std;

namespace TheCalculater::util {
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
} // namespace TheCalculater::util
