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
#include <memory>
#include <sstream>
#include <string>

namespace
{

    std::unique_ptr<std::istream> make_stream(std::string const& str)
    {
        return std::unique_ptr<std::istream>(new std::istringstream(str));
    }

} // close unnamed namespace

TEST(InputStream, simple_get)
{
    klex::InputStream is(make_stream("abc"));
    ASSERT_EQ('a', is.get());
    ASSERT_EQ('b', is.get());
    ASSERT_EQ('c', is.get());
    ASSERT_EQ(EOF, is.get());
}

TEST(InputStream, simple_peek)
{
    klex::InputStream is(make_stream("abc"));
    ASSERT_EQ('a', is.peek(0));
    ASSERT_EQ('b', is.peek(1));
    ASSERT_EQ('c', is.peek(2));
    ASSERT_EQ(EOF, is.peek(3));
    ASSERT_EQ('a', is.get());
}

TEST(InputStream, simple_get_column)
{
    klex::InputStream is(make_stream("abc"));
    ASSERT_EQ(1, is.get_column());
    is.peek(2);
    ASSERT_EQ(1, is.get_column());
    is.get();
    ASSERT_EQ(2, is.get_column());
    is.get();
    ASSERT_EQ(3, is.get_column());
    is.get();
    ASSERT_EQ(4, is.get_column());
    ASSERT_EQ(EOF, is.get());
    ASSERT_EQ(4, is.get_column());
}

TEST(InputStream, new_line_1)
{
    klex::InputStream is(make_stream("a\n\nb\n"));
    ASSERT_EQ(1, is.get_line());
    ASSERT_EQ(1, is.get_line());
    ASSERT_EQ('a', is.get());
    ASSERT_EQ(1, is.get_line());
    ASSERT_EQ(2, is.get_column());
    ASSERT_EQ('\n', is.get());
    ASSERT_EQ(2, is.get_line());
    ASSERT_EQ(1, is.get_column());
    ASSERT_EQ('\n', is.get());
    ASSERT_EQ(3, is.get_line());
    ASSERT_EQ(1, is.get_column());
    ASSERT_EQ('b', is.get());
    ASSERT_EQ(3, is.get_line());
    ASSERT_EQ(2, is.get_column());
    ASSERT_EQ('\n', is.get());
    ASSERT_EQ(4, is.get_line());
    ASSERT_EQ(1, is.get_column());
    ASSERT_EQ(EOF, is.get());
    ASSERT_EQ(4, is.get_line());
    ASSERT_EQ(1, is.get_column());
}

TEST(InputStream, new_line_2)
{
    klex::InputStream is(make_stream("a\r\rb\r"));
    ASSERT_EQ(1, is.get_line());
    ASSERT_EQ(1, is.get_line());
    ASSERT_EQ('a', is.get());
    ASSERT_EQ(1, is.get_line());
    ASSERT_EQ(2, is.get_column());
    ASSERT_EQ('\n', is.get());
    ASSERT_EQ(2, is.get_line());
    ASSERT_EQ(1, is.get_column());
    ASSERT_EQ('\n', is.get());
    ASSERT_EQ(3, is.get_line());
    ASSERT_EQ(1, is.get_column());
    ASSERT_EQ('b', is.get());
    ASSERT_EQ(3, is.get_line());
    ASSERT_EQ(2, is.get_column());
    ASSERT_EQ('\n', is.get());
    ASSERT_EQ(4, is.get_line());
    ASSERT_EQ(1, is.get_column());
    ASSERT_EQ(EOF, is.get());
    ASSERT_EQ(4, is.get_line());
    ASSERT_EQ(1, is.get_column());
}

TEST(InputStream, new_line_3)
{
    klex::InputStream is(make_stream("a\r\n\r\r\nb\r"));
    ASSERT_EQ(1, is.get_line());
    ASSERT_EQ(1, is.get_line());
    ASSERT_EQ('a', is.get());
    ASSERT_EQ(1, is.get_line());
    ASSERT_EQ(2, is.get_column());
    ASSERT_EQ('\n', is.get());
    ASSERT_EQ(2, is.get_line());
    ASSERT_EQ(1, is.get_column());
    ASSERT_EQ('\n', is.get());
    ASSERT_EQ(3, is.get_line());
    ASSERT_EQ(1, is.get_column());
    ASSERT_EQ('\n', is.get());
    ASSERT_EQ(4, is.get_line());
    ASSERT_EQ(1, is.get_column());
    ASSERT_EQ('b', is.get());
    ASSERT_EQ(4, is.get_line());
    ASSERT_EQ(2, is.get_column());
    ASSERT_EQ('\n', is.get());
    ASSERT_EQ(5, is.get_line());
    ASSERT_EQ(1, is.get_column());
    ASSERT_EQ(EOF, is.get());
    ASSERT_EQ(5, is.get_line());
    ASSERT_EQ(1, is.get_column());
}

TEST(InputStream, new_line_4)
{
    klex::InputStream is(make_stream("\rb\n"));
    ASSERT_EQ('\n', is.get());
    ASSERT_EQ(2, is.get_line());
    ASSERT_EQ(1, is.get_column());
    ASSERT_EQ('b', is.get());
    ASSERT_EQ(2, is.get_line());
    ASSERT_EQ(2, is.get_column());
    ASSERT_EQ('\n', is.get());
    ASSERT_EQ(3, is.get_line());
    ASSERT_EQ(1, is.get_column());
    ASSERT_EQ(EOF, is.get());
    ASSERT_EQ(3, is.get_line());
    ASSERT_EQ(1, is.get_column());
}

