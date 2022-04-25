#pragma once
#include <algorithm>
#include <rapidfuzz/details/type_traits.hpp>
#include <string>

namespace rapidfuzz {

template <typename InputIt>
class SplittedSentenceView {
public:
    using CharT = iter_value_t<InputIt>;

    SplittedSentenceView(IteratorViewVec<InputIt> sentence)
        noexcept(std::is_nothrow_move_constructible<IteratorViewVec<InputIt>>::value) : m_sentence(std::move(sentence))
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

    const IteratorViewVec<InputIt>& words() const
    {
        return m_sentence;
    }

private:
    IteratorViewVec<InputIt> m_sentence;
};

template <typename InputIt>
std::size_t SplittedSentenceView<InputIt>::dedupe()
{
    std::size_t old_word_count = word_count();
    m_sentence.erase(std::unique(m_sentence.begin(), m_sentence.end()), m_sentence.end());
    return old_word_count - word_count();
}

template <typename InputIt>
std::size_t SplittedSentenceView<InputIt>::size() const
{
    if (m_sentence.empty()) return 0;

    // there is a whitespace between each word
    std::size_t result = m_sentence.size() - 1;
    for (const auto& word : m_sentence) {
        result += static_cast<std::size_t>(std::distance(word.first, word.last));
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