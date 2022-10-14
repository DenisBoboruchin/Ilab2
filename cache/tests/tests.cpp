#include "cache.h"

#include <iostream>
#include <string>
#include <gtest/gtest.h>
#include <fstream>

int main ()
{
    ::testing::InitGoogleTest();

    return RUN_ALL_TESTS();
}

TEST (all_caches, small_test_1)
{
    caches::hits hits = caches::get_hits ("small_test_1.txt");

    ASSERT_EQ (hits.hits_lru, 6);
    ASSERT_EQ (hits.hits_lfu, 5);
}

TEST (all_caches, small_test_2)
{
    caches::hits hits = caches::get_hits ("small_test_2.txt");

    ASSERT_EQ (hits.hits_lru, 3);
    ASSERT_EQ (hits.hits_lfu, 1);
}

TEST (all_caches, small_test_3)
{
    caches::hits hits = caches::get_hits ("small_test_3.txt");

    ASSERT_EQ (hits.hits_lru, 2);
    ASSERT_EQ (hits.hits_lfu, 3);
}

TEST (all_caches, small_test_4)
{
    caches::hits hits = caches::get_hits ("small_test_4.txt");

    ASSERT_EQ (hits.hits_lru, 1);
    ASSERT_EQ (hits.hits_lfu, 4);
}

TEST (all_caches, big_trivial_test_1000000)
{

    std::ofstream out;
    out.open("../tests/tests_files/big_trivial_test_1000000.txt");


    out << 5 << '\n';
    out << 1000000 << '\n';
    int n = 1000000;
    for (int i = 0; i != n; ++i)
        out << i << '\n';

    out.close ();

    caches::hits hits = caches::get_hits ("big_trivial_test_1000000.txt");

    ASSERT_EQ (hits.hits_lru, 0);
    ASSERT_EQ (hits.hits_lfu, 0);

}
