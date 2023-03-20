#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <iostream>

#include "vector.h"

namespace matrix_space {

template <typename T>
class matrix final {
public:
    matrix(int num_rows = 0, int num_cols = 0, const T &value = {});

    int get_num_rows() const;
    int get_num_cols() const;

private:
    my_containers::vector<T> data_;

    int num_rows_;
    int num_cols_;
};

template <typename T>
matrix<T>::matrix(int num_rows, int num_cols, const T &value)
    : data_ {num_rows * num_cols, value}, num_rows_ {num_rows}, num_cols_ {num_cols}
{
}

template <typename T>
int matrix<T>::get_num_rows() const
{
    return num_rows_;
}

template <typename T>
int matrix<T>::get_num_cols() const
{
    return num_cols_;
}

}  // namespace matrix_space

#endif
