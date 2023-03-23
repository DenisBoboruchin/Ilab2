#include "cache.h"
#include "parser.h"

#include <cstring>

int main (int argc, char** argv)
{
    if (!strcmp (argv[1], "-lfu"))
    {
        std::cout << get_lfu_hits (slow_get_page_int) << std::endl;
    }
    else if (!strcmp (argv[1], "-lru"))
    {
        std::cout << get_lru_hits (slow_get_page_int) << std::endl;
    }
    else if (!strcmp (argv[1], "-perfect"))
    {
        std::cout << get_perfect_hits (slow_get_page_int) << std::endl;
    }

    return 0;
}
