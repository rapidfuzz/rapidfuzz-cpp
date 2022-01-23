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
    int64_t spos;
    int64_t dpos;
    int64_t length;
    MatchingBlock(int64_t aSPos, int64_t aDPos, int64_t aLength)
        : spos(aSPos), dpos(aDPos), length(aLength)
    {}
};

namespace difflib {

template <typename InputIt1, typename InputIt2>
class SequenceMatcher {
public:
    using match_t = std::tuple<int64_t, int64_t, int64_t>;

    SequenceMatcher(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2)
        : a_first(first1), a_last(last1), b_first(first2), b_last(last2)
    {
        int64_t b_len = std::distance(b_first, b_last);
        j2len_.resize(b_len + 1);
        for (int64_t i = 0; i < b_len; ++i) {
            b2j_.create(b_first[i]).push_back(i);
        }
    }

    match_t find_longest_match(int64_t a_low, int64_t a_high, int64_t b_low, int64_t b_high)
    {
        int64_t best_i = a_low;
        int64_t best_j = b_low;
        int64_t best_size = 0;

        // Find longest junk free match
        {
            for (int64_t i = a_low; i < a_high; ++i) {
                const auto& indexes = b2j_[a_first[i]];
                size_t pos = 0;
                int64_t next_val = 0;
                bool found = false;
                for (; pos < indexes.size(); pos++) {
                    int64_t j = indexes[pos];
                    if (j < b_low) continue;

                    next_val = j2len_[j];
                    break;
                }

                for (; pos < indexes.size(); pos++) {
                    int64_t j = indexes[pos];
                    if (j >= b_high) break;

                    found = true;
                    int64_t k = next_val + 1;

                    /* the next value might be overwritten below
                     * so cache it */
                    if (pos + 1 < indexes.size()) {
                        next_val = j2len_[indexes[pos + 1]];
                    }

                    j2len_[j + 1] = k;
                    if (k > best_size) {
                        best_i = i - k + 1;
                        best_j = j - k + 1;
                        best_size = k;
                    }
                }

                if (!found) {
                    std::fill(j2len_.begin() + b_low, j2len_.begin() + b_high, 0);
                }
            }

            std::fill(j2len_.begin() + b_low, j2len_.begin() + b_high, 0);
        }

        while (best_i > a_low && best_j > b_low && a_first[best_i - 1] == b_first[best_j - 1]) {
            --best_i;
            --best_j;
            ++best_size;
        }

        while ((best_i + best_size) < a_high && (best_j + best_size) < b_high &&
               a_first[best_i + best_size] == b_first[best_j + best_size])
        {
            ++best_size;
        }

        return std::make_tuple(best_i, best_j, best_size);
    }

    std::vector<MatchingBlock> get_matching_blocks()
    {
        int64_t a_len = std::distance(a_first, a_last);
        int64_t b_len = std::distance(b_first, b_last);
        // The following are tuple extracting aliases
        std::vector<std::tuple<int64_t, int64_t, int64_t, int64_t>> queue;
        std::vector<match_t> matching_blocks_pass1;

        size_t queue_head = 0;
        queue.reserve(std::min(a_len, b_len));
        queue.emplace_back(0, a_len, 0, b_len);

        while (queue_head < queue.size()) {
            int64_t a_low, a_high, b_low, b_high;
            std::tie(a_low, a_high, b_low, b_high) = queue[queue_head++];
            int64_t spos, dpos, length;
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
        std::sort(common::to_begin(matching_blocks_pass1), common::to_end(matching_blocks_pass1));

        std::vector<MatchingBlock> matching_blocks;

        matching_blocks.reserve(matching_blocks_pass1.size());

        int64_t i1, j1, k1;
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
    std::vector<int64_t> j2len_;
    common::CharHashTable<iterator_type<InputIt2>, std::vector<int64_t>> b2j_;
};
} // namespace difflib

template <typename InputIt1, typename InputIt2>
std::vector<MatchingBlock> get_matching_blocks(InputIt1 first1, InputIt1 last1, InputIt2 first2,
                                               InputIt2 last2)
{
    return difflib::SequenceMatcher<InputIt1, InputIt2>(first1, last1, first2, last2)
        .get_matching_blocks();
}

} /* namespace detail */
} /* namespace rapidfuzz */
