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

    std::list <T> cashe_;

    using ListPtr = typename std::list <T>::iterator;
    std::unordered_map <keyT, ListPtr> hash_;

//-------------------------------------------------------------------------------------------------
//--------------------------------------Cashe-type-LRU---------------------------------------------
//-------------------------------------------------------------------------------------------------
    template <typename F> bool update_as_LRU (const keyT key, const F slow_get_page)
    {
        auto hit = hash_.find (key);
    	if (hit == hash_.end ())
    	{
        	if (check_full ())
        	{
        	    hash_.erase (cashe_.back ());
        	    cashe_.pop_back ();
        	}
            
        	cashe_.push_front (slow_get_page (key));
	        hash_[key] = cashe_.begin ();

        	return false;
    	}

    	auto pos       = hit->second;
    	auto pos_first = cashe_.begin ();
    	if (pos != pos_first)
        	cashe_.splice (pos_first, cashe_, pos);
        
    	return true;
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

            default:
                assert (!"ERROR!!! Unknown type of cashe");
        }
    }
};

};  
