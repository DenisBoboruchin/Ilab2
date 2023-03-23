#include "cache.h"

struct hits {
    int hits_lru = 0;
    int hits_lfu = 0;
    int hits_perfect = 0;
};

std::vector<int> read_keys(const std::string &file_name);

template <typename T = int, typename keyT = int, typename F>
hits get_hits(const F get_page, const std::string &file_name = {})
{
    std::vector<int> keys = read_keys(file_name);

    int capacity = keys[0];
    keys.erase(keys.begin());

    caches::cache_lru<T, keyT> cache_lru(capacity);
    caches::cache_lfu<T, keyT> cache_lfu(capacity);
    caches::cache_perfect<T, keyT> cache_perfect(capacity);

    hits hits {};
    for (int i = 0; i != keys.size(); i++) {
        if (cache_lru.lookup_update(keys[i], get_page)) {
            hits.hits_lru++;
        }
        if (cache_lfu.lookup_update(keys[i], get_page)) {
            hits.hits_lfu++;
        }
    }

    hits.hits_perfect = cache_perfect.lookup_update(keys, get_page);

    return hits;
}

std::string slow_get_page_str(int key);
int slow_get_page_int(int key);
char slow_get_page_char(int key);
