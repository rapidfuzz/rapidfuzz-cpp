
#include "SentenceView.hpp"

namespace rapidfuzz {

template <typename CharT>
SplittedSentenceView<CharT> SentenceView<CharT>::sorted_split() const
{
  auto result = split();
  result.sort();
  return result;
}

template <typename CharT>
SplittedSentenceView<CharT> SentenceView<CharT>::split() const
{
  string_view_vec<CharT> splitted;
  auto first = m_sentence.data();
  auto second = m_sentence.data();
  auto last = first + m_sentence.size();

  for (; second != last && first != last; first = second + 1) {
    second = std::find_if(first, last, Unicode::is_space<CharT>);

    if (first != second) {
      splitted.emplace_back(first, second - first);
    }
  }

  return SplittedSentenceView<CharT>(splitted);
}

template <typename CharT>
std::size_t SentenceView<CharT>::size() const noexcept
{
  return m_sentence.size();
}

template <typename CharT>
std::size_t SentenceView<CharT>::length() const noexcept
{
  return size();
}

template <typename CharT>
bool SentenceView<CharT>::empty() const noexcept
{
  return m_sentence.empty();
}

template <typename CharT>
typename basic_string_view<CharT>::const_iterator SentenceView<CharT>::begin() const
{
  return m_sentence.begin();
}

template <typename CharT>
typename basic_string_view<CharT>::const_iterator SentenceView<CharT>::end() const
{
  return m_sentence.end();
}

} // namespace rapidfuzz