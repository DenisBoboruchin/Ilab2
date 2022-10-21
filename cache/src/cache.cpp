#include "cache.h"

#include <iostream>
#include <fstream>

namespace caches
{

hits get_hits (std::string file_name)
{
    std::ifstream input;
    std::streambuf* cin_buf = std::cin.rdbuf ();

    if (file_name.size ())
    {
        std::string local_file = PROJECT_DIR_PATH 
                            + std::string{"/tests/tests_files/"} + file_name;    
        
        input.open (local_file);
        
        std::streambuf* new_buf = input.rdbuf ();
        std::cin.rdbuf(new_buf);
    }
   
    int capacity = 0;
    std::cin >> capacity;
   
    caches::cache_lru<char> cache_lru (capacity);
    caches::cache_lfu<char> cache_lfu (capacity);

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

    std::cin.rdbuf(cin_buf);
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

