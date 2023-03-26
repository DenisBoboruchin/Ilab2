#include <gtest/gtest.h>

#include "matrix.h"

using matrix_space::matrix;

int main()
{
    testing::InitGoogleTest();

    return RUN_ALL_TESTS();
}

TEST(matrix, constructor)
{
    matrix<int> first_matrix {};
    ASSERT_EQ(first_matrix.get_num_rows(), 0);
    ASSERT_EQ(first_matrix.get_num_cols(), 0);

    matrix<int> second_matrix(4, 6, 1);
    ASSERT_EQ(second_matrix.get_num_rows(), 4);
    ASSERT_EQ(second_matrix.get_num_cols(), 6);
}

TEST(matrix, square_brackets)
{
    matrix<int> matrix(5, 2, 15);
    ASSERT_EQ(matrix.get_num_rows(), 5);
    ASSERT_EQ(matrix.get_num_cols(), 2);

    ASSERT_EQ(matrix[1][1], 15);
    ASSERT_EQ(matrix[4][1], 15);

    matrix[3][0] = 10;
    ASSERT_EQ(matrix[3][0], 10);

    const matrix_space::matrix<int> matrix_const(10, 10, 7);
    ASSERT_EQ(matrix_const[4][2], 7);
}

TEST(matrix, is_square)
{
    matrix<int> matrix1(3, 2);
    ASSERT_TRUE(!matrix1.is_square());

    matrix<int> matrix2(3, 3);
    ASSERT_TRUE(matrix2.is_square());
}

TEST(matrix, transpose)
{
    matrix<int> matrix(3, 4, 6);
    matrix[2][1] = 7;
    matrix[1][3] = 8;

    matrix.transpose();
}

TEST(matrix, sum)
{
    matrix<int> matrix1(3, 6, 1);
    matrix1[2][1] = 3;
    matrix1[1][3] = 2;

    matrix<int> matrix2(3, 6, 2);
    matrix<int> matrix_sum = matrix1 + matrix2;
    matrix1 += matrix2;

    ASSERT_EQ(matrix_sum == matrix1, true);

    const matrix<int> matrix_const(3, 6, 3);
    matrix1 += matrix_const;
    matrix1[2][1] = 10;
    matrix1[1][3] = 8;
    matrix_sum += matrix_sum;

    ASSERT_EQ(matrix_sum == matrix1, true);
}

TEST(matrix, product)
{
    matrix<int> matrix1(2, 3, 1);
    matrix<int> matrix2(3, 2, 2);

    matrix<int> matrix_mul = matrix_space::product (matrix1, matrix2);
    matrix<int> res (2, 2, 6);

    ASSERT_EQ(matrix_mul == res, true);
}

TEST(matrix, mul_val)
{
    matrix<int> matrix1(3, 6, 3);
    matrix<int> matrix2(3, 6, 9);

    ASSERT_EQ(matrix2, matrix1 * 3);
    ASSERT_EQ(matrix2, 3 * matrix1);

    matrix1 *= 3;
    ASSERT_EQ(matrix2, matrix1);
}

TEST(matrix, compare_operator)
{
    matrix<int> matrix1(3, 4, 1);
    matrix<int> matrix2 = matrix1;
    ASSERT_EQ(matrix1 == matrix2, true);
    ASSERT_EQ(matrix1 != matrix2, false);

    matrix2[1][2] = 2;
    ASSERT_EQ(matrix1 == matrix2, false);
    ASSERT_EQ(matrix1 != matrix2, true);
}

TEST(matrix, eye_and_square)
{
    matrix<int> matrix_eye = matrix_space::matrix<int>::eye(2, 4);
    matrix<std::string> matrix_square = matrix_space::matrix<std::string>::square(5, "sqr");
}

TEST(matrix, swap_rows)
{
    matrix<int> matrix_1(3, 4, 2);
    matrix_1[1][3] = 0;
    matrix_1[2][1] = 8;

    matrix_1.swap_rows(1, 2);

    ASSERT_EQ(matrix_1[2][3], 0);
    ASSERT_EQ(matrix_1[1][1], 8);
    ASSERT_EQ(matrix_1[1][3], 2);
    ASSERT_EQ(matrix_1[2][1], 2);
}

TEST(matrix, add_row)
{
    matrix<int> matrix_1(3, 4, 2);
    matrix_1[1][3] = 0;
    matrix_1[2][1] = 8;

    matrix_1.add_row(1, 2);

    ASSERT_EQ(matrix_1[2][3], 2);
    ASSERT_EQ(matrix_1[1][1], 10);
    ASSERT_EQ(matrix_1[1][3], 2);
    ASSERT_EQ(matrix_1[2][1], 8);
}

TEST(matrix, determinant_2_2)
{
    matrix<int> matrix {2, 2};
    matrix[0][0] = 1;
    matrix[0][1] = 2;
    matrix[1][0] = 2;
    matrix[1][1] = 3;

    ASSERT_EQ(matrix.determinant(), -1);
}

TEST(matrix, determinant_3_3)
{
    matrix<int> matrix {3, 3};

    for (int i = 0; i != 3; ++i) {
        for (int j = 0; j != 3; ++j) {
            matrix[i][j] = i + j;
        }
    }
    matrix[0][0] = 1;

    ASSERT_EQ(matrix.determinant(), -1);
}

TEST(matrix, determinant_100_100)
{
    int size = 100;
    matrix<int> matrix = matrix_space::matrix<int>::eye(size, size);
    matrix[5][5] = 2;
    matrix[7][7] = 3;
    matrix[18][18] = 7;

    for (int i = 0; i != size; ++i) {
        for (int j = 0; j != size; ++j) {
            if (i != j) {
                matrix.add_row(i, j);
            }
        }
    }

    ASSERT_EQ(matrix.determinant(), 42);
}
