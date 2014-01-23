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

#include "../src/CodePointBuffer.h"
#include <gtest/gtest.h>

TEST(CodePointBuffer, empty_1)
{
    klex::CodePointBuffer b;
    ASSERT_TRUE(b.empty());
}

TEST(CodePointBuffer, empty_2)
{
    klex::CodePointBuffer b;
    b.push_back(1);
    ASSERT_FALSE(b.empty());
}

TEST(CodePointBuffer, size_1)
{
    klex::CodePointBuffer b;
    ASSERT_EQ(0, b.size());
}

TEST(CodePointBuffer, size_2)
{
    klex::CodePointBuffer b;
    b.push_back(1);
    ASSERT_EQ(1, b.size());
    b.push_back(2);
    ASSERT_EQ(2, b.size());
}

TEST(CodePointBuffer, push_pop_1)
{
    klex::CodePointBuffer b;
    b.push_back(1);
    b.push_back(2);
    b.push_back(3);
    ASSERT_EQ(3, b.size());

    ASSERT_EQ(1, b.front());
    b.pop_front();
    ASSERT_EQ(2, b.size());

    ASSERT_EQ(2, b.front());
    b.pop_front();
    ASSERT_EQ(1, b.size());

    ASSERT_EQ(3, b.front());
    b.pop_front();
    ASSERT_EQ(0, b.size());
    ASSERT_TRUE(b.empty());
}

TEST(CodePointBuffer, max_size)
{
    klex::CodePointBuffer b;
    ASSERT_EQ(255, b.max_size());
}

TEST(CodePointBuffer, push_pop_2)
{
    klex::CodePointBuffer b;
    std::uint8_t const max = b.max_size();
    for (int i = 0; i < max; ++i)
    {
        b.push_back(i);
    }
    ASSERT_EQ(max, b.size());
    ASSERT_EQ(0, b.front());
    for (int i = 0; i < max; ++i)
    {
        b.pop_front();
    }
    ASSERT_TRUE(b.empty());
    for (int i = 0; i < max; ++i)
    {
        b.push_back(i);
    }
    ASSERT_EQ(max, b.size());
}

TEST(CodePointBuffer, index)
{
    klex::CodePointBuffer b;
    b.push_back(0);
    b.push_back(1);
    b.push_back(2);
    ASSERT_EQ(0, b[0]);
    ASSERT_EQ(1, b[1]);
    ASSERT_EQ(2, b[2]);
    b.pop_front();
    ASSERT_EQ(1, b[0]);
    ASSERT_EQ(2, b[1]);
}

