#pragma once

template <typename T>
class BidirectionalIterWrapper {
public:
    using difference_type = typename T::difference_type;
    using value_type = typename T::value_type;
    using pointer = typename T::pointer;
    using reference = typename T::reference;
    using iterator_category = std::bidirectional_iterator_tag;

    BidirectionalIterWrapper() : iter()
    {}

    BidirectionalIterWrapper(T iter_) : iter(iter_)
    {}

    bool operator==(const BidirectionalIterWrapper& i) const
    {
        return iter == i.iter;
    }

    bool operator!=(const BidirectionalIterWrapper& i) const
    {
        return !(*this == i);
    }

    BidirectionalIterWrapper operator++(int)
    {
        BidirectionalIterWrapper cur(iter);
        ++iter;
        return cur;
    }
    BidirectionalIterWrapper operator--(int)
    {
        BidirectionalIterWrapper cur(iter);
        --iter;
        return cur;
    }

    BidirectionalIterWrapper& operator++()
    {
        ++iter;
        return *this;
    }
    BidirectionalIterWrapper& operator--()
    {
        --iter;
        return *this;
    }

    const value_type& operator*() const
    {
        return *iter;
    }

private:
    T iter;
};

template <typename Iter>
constexpr auto make_bidir(Iter iter) -> BidirectionalIterWrapper<Iter>
{
    return BidirectionalIterWrapper<Iter>(iter);
}

template <typename T, typename = rapidfuzz::rf_enable_if_t<std::is_same<T, char>::value>>
std::basic_string<T> str_multiply(std::basic_string<T> a, size_t b)
{
    std::basic_string<T> output;
    while (b--)
        output += a;

    return output;
}
