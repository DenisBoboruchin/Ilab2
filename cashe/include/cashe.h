#pragma once

#include <iostream>
#include <list>
#include <unordered_map>
#include <assert.h>

namespace cashes
{

enum cashe_type
{
    LRU = 1,
    LFU
};

template <typename T, typename keyT = int> struct cashe_t 
{
private:
    size_t      sz_         = 0;
    cashe_type  cashe_type_ = LRU; 


    std::list<std::pair<T, keyT>> cashe_;

    std::unordered_map<keyT, std::pair<T, keyT>*> hash_;
    using hashItr = typename std::unordered_map<keyT, std::pair<T, keyT>*>::iterator;

    typename std::list<std::pair<T, keyT>>::iterator itr_unuse_page_ = cashe_.begin (); //only for LFU
    T unuse_page_;                                                                      //only for LFU

//-------------------------------------------------------------------------------------------------
//--------------------------------------Cashe-type-LRU---------------------------------------------
//-------------------------------------------------------------------------------------------------
    template <typename F> bool update_as_LRU (const keyT key, const F slow_get_page)
    {
        hashItr new_page_itr = hash_.find (key);

        if (new_page_itr == hash_.end ())
        {
            if (check_full ())
            {
                hash_.erase (cashe_.back().second);
                cashe_.pop_back ();
            }

            std::pair<T, keyT> new_elem = std::pair (slow_get_page (key), key);
            cashe_.push_front (new_elem);
            hash_[key] = &new_elem;

            return true;
        }


    }

//-------------------------------------------------------------------------------------------------
//--------------------------------------Cashe-type-LFU---------------------------------------------
//-------------------------------------------------------------------------------------------------
    template <typename F> bool update_as_LFU (const keyT key, const F slow_get_page)
    {
        auto hit = hash_.find (key);
        if (hit == hash_.end ())
        {
            if (check_full ())
            { 
                //if (itr_unuse_page_ == 0)

                //hash_.erase  (unuse_page_);
                //cashe_.erase (itr_unuse_page_);
            }
        }
    }

public:
    cashe_t (size_t sz, cashe_type cashe_type = LRU): sz_ {sz}, cashe_type_ {cashe_type} {};
   
    bool check_full () const 
    {
        int csize = cashe_.size ();
        assert (csize == hash_.size ()); 

        return (csize == sz_);
    }
	
    template <typename F> bool lookup_update (const keyT key, const F slow_get_page)
    {
        switch (cashe_type_)
        {
            case LRU:
                return update_as_LRU (key, slow_get_page);
            
            case LFU:
                return update_as_LFU (key, slow_get_page);

            default:
                assert (!"ERROR!!! Unknown type of cashe");
        }
    }
};

};  
