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
    explicit matrix(const size_t num_rows = 0, const size_t num_cols = 0, const T &value = {});

    static matrix eye(const size_t num_rows, const size_t num_cols);
    static matrix square(const size_t num_rows, const T &value = {});

    proxy_row_t operator[](size_t num_row) &;
    const const_proxy_row_t operator[](size_t num_row) const &;

    matrix operator+(const matrix &other) const;
    matrix &operator+=(const matrix &other);

    matrix operator-(const matrix &other) const;
    matrix &operator-=(const matrix &other);

    matrix &operator*=(const matrix &other);
    matrix &operator*=(const T &value);

    bool operator!=(const matrix &other) const;
    bool operator==(const matrix &other) const;

    matrix &transpose();

    T determinant() const;

    void swap_rows(const size_t num_row_1, const size_t num_row_2);
    void add_row(const size_t num_row_1, const size_t num_row_2);

    size_t get_num_rows() const;
    size_t get_num_cols() const;

    bool is_square() const;

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
matrix<T> matrix<T>::eye(const size_t num_rows, const size_t num_cols)
{
    matrix eye {num_rows, num_cols};
    int min = std::min(num_rows, num_cols);

    for (int index = 0; index != min; ++index) {
        eye[index][index] = T {1};
    }

    return eye;
}

template <typename T>
matrix<T> matrix<T>::square(const size_t num_rows, const T &value)
{
    return matrix<T> {num_rows, num_rows, value};
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
    return {data_[num_row]};
}

template <typename T>
const typename matrix<T>::const_proxy_row_t matrix<T>::operator[](size_t num_row) const &
{
    return {data_[num_row]};
}

template <typename T>
matrix<T> matrix<T>::operator+(const matrix &other) const
{
    if (num_rows_ != other.get_num_rows() || num_cols_ != other.get_num_cols()) {
        std::cout << "ahaha 123" << std::endl;
        return matrix<T> {};
    }

    matrix<T> sum {*this};

    for (int index_row = 0; index_row != num_rows_; ++index_row) {
        row_t &sum_row = sum[index_row].row;
        for (int index_col = 0; index_col != num_cols_; ++index_col) {
            sum_row[index_col] += other[index_row][index_col];
        }
    }

    return sum;
}

template <typename T>
matrix<T> &matrix<T>::operator+=(const matrix &other)
{
    if (num_rows_ != other.get_num_rows() || num_cols_ != other.get_num_cols()) {
        std::cout << "ahaha 123" << std::endl;
        return *this;
    }

    for (int index_row = 0; index_row != num_rows_; ++index_row) {
        row_t &row = data_[index_row];
        for (int index_col = 0; index_col != num_cols_; ++index_col) {
            row[index_col] += other[index_row][index_col];
        }
    }

    return *this;
}

template <typename T>
matrix<T> matrix<T>::operator-(const matrix &other) const
{
    if (num_rows_ != other.get_num_rows() || num_cols_ != other.get_num_cols()) {
        std::cout << "ahaha 123" << std::endl;
        return {};
    }

    matrix<T> sub {*this};

    for (int index_row = 0; index_row != num_rows_; ++index_row) {
        row_t &sub_row = sub[index_row].row;
        for (int index_col = 0; index_col != num_cols_; ++index_col) {
            sub_row[index_col] -= other[index_row][index_col];
        }
    }

    return sub;
}

template <typename T>
matrix<T> &matrix<T>::operator-=(const matrix &other)
{
    if (num_rows_ != other.get_num_rows() || num_cols_ != other.get_num_cols()) {
        std::cout << "ahaha 123" << std::endl;
        return *this;
    }

    for (int index_row = 0; index_row != num_rows_; ++index_row) {
        row_t &row = data_[index_row];
        for (int index_col = 0; index_col != num_cols_; ++index_col) {
            row[index_col] -= other[index_row][index_col];
        }
    }

    return *this;
}

template <typename T>
matrix<T> operator*(const matrix<T> &matrix_1, const matrix<T> &matrix_2)
{
    size_t num_rows = matrix_1.get_num_rows();
    size_t num_cols = matrix_1.get_num_cols();
    if (num_rows != matrix_2.get_num_cols() || num_cols != matrix_2.get_num_rows()) {
        std::cout << "ahaha 123" << std::endl;
        return matrix<T> {};
    }

    matrix<T> muled {num_rows, num_rows};
    matrix<T> matrix_2_transposed = matrix_2;
    matrix_2_transposed.transpose();

    for (int index = 0; index != num_rows; ++index) {
        // const row_t &data_row = matrix_1[index].row;
        for (int index_row = 0; index_row != num_rows; ++index_row) {
            T val = T {0};

            // row_t &trns_row = matrix_2_transposed[index_row].row;
            for (int index_col = 0; index_col != num_cols; ++index_col) {
                val += matrix_1[index][index_col] * matrix_2_transposed[index_row][index_col];
            }

            muled[index][index_row] = val;
        }
    }

    return muled;
}

template <typename T>
matrix<T> operator*(const matrix<T> &matrix_mul, const T &value)
{
    matrix<T> muled {matrix_mul};
    for (int index_row = 0, num_rows = muled.get_num_rows(); index_row != num_rows; ++index_row) {
        for (int index_col = 0, num_cols = muled.get_num_cols(); index_col != num_cols; ++index_col) {
            muled[index_row][index_col] *= value;
        }
    }

    return muled;
}

template <typename T>
matrix<T> operator*(const T &value, const matrix<T> &matrix_mul)
{
    matrix<T> muled {matrix_mul};
    for (int index_row = 0, num_rows = muled.get_num_rows(); index_row != num_rows; ++index_row) {
        for (int index_col = 0, num_cols = muled.get_num_cols(); index_col != num_cols; ++index_col) {
            muled[index_row][index_col] *= value;
        }
    }

    return muled;
}

template <typename T>
matrix<T> &matrix<T>::operator*=(const matrix &other)
{
    if (num_rows_ != other.get_num_cols() || num_cols_ != other.get_num_rows()) {
        std::cout << "ahaha 123" << std::endl;
        return *this;
    }

    matrix<T> muled = *this * other;
    std::swap(*this, muled);

    return *this;
}

template <typename T>
matrix<T> &matrix<T>::operator*=(const T &value)
{
    matrix<T> muled = *this * value;
    std::swap(*this, muled);

    return *this;
}

template <typename T>
bool matrix<T>::operator!=(const matrix &other) const
{
    if (num_rows_ != other.get_num_rows() || num_cols_ != other.get_num_cols()) {
        std::cout << "ahaha 123" << std::endl;
        return 0;
    }

    for (int index_row = 0; index_row != num_rows_; ++index_row) {
        const row_t &row = data_[index_row];
        const row_t &othr_row = other[index_row].row;
        for (int index_col = 0; index_col != num_cols_; ++index_col) {
            if (row[index_col] != othr_row[index_col]) {
                return true;
            }
        }
    }

    return false;
}

template <typename T>
bool matrix<T>::operator==(const matrix &other) const
{
    return !(operator!=(other));
}

template <typename T>
T matrix<T>::determinant() const
{
    if (!is_square()) {
        std::cout << "ahaha 123" << std::endl;
        return {};
    }

    matrix lower = eye(num_rows_, num_cols_);
    matrix upper_transposed = square(num_rows_);

    T determinant = T {1};
    for (int index_row = 0; index_row != num_rows_; ++index_row) {
        const row_t &data_row = data_[index_row];
        row_t &lower_row = lower[index_row].row;
        for (int index_col = 0; index_col != num_rows_; ++index_col) {
            row_t &upper_row = upper_transposed[index_col].row;
            T temp {};
            if (index_col >= index_row) {
                for (int index = 0; index != index_row; ++index) {
                    temp += lower_row[index] * upper_row[index];
                }

                T elem = data_row[index_col] - temp;
                upper_row[index_row] = elem;
                if (index_row == index_col) {
                    determinant *= elem;
                }

            } else {
                for (int index = 0; index != index_col; ++index) {
                    temp += lower_row[index] * upper_row[index];
                }

                T elem = upper_row[index_col];
                if (elem == T {0}) {
                    return T {0};
                }
                lower_row[index_col] = (data_row[index_col] - temp) / elem;
            }
        }
    }

    return determinant;
}

template <typename T>
matrix<T> &matrix<T>::transpose()
{
    matrix<T> transposed_matrix {num_cols_, num_rows_};

    for (int index_row = 0; index_row != num_cols_; ++index_row) {
        row_t &row = transposed_matrix[index_row].row;
        for (int index_col = 0; index_col != num_rows_; ++index_col) {
            row[index_col] = data_[index_col][index_row];
        }
    }

    std::swap(*this, transposed_matrix);

    return *this;
}

template <typename T>
void matrix<T>::swap_rows(const size_t num_row_1, const size_t num_row_2)
{
    if (num_row_1 >= num_rows_ || num_row_2 >= num_rows_) {
        std::cout << "ahaha 123" << std::endl;
        return;
    }

    std::swap(data_[num_row_1], data_[num_row_2]);
}

template <typename T>
void matrix<T>::add_row(const size_t num_row_1, const size_t num_row_2)
{
    if (num_row_1 >= num_rows_ || num_row_2 >= num_rows_) {
        std::cout << "ahaha 123" << std::endl;
        return;
    }

    row_t &data_row = data_[num_row_1];
    const row_t &data_row_add = data_[num_row_2];
    for (int index_col = 0; index_col != num_cols_; ++index_col) {
        data_row[index_col] += data_row_add[index_col];
    }
}

template <typename T>
void matrix<T>::dump() const
{
    for (int index_row = 0; index_row != num_rows_; ++index_row) {
        for (int index_col = 0; index_col != num_cols_; ++index_col) {
            std::cout << data_[index_row][index_col] << ' ';
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

template <typename T>
bool matrix<T>::is_square() const
{
    return num_cols_ == num_rows_;
}

}  // namespace matrix_space

#endif
