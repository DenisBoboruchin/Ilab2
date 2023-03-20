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
