#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <iostream>

#include "vector.h"

namespace matrix_space {

template <typename T>
class matrix final {
private:
    using row_t = typename my_containers::vector<T>;
    struct proxy_row_t final {
        T &operator[](size_t num_col);

        row_t &row;
    };

    struct const_proxy_row_t final {
        const T &operator[](size_t num_col) const;

        const row_t &row;
    };

public:

    matrix(const size_t num_rows = 0, const size_t num_cols = 0, const T &value = {});


    size_t get_num_rows() const;
    size_t get_num_cols() const;

    proxy_row_t operator[](size_t num_row) &;
    const const_proxy_row_t operator[](size_t num_row) const &;

    void dump() const;

private:
    my_containers::vector<row_t> data_;

    size_t num_rows_;
    size_t num_cols_;
};

template <typename T>
matrix<T>::matrix(const size_t num_rows, const size_t num_cols, const T &value)
    : data_ {num_rows, row_t {num_cols, value}}, num_rows_ {num_rows}, num_cols_ {num_cols}
{
}

template <typename T>
T &matrix<T>::proxy_row_t::operator[](size_t num_col)
{
    return row[num_col];
}

template <typename T>
const T &matrix<T>::const_proxy_row_t::operator[](size_t num_col) const
{
    return row[num_col];
}

template <typename T>
typename matrix<T>::proxy_row_t matrix<T>::operator[](size_t num_row) &
{
    row_t &row = data_[num_row];
    proxy_row_t proxy_row {row};
    return proxy_row;
}

template <typename T>
const typename matrix<T>::const_proxy_row_t matrix<T>::operator[](size_t num_row) const &
{
    const row_t &row = data_[num_row];
    const_proxy_row_t proxy_row {row};
    return proxy_row;
}

template <typename T>
void matrix<T>::dump() const
{
    for (int index_row = 0; index_row != num_rows_; ++index_row) {
        for (int index_col = 0; index_col != num_cols_; ++index_col) {
            std::cout << (data_.at(index_row)).at(index_col) << ' ';
        }

        std::cout << std::endl;
    }
}

template <typename T>
size_t matrix<T>::get_num_rows() const
{
    return num_rows_;
}

template <typename T>
size_t matrix<T>::get_num_cols() const
{
    return num_cols_;
}

}  // namespace matrix_space

#endif
