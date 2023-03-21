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
