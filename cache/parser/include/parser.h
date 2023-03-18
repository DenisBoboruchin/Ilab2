#include "cache.h"

struct hits {
    int hits_lru = 0;
    int hits_lfu = 0;
    int hits_perfect = 0;
};

hits get_hits(const std::string &file_name = {});
int slow_get_page_int(int key);
char slow_get_page_char(int key);
