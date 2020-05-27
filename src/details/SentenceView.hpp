
#pragma once
#include "SplittedSentenceView.hpp"

namespace rapidfuzz {

template <typename CharT> class SentenceView {
public:
  SentenceView(basic_string_view<CharT> sentence) : m_sentence(std::move(sentence))
  {}

  SplittedSentenceView<CharT> sorted_split() const
  {
    auto result = split();
    result.sort();
    return result;
  }

  SplittedSentenceView<CharT> split() const
  {
    string_view_vec<CharT> splitted;
    auto first = m_sentence.data();
    auto second = m_sentence.data();
    auto last = first + m_sentence.size();

    for (; second != last && first != last; first = second + 1) {
      second = std::find_if(first, last, [](const CharT& ch) {
        // TODO: add comparisions for other whitespace chars like tab
        return ch == ' ';
      });

      if (first != second) {
        splitted.emplace_back(first, second - first);
      }
    }

    return SplittedSentenceView<CharT>(splitted);
  }

  std::size_t size() const
  {
    return m_sentence.size();
  }

  std::size_t length() const
  {
    return size();
  }

  bool empty() const
  {
    return m_sentence.empty();
  }

  typename basic_string_view<CharT>::const_iterator begin() const
  {
    return m_sentence.begin();
  }

  typename basic_string_view<CharT>::const_iterator end() const
  {
    return m_sentence.end();
  }

private:
  basic_string_view<CharT> m_sentence;
};

} // namespace rapidfuzz