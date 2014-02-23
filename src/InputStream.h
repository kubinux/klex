// Copyright (C) 2014 Jakub Lewandowski <jakub.lewandowski@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef INPUTSTREAM_H_INCLUDED_8YDFSC1N
#define INPUTSTREAM_H_INCLUDED_8YDFSC1N

#include "CodePointBuffer.h"
#include <cstdint>
#include <istream>
#include <memory>
#include <vector>

namespace klex
{

    class InputStream
    {
    public:
        explicit InputStream(std::unique_ptr<std::istream>&& stream);

        int get();

        int peek(std::uint8_t offset);

        int get_line() const;

        int get_column() const;

    private:
        void populate_buffer(std::uint8_t num);

    private:
        std::unique_ptr<std::istream> stream_;
        CodePointBuffer buffer_;
        int line_;
        int column_;
        bool ignore_line_feed_;
    };

} // close klex namespace

#endif // include guard

