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

#include "Utf8Decoder.h"

namespace klex
{

    namespace
    {

        int utf8_continue(std::istream& is, int current_value)
        {
            int result = is.get();
            if ((result & 0xC0) == 0x80)
            {
                result &= 0x3F;
                result |= (current_value << 6);
            }
            else
            {
                result = Utf8Decoder::INVALID;
            }
            return result;
        }

        template <int Min, int Max>
        int utf8_continue(std::istream& is, int current_value)
        {
            int result = is.get();
            if (result >= Min && result <= Max)
            {
                result &= 0x3F;
                result |= (current_value << 6);
            }
            else
            {
                result = Utf8Decoder::INVALID;
            }
            return result;
        }

    } // close unnamed namespace

    int const Utf8Decoder::INVALID = 0x10FFFF + 1;

    int Utf8Decoder::decode(std::istream& is) const
    {
        int result = is.get();

        if ((result & 0x80) == 0x0 || result == EOF)
        {
            // nothing else needs to be done
        }
        else if ((result & 0xE0) == 0xC0)
        {
            if (result == 0xC0 || result == 0xC1)
            {
                result = Utf8Decoder::INVALID;
            }
            else
            {
                result &= 0x1F;
                result = utf8_continue(is, result);
            }
        }
        else if ((result & 0xF0) == 0xE0)
        {
            if (result == 0xE0)
            {
                result &= 0xF;
                result = utf8_continue<0xA0, 0xBF>(is, result);
            }
            else if (result == 0xED)
            {
                result &= 0xF;
                result = utf8_continue<0x80, 0x9F>(is, result);
            }
            else
            {
                result &= 0xF;
                result = utf8_continue(is, result);
            }
            if (result != Utf8Decoder::INVALID)
            {
                result = utf8_continue(is, result);
            }
        }
        else if (result <= 0xF4)
        {
            if (result == 0xF0)
            {
                result &= 0x7;
                result = utf8_continue<0x90, 0xBF>(is, result);
            }
            else if (result == 0xF4)
            {
                result &= 0x7;
                result = utf8_continue<0x80, 0x8F>(is, result);
            }
            else
            {
                result &= 0x7;
                result = utf8_continue(is, result);
            }
            if (result != Utf8Decoder::INVALID)
            {
                result = utf8_continue(is, result);
                if (result != Utf8Decoder::INVALID)
                {
                    result = utf8_continue(is, result);
                }
            }
        }
        else
        {
            result = Utf8Decoder::INVALID;
        }
        return result;
    }

} // close klex namespace

