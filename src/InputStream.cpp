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

    InputStream::InputStream(std::unique_ptr<std::istream>&& stream)
    : stream_{std::move(stream)}
    , buffer_{}
    , line_{1}
    , column_{1}
    , ignore_line_feed_{false}
    {
    }

    int InputStream::get()
    {
        populate_buffer(1);
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
        populate_buffer(offset);
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

    void InputStream::populate_buffer(std::uint8_t num)
    {
        assert(num < buffer_.max_size());
        Utf8Decoder decoder;
        while (num >= buffer_.size())
        {
            int code_point = decoder.decode(*stream_);
            if (code_point == '\r')
            {
                buffer_.push_back('\n');
                ignore_line_feed_ = true;
            }
            else if (code_point == '\n')
            {
                if (!ignore_line_feed_)
                {
                    buffer_.push_back(code_point);
                }
                ignore_line_feed_ = false;
            }
            else
            {
                buffer_.push_back(code_point);
                ignore_line_feed_ = false;
            }
        }
    }

} // close klex namespace

