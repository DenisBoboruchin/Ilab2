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

    const matrix_space::matrix<int> matrix_cnst(10, 10, 7);
    ASSERT_EQ(matrix_cnst[4][2], 7);
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
}

TEST(matrix, mul)
{
    matrix<int> matrix1(2, 3, 1);
    matrix<int> matrix2(3, 2, 2);

    matrix<int> matrix_mul = matrix1 * matrix2;
}

TEST(matrix, eye_and_square)
{
    matrix<int> matrix_eye = matrix_space::matrix<int>::eye(2, 4);
    matrix<int> matrix_square = matrix_space::matrix<int>::square(5, 4);
}
