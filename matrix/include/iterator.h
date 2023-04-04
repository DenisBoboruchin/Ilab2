#ifndef ITERATOR_H

namespace matrix_space{

template <typename T>
class list<T>::iterator final {
public:
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = int;
    using value_type = T;
    using reference = T &;
    using pointer = T *;

    explicit iterator(list_node_t *node_ptr = nullptr);

    iterator &operator++();
    iterator &operator++(int);
    iterator &operator--();
    iterator &operator--(int);

    reference operator*() const;
    pointer operator->() const;

    auto operator<=>(const iterator &other) const = default;

    list_node_t *get_ptr_ ();

private:
    list_node_t *node_itr_;
};

template <typename T>
list<T>::iterator::iterator(list_node_t *node_ptr) : node_itr_ {node_ptr}
{
}

template <typename T>
typename list<T>::iterator &list<T>::iterator::operator++()
{
    node_itr_ = node_itr_->next_;

    return *this;
}

template <typename T>
typename list<T>::iterator &list<T>::iterator::operator++(int)
{
    auto temp {*this};
    operator++();

    return temp;
}

template <typename T>
typename list<T>::iterator &list<T>::iterator::operator--()
{
    node_itr_ = node_itr_->prev_;

    return *this;
}

template <typename T>
typename list<T>::iterator &list<T>::iterator::operator--(int)
{
    auto temp {*this};
    operator--();

    return temp;
}

template <typename T>
typename list<T>::iterator::reference list<T>::iterator::operator*() const
{
    return node_itr_->data_;
}

template <typename T>
typename list<T>::iterator::pointer list<T>::iterator::operator->() const
{
    return &(node_itr_->data_);
}

}

#endif
