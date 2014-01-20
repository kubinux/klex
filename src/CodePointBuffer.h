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
        typedef std::uint8_t size_type;

        size_type size() const
        {
            return end_ - begin_;
        }

        bool empty() const
        {
            return begin_ == end_;
        }

        size_type max_size() const
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

        int operator[](size_type index) const
        {
            assert(index < size());
            return data_[begin_ + index];
        }

    private:
        static constexpr size_type MAX_SIZE =
            std::numeric_limits<size_type>::max();

        size_type begin_ = 0;
        size_type end_ = 0;
        int data_[MAX_SIZE + 1];
    };

} // close klex namespace

#endif // include guard

