HWMX - matrix
===
This project is a task on course "Uses and applications of C++ language" by K.I.Vladimirov. The task was to implement a class representing matrix.
My matrix calculate determinant with partial and full pivoting.

Requirements
===
The following applications have to be installed:
- CMake 3.16.3 version (or higher)
- GTest
- g++

How to build
===
To compile you need to use сmake in the directory build:
```
mkdir build
cd build
сmake ..
```
To compile tests:
```
make matrix_tests
```
To compile main program:
```
make matrix_determinant
```
To compile all:
```
make
```

Running
===
To run tests:
```
make run_matrix_tests
```

To run main programm:
```
make run_matrix_determinant
```
