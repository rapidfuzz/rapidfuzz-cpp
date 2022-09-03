// The MIT License (MIT)
//
// Copyright (c) 2020 Max Bachmann
// Copyright (c) 2014 Jean-Bernard Jansen
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#pragma once

#include <rapidfuzz/details/common.hpp>

#include <algorithm>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace rapidfuzz {
namespace detail {
struct MatchingBlock {
    size_t spos;
    size_t dpos;
    size_t length;
    MatchingBlock(size_t aSPos, size_t aDPos, size_t aLength) : spos(aSPos), dpos(aDPos), length(aLength)
    {}
};

namespace difflib {

template <typename InputIt1, typename InputIt2>
class SequenceMatcher {
    using Index = size_t;

public:
    using match_t = std::tuple<Index, Index, Index>;

    SequenceMatcher(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2)
        : a_first(first1), a_last(last1), b_first(first2), b_last(last2)
    {
        auto b_len = static_cast<size_t>(std::distance(b_first, b_last));
        j2len_.resize(b_len + 1);
        newj2len_.resize(b_len + 1);
        for (Index i = 0; i < b_len; ++i) {
            b2j_[b_first[static_cast<ptrdiff_t>(i)]].push_back(i);
        }
    }

    match_t find_longest_match(Index a_low, Index a_high, Index b_low, Index b_high)
    {
        Index best_i = a_low;
        Index best_j = b_low;
        Index best_size = 0;

        // Find longest junk free match
        for (Index i = a_low; i < a_high; ++i) {
            auto iter = b2j_.find(a_first[static_cast<ptrdiff_t>(i)]);
            if (iter != std::end(b2j_)) {
                for (Index j : iter->second) {
                    /* a[i] matches b[j] */
                    if (j < b_low) continue;
                    if (j >= b_high) break;

                    auto k = j2len_[j] + 1;
                    newj2len_[j + 1] = k;

                    if (k > best_size) {
                        best_i = i - k + 1;
                        best_j = j - k + 1;
                        best_size = k;
                    }
                }
            }

            std::swap(j2len_, newj2len_);
            std::fill(newj2len_.begin() + static_cast<ptrdiff_t>(b_low),
                      newj2len_.begin() + static_cast<ptrdiff_t>(b_high) + 1, 0);
        }

        std::fill(j2len_.begin() + static_cast<ptrdiff_t>(b_low),
                  j2len_.begin() + static_cast<ptrdiff_t>(b_high) + 1, 0);

        while (best_i > a_low && best_j > b_low &&
               a_first[static_cast<ptrdiff_t>(best_i) - 1] == b_first[static_cast<ptrdiff_t>(best_j) - 1])
        {
            --best_i;
            --best_j;
            ++best_size;
        }

        while ((best_i + best_size) < a_high && (best_j + best_size) < b_high &&
               a_first[static_cast<ptrdiff_t>(best_i + best_size)] ==
                   b_first[static_cast<ptrdiff_t>(best_j + best_size)])
        {
            ++best_size;
        }

        return std::make_tuple(best_i, best_j, best_size);
    }

    std::vector<MatchingBlock> get_matching_blocks()
    {
        auto a_len = static_cast<size_t>(std::distance(a_first, a_last));
        auto b_len = static_cast<size_t>(std::distance(b_first, b_last));
        // The following are tuple extracting aliases
        std::vector<std::tuple<Index, Index, Index, Index>> queue;
        std::vector<match_t> matching_blocks_pass1;

        size_t queue_head = 0;
        queue.reserve(std::min(a_len, b_len));
        queue.emplace_back(0, a_len, 0, b_len);

        while (queue_head < queue.size()) {
            Index a_low, a_high, b_low, b_high;
            std::tie(a_low, a_high, b_low, b_high) = queue[queue_head++];
            Index spos, dpos, length;
            std::tie(spos, dpos, length) = find_longest_match(a_low, a_high, b_low, b_high);
            if (length) {
                if (a_low < spos && b_low < dpos) {
                    queue.emplace_back(a_low, spos, b_low, dpos);
                }
                if ((spos + length) < a_high && (dpos + length) < b_high) {
                    queue.emplace_back(spos + length, a_high, dpos + length, b_high);
                }
                matching_blocks_pass1.emplace_back(spos, dpos, length);
            }
        }
        std::sort(detail::to_begin(matching_blocks_pass1), detail::to_end(matching_blocks_pass1));

        std::vector<MatchingBlock> matching_blocks;

        matching_blocks.reserve(matching_blocks_pass1.size());

        Index i1, j1, k1;
        i1 = j1 = k1 = 0;

        for (match_t const& m : matching_blocks_pass1) {
            if (i1 + k1 == std::get<0>(m) && j1 + k1 == std::get<1>(m)) {
                k1 += std::get<2>(m);
            }
            else {
                if (k1) {
                    matching_blocks.emplace_back(i1, j1, k1);
                }
                std::tie(i1, j1, k1) = m;
            }
        }
        if (k1) {
            matching_blocks.emplace_back(i1, j1, k1);
        }
        matching_blocks.emplace_back(a_len, b_len, 0);

        return matching_blocks;
    }

protected:
    InputIt1 a_first;
    InputIt1 a_last;
    InputIt2 b_first;
    InputIt2 b_last;

private:
    // Cache to avoid reallocations
    std::vector<Index> j2len_;
    std::vector<Index> newj2len_;
    std::unordered_map<iter_value_t<InputIt2>, std::vector<Index>> b2j_;
};
} // namespace difflib

template <typename InputIt1, typename InputIt2>
std::vector<MatchingBlock> get_matching_blocks(InputIt1 first1, InputIt1 last1, InputIt2 first2,
                                               InputIt2 last2)
{
    return difflib::SequenceMatcher<InputIt1, InputIt2>(first1, last1, first2, last2).get_matching_blocks();
}

} /* namespace detail */
} /* namespace rapidfuzz */
