#include "parser.h"

static std::vector<int> read_keys (std::string file_name);

hits get_hits (std::string file_name)
{  
    std::vector<int> keys = read_keys (file_name);

    int capacity = keys[0];
    caches::cache_lru<char> cache_lru (capacity);
    caches::cache_lfu<char> cache_lfu (capacity);
  
    hits hits {};
    for (int i = 1; i != keys.size(); i++)
    {
        if (cache_lru.lookup_update (keys[i], slow_get_page_char))
            hits.hits_lru++;

        if (cache_lfu.lookup_update (keys[i], slow_get_page_char))
            hits.hits_lfu++;
    }

    return hits;
}

std::vector<int> read_keys (std::string file_name)
{
    int capacity = 0;
    int num_keys = 0;    
    std::vector<int> keys;
 
    if (file_name.size ())
    {
        std::string local_file = PROJECT_DIR_PATH 
                            + std::string{"/tests/tests_files/"} + file_name;    
     
        std::ifstream input;
        input.open (local_file);
        
        input >> capacity;
        input >> num_keys;

        keys.push_back (capacity);
        for (int i = 1; i != num_keys + 1; i++)
        {
            int key = 0;
            input >> key;

            keys.push_back (key);
        }
        
        input.close ();

        return keys;
    }

    std::cin >> capacity;
    std::cin >> num_keys;
    
    keys.push_back (capacity);
    for (int i = 1; i != num_keys + 1; i++)
    {
        int key = 0;
        std::cin >> key;
        
        keys.push_back (key);
    }
 
    return keys;
}

char slow_get_page_char (int key)
{
    return char (key);
}

int slow_get_page_int (int key)
{
    return key; 
}

