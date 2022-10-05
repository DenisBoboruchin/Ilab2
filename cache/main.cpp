#include <iostream>
#include <stdio.h>
//#include <gtest/gtest.h>

#include "include/cache.h"

int get_hits ();
int slow_get_page_int (int key);
char slow_get_page_char (int key);

int main ()
{
    int hits = get_hits ();
    printf ("Число совпадений: %d\n", hits);

    return 0; 
}

int get_hits ()
{
    int capacity = 0;
    scanf ("%d", &capacity);
   
    caches::cache_lfu_lists<char> cache (capacity);
    
    int num_keys = 0;
    scanf ("%d", &num_keys);

    int hits = 0;
    int key = 0;
    scanf ("%d", &key);

    for (int i = 1; i < num_keys; i++)
    {
        if (cache.lookup_update (key, slow_get_page_char))
            hits++;

        scanf ("%d", &key);
    }

    return hits;
}

char slow_get_page_char (int key)
{
    return char (key);
}

int slow_get_page_int (int key)
{
    return key;
}


