#include "matrix.h"
#include <iostream>

int main()
{
    int size = 0;
    std::cin >> size;
    matrix_space::matrix<double> matrix = matrix_space::matrix<double>::square(size);

    for (int index_row = 0; index_row != size; ++index_row) {
        for (int index_col = 0; index_col != size; ++index_col) {
            std::cin >> matrix[index_row][index_col];
        }
    }

    double determinant = matrix.determinant();
    std::cout << determinant << std::endl;

    return 0;
}
