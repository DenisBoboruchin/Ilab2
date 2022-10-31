#include "parser.h"

static std::vector<int> read_keys (std::string file_name);

hits get_hits (std::string file_name)
{  
    std::vector<int> keys = read_keys (file_name);

    int capacity = keys[0];
    keys.erase (keys.begin ());

    caches::cache_lru<char> cache_lru (capacity);
    caches::cache_lfu<char> cache_lfu (capacity);
    caches::cache_perfect<char> cache_perfect (capacity);

    hits hits {};
    for (int i = 0; i != keys.size(); i++)
    {
        if (cache_lru.lookup_update (keys[i], slow_get_page_char))
            hits.hits_lru++;

        if (cache_lfu.lookup_update (keys[i], slow_get_page_char))
            hits.hits_lfu++;
    }

    hits.hits_perfect = cache_perfect.lookup_update (keys, slow_get_page_char);

    return hits;
}

std::vector<int> read_keys (std::string file_name)
{
    int capacity = 0;
    int num_keys = 0;    
    std::vector<int> keys;

    std::ifstream input (file_name);

    if (input)
    {       
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

