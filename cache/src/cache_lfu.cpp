#include "cache.h"
#include "parser.h"

#include <cstring>

int main(int argc, char **argv)
{
    std::cout << get_lfu_hits(slow_get_page_int) << std::endl;

    return 0;
}
