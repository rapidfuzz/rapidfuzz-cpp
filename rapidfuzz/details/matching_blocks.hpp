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

#include "type_traits.hpp"

#include <unordered_map>
#include <algorithm>
#include <tuple>
#include <algorithm>
#include <memory>
#include <vector>


namespace rapidfuzz {

struct MatchingBlock {
  std::size_t spos;
  std::size_t dpos;
  std::size_t length;	
  MatchingBlock(std::size_t spos, std::size_t dpos, std::size_t length)
      : spos(spos), dpos(dpos), length(length)
  {}
};


namespace difflib {

template <typename Sentence1, typename Sentence2>
class SequenceMatcher {
 public:
  using match_t = std::tuple<size_t, size_t, size_t>;

  SequenceMatcher(Sentence1 const& a, Sentence2 const& b)
  : a_(a), b_(b) {
    j2len_.resize(b.size()+1);
  }

  match_t find_longest_match(size_t a_low, size_t a_high, size_t b_low, size_t b_high) {
    size_t best_i = a_low;
    size_t best_j = b_low;
    size_t best_size = 0;

    // Find longest junk free match
    {
      j2_values_to_erase_.clear();
      for(size_t i = a_low; i < a_high; ++i) {
        j2_values_to_affect_.clear();

        for(size_t j = b_low; j < b_high; ++j) {
          if (b_[j] != a_[i]) {
            continue;
          }

          size_t k = j2len_[j] + 1;
          j2_values_to_affect_.emplace_back(j+1,k);
          if (k > best_size) {
            best_i = i - k + 1;
            best_j = j - k + 1;
            best_size = k;
          }
        }

        for(auto const& elem : j2_values_to_erase_) {
          j2len_[elem.first] = 0;
        }
        for(auto const& elem : j2_values_to_affect_) {
          j2len_[elem.first] = elem.second;
        }
        std::swap(j2_values_to_erase_, j2_values_to_affect_);
      }
      for(auto const& elem : j2_values_to_erase_) {
        j2len_[elem.first] = 0;
      }
    }

    while (best_i > a_low && best_j > b_low && a_[best_i-1] == b_[best_j-1]) {
      --best_i;
      --best_j;
      ++best_size;
    }

    while ((best_i+best_size) < a_high && (best_j+best_size) < b_high
           && a_[best_i+best_size] == b_[best_j+best_size])
    {
      ++best_size;
    }

    return std::make_tuple(best_i, best_j, best_size);
  }

  std::vector<MatchingBlock> get_matching_blocks() {
    // The following are tuple extracting aliases
    std::vector<std::tuple<size_t, size_t, size_t, size_t>> queue;
    std::vector<match_t> matching_blocks_pass1;

    std::size_t queue_head = 0;
    queue.reserve(std::min(a_.size(), b_.size()));
    queue.emplace_back(0, a_.size(), 0, b_.size());

    while(queue_head < queue.size()) {
      size_t a_low, a_high, b_low, b_high;
      std::tie(a_low, a_high, b_low, b_high) = queue[queue_head++];
      std::size_t spos, dpos, length;
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
    std::sort(std::begin(matching_blocks_pass1), std::end(matching_blocks_pass1));

    std::vector<MatchingBlock> matching_blocks;

    matching_blocks.reserve(matching_blocks_pass1.size());

    size_t i1, j1, k1;
    i1 = j1 = k1 = 0;

    for(match_t const& m : matching_blocks_pass1) {
      if (i1 + k1 == std::get<0>(m) && j1 + k1 == std::get<1>(m)) {
        k1 += std::get<2>(m);
      }
      else {
        if (k1) matching_blocks.emplace_back(i1, j1, k1);
        std::tie(i1, j1, k1) = m;
      }
    }
    if (k1) matching_blocks.emplace_back(i1, j1, k1);
    matching_blocks.emplace_back(a_.size(), b_.size(), 0);

    return matching_blocks;
  }

protected:
  Sentence1 a_;
  Sentence2 b_;

private:
  // Cache to avoid reallocations
  std::vector<size_t> j2len_;
  std::vector<std::pair<size_t, size_t>> j2_values_to_affect_;
  std::vector<std::pair<size_t, size_t>> j2_values_to_erase_;
};

}  // namespace difflib

template<typename Sentence1, typename Sentence2>
std::vector<MatchingBlock> get_matching_blocks(Sentence1 s1, Sentence2 s2) {
  return difflib::SequenceMatcher<Sentence1, Sentence2>(s1, s2).get_matching_blocks();
}

} /* namespace rapidfuzz */
