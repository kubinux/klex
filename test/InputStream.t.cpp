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

#include "../src/InputStream.h"

#include <gtest/gtest.h>

#include <iomanip>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

//
// UTF-8 tests are based on
// http://www.cl.cam.ac.uk/~mgk25/ucs/examples/UTF-8-test.txt
// by Markus Kuhn.
//

namespace
{

    inline
    std::unique_ptr<std::istream> make_input_stream(std::string const& str)
    {
        return std::unique_ptr<std::istream>(new std::istringstream(str));
    }

} // close unnamed namespace

// Make sure correct utf-8 sequences with all possible lenghts work fine.
TEST(InputStream, correct_utf8_text)
{
    std::string input_data{
        '\xce', '\xba',                 // κ
        '\xe1', '\xbd', '\xb9',         // ό
        '\xcf', '\x83',                 // σ
        '\xce', '\xbc',                 // μ
        '\xce', '\xb5',                 // ε
        '\xf0', '\xa4', '\xad', '\xa2', // ??
    };

    std::vector<int> const expected{
        0x03ba,
        0x1f79,
        0x03c3,
        0x03bc,
        0x03b5,
        0x24b62,
    };

    klex::InputStream is(make_input_stream(input_data));
    std::vector<int> actual;
    while (int cp = is.get())
    {
        actual.push_back(cp);
    }
    ASSERT_EQ(expected, actual);
}

// Make sure InputStream doesn't allow Nul bytes in the utf-8 stream.
TEST(InputStream, nul_byte)
{
    std::string input_data{
        '\xce', '\xba',         // κ
        '\x0',                  // Nul
        '\xe1', '\xbd', '\xb9', // ό
    };

    klex::InputStream is(make_input_stream(input_data));
    ASSERT_EQ(0x03ba, is.get());
    ASSERT_THROW(is.get(), std::runtime_error);
}

