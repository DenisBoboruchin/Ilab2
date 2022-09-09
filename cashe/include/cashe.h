#include <iostream>
#include <list>
#include <unordered_map>
#include <map>

namespace cashe
{

template <typename T, typename keyT = int> struct cashe_t 
{
private:
    size_t  sz_     = 0;
    
    std::list <T> cashe_;

    using ListPtr = typename std::list <T>::pointer;
    std::unordered_map <keyT, ListPtr> hash_;

public:
    cashe_t (size_t sz): sz_ {sz} {};
   
    bool check_full_ () const 
    {
        size_t csize = cashe_.size;
        assert (csize == hash_.size); 

        return (csize == sz_);
    }

    template <typename F> bool lookup_update (keyT key, F slow_get_page)
    {
        auto hit = hash_.find (key);
        if (hit == hash_.end ())
        {
            if (check_full_ ())
            {
                hash_.erase ();
            }

            return true;
        }

        
        return true;
    }
};

};  
