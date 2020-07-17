
#pragma once
#include "SplittedSentenceView.hpp"
#include "unicode.hpp"

namespace rapidfuzz {

template <typename CharT>
class SentenceView {
public:
  //TODO add constructor for different string views
  SentenceView(basic_string_view<CharT> sentence) : m_sentence(std::move(sentence))
  {}

  SplittedSentenceView<CharT> sorted_split() const;

  SplittedSentenceView<CharT> split() const;

  /*
   * returns the number of characters
   */
  std::size_t size() const noexcept;
  std::size_t length() const noexcept;

  /*
   * checks whether the SentenceView is empty
   */
  bool empty() const noexcept;

  typename basic_string_view<CharT>::const_iterator begin() const;

  typename basic_string_view<CharT>::const_iterator end() const;

  void remove_prefix(std::size_t n) {
    m_sentence.remove_prefix(n);
  }

  void remove_suffix(std::size_t n) {
    m_sentence.remove_suffix(n);
  }

private:
  basic_string_view<CharT> m_sentence;
};

} // namespace rapidfuzz

#include "SentenceView.txx"