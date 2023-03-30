#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <iostream>
#include <exception>

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
    static matrix<double> to_double(const matrix<T> &other);

    proxy_row_t operator[](size_t num_row) &;
    const const_proxy_row_t operator[](size_t num_row) const &;

    matrix operator+(const matrix &other) const;
    matrix &operator+=(const matrix &other);

    matrix operator-(const matrix &other) const;
    matrix &operator-=(const matrix &other);

    matrix &operator*=(const T &value);

    bool operator!=(const matrix &other) const;
    bool operator==(const matrix &other) const;

    matrix &transpose();

    T determinant() const;
    int full_pivoting(const size_t index_row);

    void swap_rows(const size_t num_row_1, const size_t num_row_2);
    void swap_cols(const size_t num_col_1, const size_t num_col_2);

    void add_row(const size_t num_row_1, const size_t num_row_2);
    void add_row(const size_t num_row_1, const row_t &row);

    size_t get_num_rows() const;
    size_t get_num_cols() const;

    my_containers::vector<T> get_row(int num_row) const;

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
matrix<double> matrix<T>::to_double(const matrix<T> &other)
{
    size_t num_cols = other.get_num_cols();
    size_t num_rows = other.get_num_rows();

    matrix<double> matrix(num_cols, num_rows);
    for (int index_row = 0; index_row != num_rows; ++index_row) {
        const row_t &other_row = other[index_row].row;
        for (int index_col = 0; index_col != num_cols; ++index_col) {
            matrix[index_row][index_col] = other_row[index_col];
        }
    }

    return matrix;
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
        throw std::logic_error("not stackable");
    }

    matrix<T> sum {*this};
    sum += other;

    return sum;
}

template <typename T>
matrix<T> &matrix<T>::operator+=(const matrix &other)
{
    if (num_rows_ != other.get_num_rows() || num_cols_ != other.get_num_cols()) {
        throw std::logic_error("not stackable");
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
        throw std::logic_error("not deductible");
    }

    matrix<T> sub {*this};
    sub -= other;

    return sub;
}

template <typename T>
matrix<T> &matrix<T>::operator-=(const matrix &other)
{
    if (num_rows_ != other.get_num_rows() || num_cols_ != other.get_num_cols()) {
        throw std::logic_error("not deductible");
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
matrix<T> product(const matrix<T> &matrix_1, const matrix<T> &matrix_2)
{
    size_t num_rows = matrix_1.get_num_rows();
    size_t num_cols = matrix_1.get_num_cols();
    if (num_rows != matrix_2.get_num_cols() || num_cols != matrix_2.get_num_rows()) {
        throw std::logic_error("not multiplicable");
    }

    matrix<T> muled {num_rows, num_rows};
    matrix<T> matrix_2_transposed = matrix_2;
    matrix_2_transposed.transpose();

    for (int index = 0; index != num_rows; ++index) {
        for (int index_row = 0; index_row != num_rows; ++index_row) {
            T val = T {0};
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
    muled *= value;

    return muled;
}

template <typename T>
matrix<T> operator*(const T &value, const matrix<T> &matrix_mul)
{
    matrix<T> muled {matrix_mul};
    muled *= value;

    return muled;
}

template <typename T>
matrix<T> &matrix<T>::operator*=(const T &value)
{
    for (int index_row = 0; index_row != num_rows_; ++index_row) {
        row_t &data_row = data_[index_row];
        for (int index_col = 0; index_col != num_cols_; ++index_col) {
            data_row[index_col] *= value;
        }
    }

    return *this;
}

template <typename T>
bool matrix<T>::operator!=(const matrix &other) const
{
    if (num_rows_ != other.get_num_rows() || num_cols_ != other.get_num_cols()) {
        throw std::logic_error("not compariable");
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
int matrix<T>::full_pivoting(const size_t index)
{
    const double EPS = 0.001;
    int swaps_coef = 1;

    T max = std::abs(data_[index][index]);
    size_t piv_row = index;

    for (int index_swap_row = index; index_swap_row != num_cols_; ++index_swap_row) {
        T abs_value = std::abs(data_[index_swap_row][index]);
        if (abs_value > max) {
            max = abs_value;
            piv_row = index_swap_row;
        }
    }

    if (max > EPS) {
        if (index != piv_row) {
            swap_rows(index, piv_row);
            swaps_coef *= -1;
        }
        return swaps_coef;
    }

    size_t piv_col = index;
    for (int index_row = index; index_row != num_rows_; ++index_row) {
        row_t &row = data_[index_row];
        for (int index_col = index; index_col != num_cols_; ++index_col) {
            T abs_value = std::abs(row[index_col]);
            if (abs_value > max) {
                max = abs_value;
                piv_row = index_row;
                piv_col = index_col;
            }
        }
    }

    if (max == T {0}) {
        return 0;
    }

    if (index != piv_row) {
        swaps_coef *= -1;
        swap_rows(piv_row, index);
    }
    if (index != piv_col) {
        swaps_coef *= -1;
        swap_cols(piv_col, index);
    }

    std::cout << "adsf\n";
    return swaps_coef;
}

template <typename T>
T matrix<T>::determinant() const
{
    if (!is_square()) {
        throw std::logic_error("not square");
    }
    if (num_rows_ < 1) {
        throw std::out_of_range("out of range");
    }

    matrix<double> matrix = to_double(*this);

    int swaps_coef = 1;
    T determinant = T {1};
    for (int index = 0; index != num_cols_; ++index) {
        swaps_coef *= matrix.full_pivoting(index);
        if (swaps_coef == 0) {
            return T {0};
        }

        double elem = matrix[index][index];
        for (int index_row = index + 1; index_row != num_rows_; ++index_row) {
            double coef = -matrix[index_row][index] / elem;
            auto row = matrix.get_row(index);
            for (int i = 0, num = row.size(); i != num; ++i) {
                row[i] *= coef;
            }
            matrix.add_row(index_row, row);
        }

        determinant *= elem;
    }

    return determinant * swaps_coef;
}

template <typename T>
my_containers::vector<T> matrix<T>::get_row(int num_row) const
{
    if (num_row >= num_rows_ || num_row < 0) {
        throw std::out_of_range("num out of range");
    }
    return data_[num_row];
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
    if (num_row_1 >= num_rows_ || num_row_2 >= num_rows_ || num_row_1 < 0 || num_row_2 < 0) {
        throw std::out_of_range("num out of range");
    }

    if (num_row_1 != num_row_2) {
        std::swap(data_[num_row_1], data_[num_row_2]);
    }
}

template <typename T>
void matrix<T>::swap_cols(const size_t num_col_1, const size_t num_col_2)
{
    if (num_col_1 >= num_cols_ || num_col_2 >= num_cols_ || num_col_1 < 0 || num_col_2 < 0) {
        throw std::out_of_range("num out of range");
    }

    if (num_col_1 == num_col_2) {
        return;
    }

    for (int index_row = 0; index_row != num_rows_; ++index_row) {
        T &elem_1 = data_[index_row][num_col_1];
        T &elem_2 = data_[index_row][num_col_2];
        T temp_elem = elem_1;

        elem_1 = elem_2;
        elem_2 = temp_elem;
    }
}

template <typename T>
void matrix<T>::add_row(const size_t num_row_1, const size_t num_row_2)
{
    if (num_row_1 >= num_rows_ || num_row_2 >= num_rows_ || num_row_1 < 0 || num_row_2 < 0) {
        throw std::out_of_range("num out of range");
    }

    row_t &data_row = data_[num_row_1];
    const row_t &data_row_add = data_[num_row_2];
    for (int index_col = 0; index_col != num_cols_; ++index_col) {
        data_row[index_col] += data_row_add[index_col];
    }
}

template <typename T>
void matrix<T>::add_row(const size_t num_row_1, const row_t &row)
{
    if (num_row_1 >= num_rows_ || num_row_1 < 0 || row.size() != num_cols_) {
        throw std::out_of_range("num out of range");
    }

    row_t &data_row = data_[num_row_1];
    for (int index_col = 0; index_col != num_cols_; ++index_col) {
        data_row[index_col] += row[index_col];
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
