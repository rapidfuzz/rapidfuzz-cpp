#pragma once

#include <vector>

namespace rapidfuzz {
enum class EditType { DELETE, INSERT, REPLACE };

struct EditOp {
  EditType type;
  std::size_t spos;
  std::size_t dpos;

  EditOp(EditType type, std::size_t spos, std::size_t dpos) : type(type), spos(spos), dpos(dpos)
  {}
};

struct MatchingBlock {
  std::size_t spos;
  std::size_t dpos;
  std::size_t length;

  MatchingBlock(std::size_t spos, std::size_t dpos, std::size_t length)
      : spos(spos), dpos(dpos), length(length)
  {}
};

template <typename Sentence1, typename Sentence2>
std::vector<MatchingBlock> get_matching_blocks(const Sentence1& sentence1,
                                               const Sentence2& sentence2);

} // namespace rapidfuzz

#include "matching_blocks.txx"
