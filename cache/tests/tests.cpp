#include "cache.h"

#include <iostream>
#include <string>
#include <fstream>
#include <gtest/gtest.h>

int main ()
{
    ::testing::InitGoogleTest();

    return RUN_ALL_TESTS();
}

TEST (all_caches, small_test_1)
{
    std::string local_file = PROJECT_DIR_PATH + std::string{"/tests/tests_files/"} + "small_test_1.txt";
    
    std::ifstream input;
    input.open (local_file);

    std::streambuf* cinbuf = std::cin.rdbuf();
    std::streambuf* new_buf = input.rdbuf ();
    std::cin.rdbuf(new_buf);

    caches::hits hits = caches::get_hits ();

    std::cout << "lru hits: " << hits.hits_lru << "\n";
    std::cout << "lfu hits: " << hits.hits_lfu << "\n";
    std::cout << "beauty hits: " << hits.hits_beauty << "\n";

    std::cin.rdbuf(cinbuf);
}

