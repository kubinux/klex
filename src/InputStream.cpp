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

namespace klex
{

    InputStream::InputStream(std::unique_ptr<std::istream>&& stream)
    : stream_(std::move(stream))
    {
    }

    int InputStream::get()
    {
        int code_point = 0;
        if (!buffer_.empty())
        {
            code_point = buffer_.back();
            buffer_.pop_back();
        }
        else
        {
            Utf8Decoder decoder;
            code_point = decoder.decode(*stream_);
            if (code_point == 0)
            {
                throw std::runtime_error("InputStream: NUL character");
            }
            else if (code_point == Utf8Decoder::INVALID)
            {
                throw std::runtime_error("InputStream: invalid character");
            }
            else if (code_point == EOF)
            {
                code_point = 0;
            }
        }
        return code_point;
    }

} // close klex namespace

