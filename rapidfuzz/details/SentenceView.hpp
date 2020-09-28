
#pragma once
#include "SplittedSentenceView.hpp"
#include "type_traits.hpp"
#include "unicode.hpp"

namespace rapidfuzz {

template <typename CharT>
class SentenceView {
public:
  template <
      typename Sentence,
      typename = enable_if_t<is_explicitly_convertible<Sentence, basic_string_view<CharT>>::value>>
  SentenceView(const Sentence& str) : m_sentence(basic_string_view<CharT>(str))
  {}

  template <typename Sentence,
            typename =
                enable_if_t<!is_explicitly_convertible<Sentence, basic_string_view<CharT>>::value &&
                            has_data_and_size<Sentence>::value>>
  SentenceView(Sentence str) : m_sentence(basic_string_view<CharT>(str.data(), str.size()))
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

private:
  basic_string_view<CharT> m_sentence;
};

} // namespace rapidfuzz

#include "SentenceView.txx"