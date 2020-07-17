#pragma once
#include "types.hpp"
#include <string>
#include <limits>

namespace rapidfuzz {

template <typename CharT> class SplittedSentenceViewConstIter;

template <typename CharT>
class SplittedSentenceView {
public:
    friend class SplittedSentenceViewConstIter<CharT>;
    typedef SplittedSentenceViewConstIter<CharT> const_iterator;

  class const_reverse_iterator {
  public:
    explicit const_reverse_iterator(typename string_view_vec<CharT>::const_reverse_iterator iter) : words_iter(iter)
    {}

    const_reverse_iterator operator++();
    bool operator!=(const const_reverse_iterator& other) const;
    bool operator==(const const_reverse_iterator& other) const;
    const CharT& operator*() const;

  private:
    CharT whitespace{0x20};
    bool add_whitespace = false;
    std::size_t word_pos = 1;
    typename string_view_vec<CharT>::const_reverse_iterator words_iter;
  };

public:
  SplittedSentenceView(string_view_vec<CharT> sentence) : m_sentence(std::move(sentence))
  {}

  std::size_t dedupe();

  // calculates size on first run O(N) with N being the word count
  std::size_t size();

  std::size_t length()
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

  const_reverse_iterator rbegin() const
  {
    return crbegin();
  }
  const_reverse_iterator rend() const
  {
    return crend();
  }

  const_reverse_iterator crbegin() const
  {
    return const_reverse_iterator(m_sentence.rbegin());
  }
  const_reverse_iterator crend() const
  {
    return const_reverse_iterator(m_sentence.rend());
  }

  string_view_vec<CharT> words() const
  {
    return m_sentence;
  }

  void remove_prefix(std::size_t n);
  void remove_suffix(std::size_t n);

private:
  string_view_vec<CharT> m_sentence;
  std::size_t m_size = std::numeric_limits<std::size_t>::max();
};

template <typename CharT>
struct SplittedSentenceViewConstIter {
  explicit SplittedSentenceViewConstIter(typename string_view_vec<CharT>::const_iterator iter)
    : words_iter(iter)
  {}

  SplittedSentenceViewConstIter<CharT> operator++();
  bool operator!=(const SplittedSentenceViewConstIter<CharT>& other) const;
  bool operator==(const SplittedSentenceViewConstIter<CharT>& other) const;
  const CharT& operator*();

private:
  CharT whitespace{0x20};
  bool add_whitespace = false;
  std::size_t word_pos = 1;

  typename basic_string_view<CharT>::const_iterator letter_iter;
  typename string_view_vec<CharT>::const_iterator words_iter;
};


} // namespace rapidfuzz

#include "SplittedSentenceView.txx"

namespace std {
    template<typename CharT>
    class iterator_traits<rapidfuzz::SplittedSentenceViewConstIter<CharT>>
    {
    public:
        //using difference_type = std::ptrdiff_t;
        using size_type = std::size_t;
        using value_type = CharT;
        using pointer = CharT*;
        //using reference = CharT&;
        //using iterator_category = std::random_access_iterator_tag;
    };
}