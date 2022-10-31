#include "parser.h"

#include <iostream>
#include <string>
#include <gtest/gtest.h>

std::string get_local_file (std::string file_name)
{
    return (PROJECT_DIR_PATH + std::string{"/tests/tests_files/"} + file_name);
}

int main ()
{
    ::testing::InitGoogleTest();

    return RUN_ALL_TESTS();
}

/*
TEST (all_cashes, test_from_cin)
{
    hits hits = get_hits ();
}*/

TEST (all_caches, small_test_1)
{
    hits hits = get_hits (get_local_file ("small_test_1.txt"));

    ASSERT_EQ (hits.hits_lru, 6);
    ASSERT_EQ (hits.hits_lfu, 5);
}

TEST (all_caches, small_test_2)
{
    hits hits = get_hits (get_local_file ("small_test_2.txt"));

    ASSERT_EQ (hits.hits_lru, 3);
    ASSERT_EQ (hits.hits_lfu, 1);
}

TEST (all_caches, small_test_3)
{
    hits hits = get_hits (get_local_file ("small_test_3.txt"));

    ASSERT_EQ (hits.hits_lru, 2);
    ASSERT_EQ (hits.hits_lfu, 3);
}

TEST (all_caches, small_test_4)
{
    hits hits = get_hits (get_local_file ("small_test_4.txt"));

    ASSERT_EQ (hits.hits_lru, 1);
    ASSERT_EQ (hits.hits_lfu, 4);
}

/*
TEST (all_caches, big_trivial_test_1000000)
{
    hits hits = get_hits (get_local_file ("big_trivial_test_1000000.txt"));

    ASSERT_EQ (hits.hits_lru, 0);
    ASSERT_EQ (hits.hits_lfu, 0);

}*/
