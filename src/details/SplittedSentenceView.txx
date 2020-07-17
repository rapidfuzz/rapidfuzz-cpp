#include "SplittedSentenceView.hpp"
#include <iostream>

namespace rapidfuzz {

template <typename CharT>
void SplittedSentenceView<CharT>::remove_prefix(std::size_t n) {
  std::size_t word_cut = 0;
  if (m_size != std::numeric_limits<std::size_t>::max()) {
    m_size -= n;
  }

  for (auto& word : m_sentence) {
    if (n > word.size()) {
      ++word_cut;
      n = n - word.size() - 1;
    } else {
      word.remove_prefix(n);
      break;
    }
  }
  m_sentence.erase(m_sentence.begin(), m_sentence.begin() + word_cut);

  if (word_count() == 1 && m_sentence[0].size() == 0) {
	m_sentence.resize(0);
  }
}

template <typename CharT>
void SplittedSentenceView<CharT>::remove_suffix(std::size_t n) {
  std::size_t word_cut = 0;
  if (m_size != std::numeric_limits<std::size_t>::max()) {
    m_size -= n;
  }

  for (auto it = m_sentence.rbegin(); it != m_sentence.rend(); ++it) {
    if (n > it->size()) {
      ++word_cut;
      n = n - it->size() - 1;
    } else {
      it->remove_suffix(n);
      break;
    }
  }

  m_sentence.resize(word_count() - word_cut);

  if (word_count() == 1 && m_sentence[0].size() == 0) {
	m_sentence.resize(0);
  }
}

template <typename CharT>
SplittedSentenceViewConstIter<CharT> SplittedSentenceViewConstIter<CharT>::operator++()
{
  if (add_whitespace == true) {
    add_whitespace = false;
  }
  // occurs when the string is empty. Whitespace was already inserted by operator*
  else if (word_pos == words_iter->size()) {
    ++words_iter;
    word_pos = 1;
  } else if (word_pos < words_iter->size()) {
    ++word_pos;
  } else {
    ++words_iter;
    word_pos = 1;
    add_whitespace = true;
  }
  return *this;
}

template <typename CharT>
bool SplittedSentenceViewConstIter<CharT>::operator!=(const SplittedSentenceViewConstIter<CharT>& other) const
{
  return words_iter != other.words_iter;
}

template <typename CharT>
bool SplittedSentenceViewConstIter<CharT>::operator==(const SplittedSentenceViewConstIter<CharT>& other) const
{
  return words_iter == other.words_iter;
}

template <typename CharT>
const CharT& SplittedSentenceViewConstIter<CharT>::operator*()
{
  if (add_whitespace == true) {
    return whitespace;
  }
  if (word_pos == words_iter->size()) {
	  return whitespace;
  }

  return (*words_iter)[word_pos-1];
}

template <typename CharT>
typename SplittedSentenceView<CharT>::const_reverse_iterator
SplittedSentenceView<CharT>::const_reverse_iterator::operator++()
{
  if (add_whitespace == true) {
    add_whitespace = false;
  }
  // occurs when the string is empty. Whitespace was already inserted by operator*
  else if (word_pos == words_iter->size()) {
    ++words_iter;
    word_pos = 1;
  } else if (word_pos < words_iter->size()) {
    ++word_pos;
  } else {
    ++words_iter;
    word_pos = 1;
    add_whitespace = true;
  }
  return *this;
}

template <typename CharT>
bool SplittedSentenceView<CharT>::const_reverse_iterator::operator!=(const const_reverse_iterator& other) const
{
  return words_iter != other.words_iter;
}

template <typename CharT>
bool SplittedSentenceView<CharT>::const_reverse_iterator::operator==(const const_reverse_iterator& other) const
{
  return words_iter == other.words_iter;
}

template <typename CharT>
const CharT& SplittedSentenceView<CharT>::const_reverse_iterator::operator*() const
{
  if (add_whitespace == true) {
    return whitespace;
  }
  if (word_pos == words_iter->size()) {
	  return whitespace;
  }

  return (*words_iter)[words_iter->size()-word_pos-1];
}

template <typename CharT>
std::size_t SplittedSentenceView<CharT>::dedupe()
{
  std::size_t old_word_count = word_count();
  m_sentence.erase(std::unique(m_sentence.begin(), m_sentence.end()), m_sentence.end());
  return old_word_count - word_count();
}

template <typename CharT>
std::size_t SplittedSentenceView<CharT>::size()
{
  if (m_sentence.empty()) {
    m_size = 0;
  } else if (m_size == std::numeric_limits<std::size_t>::max()) {
    // there is a whitespace between each word
    m_size = m_sentence.size() - 1;
    for (const auto& word : m_sentence) {
      m_size += word.size();
    }
  }

  return m_size;
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