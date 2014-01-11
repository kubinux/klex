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
#include <utility>
#include <stdexcept>

namespace
{

    int num_octets_per_code_point(int first_octet)
    {
        int res = 0;
        if (first_octet != 0 && (first_octet & 0x80) == 0x0)
        {
            res = 1;
        }
        else if ((first_octet & 0xE0) == 0xC0)
        {
            res = 2;
        }
        else if ((first_octet & 0xF0) == 0xE0)
        {
            res = 3;
        }
        else if ((first_octet & 0xF8) == 0xF0)
        {
            res = 4;
        }
        return res;
    }

    int process_first_octet(int first_octet, int& code_point)
    {
        int num = num_octets_per_code_point(first_octet);
        switch (num)
        {
        case 1:
            code_point = first_octet;
            break;
        case 2:
            code_point = first_octet & 0x1F;
            break;
        case 3:
            code_point = first_octet & 0xF;
            break;
        case 4:
            code_point = first_octet & 0x7;
            break;
        default:
            throw std::runtime_error("UTF-8: invalid first octet");
        }
        return num - 1;
    }

    int read_code_point(std::istream& is)
    {
        int octet = is.get();
        if (octet == EOF)
        {
            return 0;
        }

        int code_point = 0;
        int num_octets_left = process_first_octet(octet, code_point);
        while (num_octets_left--)
        {
            octet = is.get();
            if (octet == EOF)
            {
                throw std::runtime_error("UTF-8: unexpected EOF");
            }

            code_point <<= 6;
            code_point |= (0x3F & octet);
        }
        return code_point;
    }

} // close unnamed namespace

namespace klex
{

    InputStream::InputStream(std::unique_ptr<std::istream>&& utf8_stream)
    : utf8_stream_(std::move(utf8_stream))
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
            code_point = read_code_point(*utf8_stream_);
        }
        return code_point;
    }

} // close klex namespace

