#include "matching_blocks.hpp"

#include "utils.hpp"
#include <cassert>
#include <numeric>

namespace rapidfuzz {
namespace details {

template <typename CharT1, typename CharT2>
std::vector<EditOp> edit_ops_from_cost_matrix(basic_string_view<CharT1> s1,
                                              basic_string_view<CharT2> s2, std::size_t prefix_len,
                                              std::vector<std::size_t> matrix)
{
  int8_t dir = 0;

  std::size_t rows = s1.size() + 1;
  std::size_t columns = s2.size() + 1;

  std::size_t current_pos = rows * columns - 1;
  std::size_t row = rows - 1;
  std::size_t column = columns - 1;

  std::vector<EditOp> ops;
  ops.reserve(matrix[current_pos]);

  auto store_editop = [&ops, prefix_len](EditType type, std::size_t r, std::size_t c) {
    ops.emplace_back(type, r + prefix_len, c + prefix_len);
  };

  while (row > 0 || column > 0) {
    std::size_t left_pos = current_pos - 1;
    std::size_t above_pos = current_pos - columns;
    std::size_t diagonal_pos = current_pos - columns - 1;

    // no edit required -> does not has to be stored in the editops
    if (row != 0 && column != 0 && matrix[current_pos] == matrix[diagonal_pos] &&
        s1[row - 1] == s2[column - 1])
    {
      --row;
      --column;
      current_pos = diagonal_pos;
      dir = 0;
    }
    else if (dir < 0 && column != 0 && matrix[current_pos] == matrix[left_pos] + 1) {
      --column;
      current_pos = left_pos;
      store_editop(EditType::INSERT, row, column);
    }
    else if (dir > 0 && row != 0 && matrix[current_pos] == matrix[above_pos] + 1) {
      --row;
      current_pos = above_pos;
      store_editop(EditType::DELETE, row, column);
    }
    else if (row != 0 && column != 0 && matrix[current_pos] == matrix[diagonal_pos] + 1) {
      --row;
      --column;
      current_pos = diagonal_pos;
      dir = 0;
      store_editop(EditType::REPLACE, row, column);
    }
    else if (dir == 0 && column != 0 && matrix[current_pos] == matrix[left_pos] + 1) {
      --column;
      current_pos = left_pos;
      dir = -1;
      store_editop(EditType::INSERT, row, column);
    }
    else if (dir == 0 && row != 0 && matrix[current_pos] == matrix[above_pos] + 1) {
      --row;
      current_pos = above_pos;
      dir = 1;
      store_editop(EditType::DELETE, row, column);
    }
    else {
      assert(("Something went wrong when extracting the editops from the cost matrix", false));
    }
  }

  // ops are added in reversed order, so the order has to be corrected
  std::reverse(ops.begin(), ops.end());
  return ops;
}
template <typename CharT1, typename CharT2>
std::vector<EditOp> get_edit_ops(basic_string_view<CharT1> s1, basic_string_view<CharT2> s2)
{
  auto affix = utils::remove_common_affix(s1, s2);

  std::size_t rows = s1.size() + 1;
  std::size_t columns = s2.size() + 1;

  std::vector<std::size_t> matrix(rows * columns);
  std::iota(matrix.begin(), matrix.begin() + columns, 0);
  for (std::size_t i = 1; i < rows; ++i) {
    matrix[columns * i] = i;
  }

  std::size_t sentence1_pos = 1;

  for (const auto& char1 : s1) {
    std::size_t prev_pos = (sentence1_pos - 1) * columns;
    std::size_t current_pos = sentence1_pos * columns + 1;
    std::size_t result = sentence1_pos;

    for (const auto& char2 : s2) {
      result = std::min({matrix[prev_pos++] + (char1 != char2), result + 1, matrix[prev_pos] + 1});

      matrix[current_pos] = result;
      ++current_pos;
    }
    ++sentence1_pos;
  }

  return edit_ops_from_cost_matrix(s1, s2, affix.prefix_len, matrix);
}
} // namespace details

template <typename Sentence1, typename Sentence2>
std::vector<MatchingBlock> get_matching_blocks(const Sentence1& sentence1,
                                               const Sentence2& sentence2)
{
  auto s1 = utils::to_string_view(sentence1);
  auto s2 = utils::to_string_view(sentence2);

  auto ops = details::get_edit_ops(s1, s2);

  std::vector<MatchingBlock> mblocks;

  std::size_t spos = 0;
  std::size_t dpos = 0;

  for (const auto& op : ops) {
    if (spos < op.spos || dpos < op.dpos) {
      mblocks.emplace_back(spos, dpos, op.spos - spos);
      spos = op.spos;
      dpos = op.dpos;
    }

    switch (op.type) {
    case EditType::REPLACE:
      ++spos;
      ++dpos;
      break;

    case EditType::DELETE:
      ++spos;
      break;

    case EditType::INSERT:
      ++dpos;
      break;

    default:
      break;
    }
  }

  if (spos < s1.size() || dpos < s2.size()) {
    assert(s1.size() - spos == s2.size() - dpos);
    mblocks.emplace_back(spos, dpos, s1.size() - spos);
  }

  mblocks.emplace_back(s1.size(), s2.size(), 0);

  return mblocks;
}

} // namespace rapidfuzz
