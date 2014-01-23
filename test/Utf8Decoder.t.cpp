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

#include "../src/Utf8Decoder.h"
#include <gtest/gtest.h>
#include <sstream>
#include <string>

//   Code Points         1st      2nd      3rd      4th
// --------------------------------------------------------
// U+0000....U+007F     00..7F
// U+0080....U+07FF     C2..DF   80..BF
// U+0800....U+0FFF     E0       A0..BF   80..BF
// U+1000....U+CFFF     E1..EC   80..BF   80..BF
// U+D000....U+D7FF     ED       80..9F   80..BF
// U+E000....U+FFFF     EE..EF   80..BF   80..BF
// U+10000...U+3FFFF    F0       90..BF   80..BF   80..BF
// U+40000...U+FFFFF    F1..F3   80..BF   80..BF   80..BF
// U+100000..U+10FFFF   F4       80..8F   80..BF   80..BF

TEST(Utf8Decoder, empty_stream)
{
    std::istringstream is;
    klex::Utf8Decoder decoder;
    ASSERT_EQ(EOF, decoder.decode(is));
    ASSERT_EQ(EOF, decoder.decode(is));
    ASSERT_EQ(EOF, decoder.decode(is));
}

TEST(Utf8Decoder, bytes_00)
{
    std::string str("\x0", 1);
    std::istringstream is(str);
    klex::Utf8Decoder decoder;
    ASSERT_EQ(0, decoder.decode(is));
    ASSERT_EQ(EOF, decoder.decode(is));
}

TEST(Utf8Decoder, bytes_7F)
{
    std::istringstream is("\x7F");
    klex::Utf8Decoder decoder;
    ASSERT_EQ(0x7F, decoder.decode(is));
    ASSERT_EQ(EOF, decoder.decode(is));
}

TEST(Utf8Decoder, bytes_80)
{
    std::istringstream is("\x80");
    klex::Utf8Decoder decoder;
    ASSERT_EQ(klex::Utf8Decoder::INVALID, decoder.decode(is));
    ASSERT_EQ(EOF, decoder.decode(is));
}

TEST(Utf8Decoder, bytes_C0)
{
    std::istringstream is("\xC0");
    klex::Utf8Decoder decoder;
    ASSERT_EQ(klex::Utf8Decoder::INVALID, decoder.decode(is));
    ASSERT_EQ(EOF, decoder.decode(is));
}

TEST(Utf8Decoder, bytes_C0_AF)
{
    std::istringstream is("\xC0\xAF");
    klex::Utf8Decoder decoder;
    ASSERT_EQ(klex::Utf8Decoder::INVALID, decoder.decode(is));
    ASSERT_EQ(klex::Utf8Decoder::INVALID, decoder.decode(is));
    ASSERT_EQ(EOF, decoder.decode(is));
}

TEST(Utf8Decoder, bytes_C1_80)
{
    std::istringstream is("\xC1\x80");
    klex::Utf8Decoder decoder;
    ASSERT_EQ(klex::Utf8Decoder::INVALID, decoder.decode(is));
    ASSERT_EQ(klex::Utf8Decoder::INVALID, decoder.decode(is));
    ASSERT_EQ(EOF, decoder.decode(is));
}

TEST(Utf8Decoder, bytes_C2_80)
{
    std::istringstream is("\xC2\x80");
    klex::Utf8Decoder decoder;
    ASSERT_EQ(0x80, decoder.decode(is));
    ASSERT_EQ(EOF, decoder.decode(is));
}

TEST(Utf8Decoder, bytes_C2_BF)
{
    std::istringstream is("\xC2\xBF");
    klex::Utf8Decoder decoder;
    ASSERT_EQ(0xBF, decoder.decode(is));
    ASSERT_EQ(EOF, decoder.decode(is));
}

TEST(Utf8Decoder, bytes_C2_7F)
{
    std::istringstream is("\xC2\x7F");
    klex::Utf8Decoder decoder;
    ASSERT_EQ(0xFFFD, decoder.decode(is));
    ASSERT_EQ(0x7F, decoder.decode(is));
    ASSERT_EQ(EOF, decoder.decode(is));
}

TEST(Utf8Decoder, bytes_C2_C0)
{
    std::istringstream is("\xC2\xC0");
    klex::Utf8Decoder decoder;
    ASSERT_EQ(0xFFFD, decoder.decode(is));
    ASSERT_EQ(0xFFFD, decoder.decode(is));
    ASSERT_EQ(EOF, decoder.decode(is));
}

TEST(Utf8Decoder, correct_utf8_text)
{
    std::string input_data{'\xce', '\xba',                 // κ
                           '\xe1', '\xbd', '\xb9',         // ό
                           '\xcf', '\x83',                 // σ
                           '\xce', '\xbc',                 // μ
                           '\xce', '\xb5',                 // ε
                           '\xf0', '\xa4', '\xad', '\xa2', // ??
    };

    std::istringstream is{input_data};
    klex::Utf8Decoder decoder;

    ASSERT_EQ(0x03ba, decoder.decode(is));
    ASSERT_EQ(0x1f79, decoder.decode(is));
    ASSERT_EQ(0x03c3, decoder.decode(is));
    ASSERT_EQ(0x03bc, decoder.decode(is));
    ASSERT_EQ(0x03b5, decoder.decode(is));
    ASSERT_EQ(0x24b62, decoder.decode(is));
    ASSERT_EQ(EOF, decoder.decode(is));
}

TEST(Utf8Decoder, invalid_seq_replacement_1)
{
    std::istringstream is("\xC2\x41\x42");
    klex::Utf8Decoder decoder;
    ASSERT_EQ(0xFFFD, decoder.decode(is));
    ASSERT_EQ(0x41, decoder.decode(is));
    ASSERT_EQ(0x42, decoder.decode(is));
    ASSERT_EQ(EOF, decoder.decode(is));
}

TEST(Utf8Decoder, invalid_seq_replacement_2)
{
    std::istringstream is("\xF0\x90\x80\x41");
    klex::Utf8Decoder decoder;
    ASSERT_EQ(0xFFFD, decoder.decode(is));
    ASSERT_EQ(0x41, decoder.decode(is));
    ASSERT_EQ(EOF, decoder.decode(is));
}

TEST(Utf8Decoder, invalid_seq_replacement_3)
{
    std::istringstream is(
        "\x61\xF1\x80\x80\xE1\x80\xC2\x62\x80\x63\x80\xBF\x64");
    klex::Utf8Decoder decoder;
    ASSERT_EQ(0x61, decoder.decode(is));
    ASSERT_EQ(0xFFFD, decoder.decode(is));
    ASSERT_EQ(0xE1, is.peek());
    ASSERT_EQ(0xFFFD, decoder.decode(is));
    ASSERT_EQ(0xC2, is.peek());
    ASSERT_EQ(0xFFFD, decoder.decode(is));
    ASSERT_EQ(0x62, decoder.decode(is));
    ASSERT_EQ(0xFFFD, decoder.decode(is));
    ASSERT_EQ(0x63, decoder.decode(is));
    ASSERT_EQ(0x80, is.peek());
    ASSERT_EQ(0xFFFD, decoder.decode(is));
    ASSERT_EQ(0xBF, is.peek());
    ASSERT_EQ(0xFFFD, decoder.decode(is));
    ASSERT_EQ(0x64, decoder.decode(is));
    ASSERT_EQ(EOF, decoder.decode(is));
}

