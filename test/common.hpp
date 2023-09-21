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

    const auto& operator*() const
    {
        return *iter;
    }

private:
    T iter;
};
