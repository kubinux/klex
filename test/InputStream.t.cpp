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
#include <cstdint>

//
// UTF-8 tests are based on
// http://www.cl.cam.ac.uk/~mgk25/ucs/examples/UTF-8-test.txt
// by Markus Kuhn.
//

//
// A quick way to obtain the utf-8 bytes for a given code point
// is to fire up Python, for example:
//
// >>> unichr(0x80).encode('utf-8')
// '\xc2\x80'
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

TEST(InputStream, first_possible_sequence_of_len_1)
{
    // Nul (U-00000000) is not allowed in our case
    // so we start with U-00000001.
    std::string input_data{"\x1"};
    klex::InputStream is{make_input_stream(input_data)};
    ASSERT_EQ(0x1, is.get());
    ASSERT_EQ(0x0, is.get());
}

TEST(InputStream, first_possible_sequence_of_len_2)
{
    // U-00000080
    std::string input_data{"\xc2\x80"};
    klex::InputStream is{make_input_stream(input_data)};
    ASSERT_EQ(0x80, is.get());
    ASSERT_EQ(0x0, is.get());
}

TEST(InputStream, first_possible_sequence_of_len_3)
{
    // U-00000800
    std::string input_data{"\xe0\xa0\x80"};
    klex::InputStream is{make_input_stream(input_data)};
    ASSERT_EQ(0x800, is.get());
    ASSERT_EQ(0x0, is.get());
}

TEST(InputStream, first_possible_sequence_of_len_4)
{
    // U-00010000
    std::string input_data{"\xf0\x90\x80\x80"};
    klex::InputStream is{make_input_stream(input_data)};
    ASSERT_EQ(0x10000, is.get());
    ASSERT_EQ(0x0, is.get());
}

TEST(InputStream, last_possible_sequence_of_len_1)
{
    // U-0000007F
    std::string input_data{"\x7f"};
    klex::InputStream is{make_input_stream(input_data)};
    ASSERT_EQ(0x7f, is.get());
    ASSERT_EQ(0x0, is.get());
}

TEST(InputStream, last_possible_sequence_of_len_2)
{
    // U-000007FF
    std::string input_data{"\xdf\xbf"};
    klex::InputStream is{make_input_stream(input_data)};
    ASSERT_EQ(0x7ff, is.get());
    ASSERT_EQ(0x0, is.get());
}

TEST(InputStream, last_possible_sequence_of_len_3)
{
    // U-0000FFFF
    std::string input_data{"\xef\xbf\xbf"};
    klex::InputStream is{make_input_stream(input_data)};
    ASSERT_EQ(0xffff, is.get());
    ASSERT_EQ(0x0, is.get());
}

TEST(InputStream, last_possible_sequence_of_len_4)
{
    // U-0010FFFF
    std::string input_data{"\xf4\x8f\xbf\xbf"};
    klex::InputStream is{make_input_stream(input_data)};
    ASSERT_EQ(0x10ffff, is.get());
    ASSERT_EQ(0x0, is.get());
}

TEST(InputStream, code_point_0000D7FF)
{
    // U-0000D7FF
    std::string input_data{"\xed\x9f\xbf"};
    klex::InputStream is{make_input_stream(input_data)};
    ASSERT_EQ(0xd7ff, is.get());
    ASSERT_EQ(0x0, is.get());
}

TEST(InputStream, code_point_0000E000)
{
    // U-0000D7FF
    std::string input_data{"\xee\x80\x80"};
    klex::InputStream is{make_input_stream(input_data)};
    ASSERT_EQ(0xe000, is.get());
    ASSERT_EQ(0x0, is.get());
}

TEST(InputStream, code_point_0000FFFD)
{
    // U-0000FFFD
    std::string input_data{"\xef\xbf\xbd"};
    klex::InputStream is{make_input_stream(input_data)};
    ASSERT_EQ(0xfffd, is.get());
    ASSERT_EQ(0x0, is.get());
}

TEST(InputStream, code_point_00110000)
{
    // U-00110000
    std::string input_data{"\xf4\x90\x80\x80"};
    klex::InputStream is{make_input_stream(input_data)};
    ASSERT_THROW(is.get(), std::runtime_error);
}

TEST(InputStream, invalid_first_continuation_octet_11)
{
    std::string input_data{"\xee\xc0\x80"};
    klex::InputStream is{make_input_stream(input_data)};
    ASSERT_THROW(is.get(), std::runtime_error);
}

TEST(InputStream, invalid_first_continuation_octet_01)
{
    std::string input_data{"\xee\x40\x80"};
    klex::InputStream is{make_input_stream(input_data)};
    ASSERT_THROW(is.get(), std::runtime_error);
}

TEST(InputStream, invalid_first_continuation_octet_00)
{
    std::string input_data{"\xee\x00\x80"};
    klex::InputStream is{make_input_stream(input_data)};
    ASSERT_THROW(is.get(), std::runtime_error);
}

TEST(InputStream, invalid_second_continuation_octet_11)
{
    std::string input_data{"\xee\x80\xc0"};
    klex::InputStream is{make_input_stream(input_data)};
    ASSERT_THROW(is.get(), std::runtime_error);
}

TEST(InputStream, invalid_second_continuation_octet_01)
{
    std::string input_data{"\xee\x80\x40"};
    klex::InputStream is{make_input_stream(input_data)};
    ASSERT_THROW(is.get(), std::runtime_error);
}

TEST(InputStream, invalid_second_continuation_octet_00)
{
    std::string input_data{"\xee\x80\x00"};
    klex::InputStream is{make_input_stream(input_data)};
    ASSERT_THROW(is.get(), std::runtime_error);
}

TEST(InputStream, invalid_third_continuation_octet_11)
{
    std::string input_data{"\xf4\x90\x80\x80"};
    klex::InputStream is{make_input_stream(input_data)};
    ASSERT_THROW(is.get(), std::runtime_error);
}

TEST(InputStream, invalid_third_continuation_octet_01)
{
    std::string input_data{"\xf4\x90\x80\x80"};
    klex::InputStream is{make_input_stream(input_data)};
    ASSERT_THROW(is.get(), std::runtime_error);
}

TEST(InputStream, invalid_third_continuation_octet_00)
{
    std::string input_data{"\xf4\x90\x80\x80"};
    klex::InputStream is{make_input_stream(input_data)};
    ASSERT_THROW(is.get(), std::runtime_error);
}

TEST(InputStream, invalid_first_octet_x80)
{
    std::string input_data{"\x80"};
    klex::InputStream is{make_input_stream(input_data)};
    ASSERT_THROW(is.get(), std::runtime_error);
}

TEST(InputStream, invalid_first_octet_xBF)
{
    std::string input_data{"\xbf"};
    klex::InputStream is{make_input_stream(input_data)};
    ASSERT_THROW(is.get(), std::runtime_error);
}

