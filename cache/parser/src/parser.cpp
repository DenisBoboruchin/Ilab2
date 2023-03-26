#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

#include "parser.h"

std::vector<int> read_keys(const std::string &file_name)
{
    int capacity = 0;
    int num_keys = 0;
    std::vector<int> keys;

    std::ifstream input_fstream(file_name);
    std::stringstream input {};

    input << input_fstream.rdbuf();
    input_fstream.close();

    if (input) {
        input >> capacity;
        input >> num_keys;

        keys.push_back(capacity);
        for (int i = 1; i != num_keys + 1; i++) {
            int key = 0;
            input >> key;
            keys.push_back(key);
        }

        return keys;
    }

    std::cin >> capacity;
    std::cin >> num_keys;

    keys.push_back(capacity);
    for (int i = 1; i != num_keys + 1; i++) {
        int key = 0;
        std::cin >> key;

        keys.push_back(key);
    }

    return keys;
}

int slow_get_page_int(int key)
{
    return key;
}

char slow_get_page_char(int key)
{
    return char(key);
}

std::string slow_get_page_str(int key)
{
    std::string page {"I am a page"};
    return page;
}
