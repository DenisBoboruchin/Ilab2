#include <iostream>
#include <list>
#include <unordered_map>
#include <map>

namespace cashe
{

template <typename T, typename keyT = int> struct cashe_t 
{
private:
    struct elem_
    {
        T   value;
        int num = 0;                                        // addition number  
    };

    size_t  sz_     = 0;
    int     nadd_   = 0;

    using ListPtr = typename std::list<elem_>::pointer;
    std::unordered_map <keyT, ListPtr> hash_;

public:
    cashe_t (size_t sz): sz_ {sz}, nadd_ {0} {};

    template <typename F> bool lookup_update (keyT key, F slow_get_page)
    {
        

        return true;
    }
};

};  
