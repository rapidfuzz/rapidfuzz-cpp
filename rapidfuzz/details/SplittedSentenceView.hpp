#pragma once
#include <algorithm>
#include <rapidfuzz/details/type_traits.hpp>
#include <string>

namespace rapidfuzz {

template <typename InputIt>
class SplittedSentenceView {
public:
    using CharT = iterator_type<InputIt>;

    SplittedSentenceView(IteratorViewVec<InputIt> sentence) : m_sentence(std::move(sentence))
    {}

    int64_t dedupe();
    int64_t size() const;

    int64_t length() const
    {
        return size();
    }

    bool empty() const
    {
        return m_sentence.empty();
    }

    int64_t word_count() const
    {
        return m_sentence.size();
    }

    std::basic_string<CharT> join() const;

    const IteratorViewVec<InputIt>& words() const
    {
        return m_sentence;
    }

private:
    IteratorViewVec<InputIt> m_sentence;
};

template <typename InputIt>
int64_t SplittedSentenceView<InputIt>::dedupe()
{
    int64_t old_word_count = word_count();
    m_sentence.erase(std::unique(m_sentence.begin(), m_sentence.end()), m_sentence.end());
    return old_word_count - word_count();
}

template <typename InputIt>
int64_t SplittedSentenceView<InputIt>::size() const
{
    if (m_sentence.empty()) return 0;

    // there is a whitespace between each word
    int64_t result = m_sentence.size() - 1;
    for (const auto& word : m_sentence) {
        result += std::distance(word.first, word.last);
    }

    return result;
}

template <typename InputIt>
auto SplittedSentenceView<InputIt>::join() const -> std::basic_string<CharT>
{
    if (m_sentence.empty()) {
        return std::basic_string<CharT>();
    }

    auto sentence_iter = m_sentence.begin();
    std::basic_string<CharT> joined(sentence_iter->first, sentence_iter->last);
    const std::basic_string<CharT> whitespace{0x20};
    ++sentence_iter;
    for (; sentence_iter != m_sentence.end(); ++sentence_iter) {
        joined.append(whitespace)
            .append(std::basic_string<CharT>(sentence_iter->first, sentence_iter->last));
    }
    return joined;
}

} // namespace rapidfuzz