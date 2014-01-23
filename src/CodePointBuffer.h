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

#ifndef CODEPOINTBUFFER_H_INCLUDED_CJZ6QWYX
#define CODEPOINTBUFFER_H_INCLUDED_CJZ6QWYX

#include <cassert>
#include <cstdint>
#include <limits>

namespace klex
{

    class CodePointBuffer
    {
    public:
        std::uint8_t size() const
        {
            return end_ - begin_;
        }

        bool empty() const
        {
            return begin_ == end_;
        }

        std::uint8_t max_size() const
        {
            return MAX_SIZE;
        }

        void push_back(int cp)
        {
            assert(size() < max_size());
            data_[end_] = cp;
            ++end_;
        }

        void pop_front()
        {
            assert(begin_ != end_);
            ++begin_;
        }

        int front() const
        {
            assert(!empty());
            return data_[begin_];
        }

        int operator[](std::uint8_t index) const
        {
            assert(index < size());
            return data_[begin_ + index];
        }

    private:
        static constexpr std::uint8_t MAX_SIZE =
            std::numeric_limits<std::uint8_t>::max();

        std::uint8_t begin_ = 0;
        std::uint8_t end_ = 0;
        int data_[MAX_SIZE + 1];
    };

} // close klex namespace

#endif // include guard

