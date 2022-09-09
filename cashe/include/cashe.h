#include <iostream>
#include <list>
#include <unordered_map>
#include <map>

namespace cashes
{

template <typename T, typename keyT = int> struct cashe_t 
{
private:
    size_t  sz_     = 0;
    
    std::list <T> cashe_;

    using ListPtr = typename std::list <T>::iterator;
    std::unordered_map <keyT, ListPtr> hash_;

public:
    cashe_t (size_t sz): sz_ {sz} {};
   
    bool check_full () const 
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
            if (check_full ())
            {
                hash_.erase (cashe_.end ());
                cashe_.pop_back ();
            }
            
            cashe_.push_front (slow_get_page (key));
            hash_[key] = cashe.begin ();

            return false;
        }
       
        return true;
    }

};

};  
