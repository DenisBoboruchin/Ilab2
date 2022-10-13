#include "cache.h"

#include <iostream>

namespace caches
{

hits get_hits ()
{
    int capacity = 0;

    std::cin >> capacity;
   
    caches::cache_lru<char> cache_lru (capacity);
    caches::cache_lfu_lists<char> cache_lfu (capacity);

    int num_keys = 0;
    std::cin >> num_keys;
   
    hits hits {};
    for (int i = 0; i < num_keys; i++)
    {
        int key = 0;
        std::cin >> key;
       
        if (cache_lru.lookup_update (key, slow_get_page_char))
            hits.hits_lru++;

        if (cache_lfu.lookup_update (key, slow_get_page_char))
            hits.hits_lfu++;
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

}

