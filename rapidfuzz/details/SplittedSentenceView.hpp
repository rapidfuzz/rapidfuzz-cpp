#pragma once
#include "types.hpp"
#include <string>
#include <algorithm>

namespace rapidfuzz {

template <typename CharT>
class SplittedSentenceView {
public:
  class const_iterator {
  public:
    explicit const_iterator(typename string_view_vec<CharT>::const_iterator iter) : words_iter(iter)
    {}

    const_iterator operator++();
    bool operator!=(const const_iterator& other) const;
    const CharT& operator*() const;

  private:
    CharT whitespace{0x20};
    bool add_whitespace = false;
    std::size_t word_pos = 0;
    typename string_view_vec<CharT>::const_iterator words_iter;
  };

public:
  SplittedSentenceView(string_view_vec<CharT> sentence) : m_sentence(std::move(sentence))
  {}

  std::size_t dedupe();
  std::size_t size() const;

  std::size_t length() const
  {
    return size();
  }

  bool empty() const
  {
    return m_sentence.empty();
  }

  std::size_t word_count() const
  {
    return m_sentence.size();
  }

  std::basic_string<CharT> join() const;

  void sort()
  {
    std::sort(m_sentence.begin(), m_sentence.end());
  }

  const_iterator begin() const
  {
    return cbegin();
  }
  const_iterator end() const
  {
    return cend();
  }

  const_iterator cbegin() const
  {
    return const_iterator(m_sentence.begin());
  }
  const_iterator cend() const
  {
    return const_iterator(m_sentence.end());
  }

  string_view_vec<CharT> words() const
  {
    return m_sentence;
  }

private:
  string_view_vec<CharT> m_sentence;
};

template <typename CharT>
typename SplittedSentenceView<CharT>::const_iterator
SplittedSentenceView<CharT>::const_iterator::operator++()
{
  if (word_pos == 0 && add_whitespace == true) {
    add_whitespace = false;
  }
  else if (word_pos < words_iter->size()) {
    ++word_pos;
  }
  else {
    ++words_iter;
    word_pos = 0;
    add_whitespace = true;
  }
  return *this;
}

template <typename CharT>
bool SplittedSentenceView<CharT>::const_iterator::operator!=(const const_iterator& other) const
{
  return words_iter != other.words_iter;
}

template <typename CharT>
const CharT& SplittedSentenceView<CharT>::const_iterator::operator*() const
{
  if (add_whitespace == true) {
    return whitespace;
  }
  return (*words_iter)[word_pos];
}

template <typename CharT>
std::size_t SplittedSentenceView<CharT>::dedupe()
{
  std::size_t old_word_count = word_count();
  m_sentence.erase(std::unique(m_sentence.begin(), m_sentence.end()), m_sentence.end());
  return old_word_count - word_count();
}

template <typename CharT>
std::size_t SplittedSentenceView<CharT>::size() const
{
  if (m_sentence.empty()) return 0;

  // there is a whitespace between each word
  std::size_t result = m_sentence.size() - 1;
  for (const auto& word : m_sentence) {
    result += word.size();
  }

  return result;
}

template <typename CharT>
std::basic_string<CharT> SplittedSentenceView<CharT>::join() const
{
  if (m_sentence.empty()) {
    return std::basic_string<CharT>();
  }

  auto sentence_iter = m_sentence.begin();
  std::basic_string<CharT> joined{*sentence_iter};
  const std::basic_string<CharT> whitespace{0x20};
  ++sentence_iter;
  for (; sentence_iter != m_sentence.end(); ++sentence_iter) {
    joined.append(whitespace).append(std::basic_string<CharT>{*sentence_iter});
  }
  return joined;
}

} // namespace rapidfuzz