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

#include "InputStream.h"
#include "Utf8Decoder.h"
#include <utility>
#include <stdexcept>
#include <cassert>

namespace klex
{

    namespace
    {

        void populate_buffer(CodePointBuffer& buffer,
                             std::istream& stream,
                             std::uint8_t max_num)
        {
            // Maps \r\n to \n and \r to \n.  Needs one extra space in the
            // buffer in case \r was not followed by \n and we need to store
            // that extra code point.
            assert(max_num < buffer.max_size() - 1);
            Utf8Decoder decoder;
            while (max_num >= buffer.size())
            {
                int code_point = decoder.decode(stream);
                if (code_point == '\r')
                {
                    buffer.push_back('\n');
                    code_point = decoder.decode(stream);
                    if (code_point != '\n')
                    {
                        buffer.push_back(code_point);
                    }
                }
                else
                {
                    buffer.push_back(code_point);
                }
            }
        }

    } // close anonymous namespace

    InputStream::InputStream(std::unique_ptr<std::istream>&& stream)
    : stream_{std::move(stream)}
    , buffer_{}
    , line_{1}
    , column_{1}
    {
    }

    int InputStream::get()
    {
        populate_buffer(buffer_, *stream_, 1);
        int code_point = buffer_.front();
        buffer_.pop_front();
        if (code_point == '\n')
        {
            ++line_;
            column_ = 1;
        }
        else if (code_point != EOF)
        {
            ++column_;
        }
        return code_point;
    }

    int InputStream::peek(std::uint8_t offset)
    {
        populate_buffer(buffer_, *stream_, offset);
        return buffer_[offset];
    }

    int InputStream::get_line() const
    {
        return line_;
    }

    int InputStream::get_column() const
    {
        return column_;
    }

} // close klex namespace

