HWC - caches
===
This project is the first task on course "Uses and applications of C++ language" by K.I.Vladimirov. The task was to implement cache replacement algorithms.

Project structure
===
3 cache algorithms were implemented: 
- LFU
- LRU 
- perfect

Requirements
===
The following applications have to be installed:
- CMake 3.2 version (or higher)
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
To compile all:
```
make
```

Running
===
To run tests:
```
make run_caches_tests
```

To run lfu cache:
```
make run_cache_lfu
```

To run lru cache:
```
make run_cache_lru
```

To run perfect cache:
```
make run_cache_perfect
```
