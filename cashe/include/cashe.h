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
    FILE*       log_cashe_  = fopen ("dump/log_cashe.txt", "w+");

    size_t      sz_         = 0;
    cashe_type  cashe_type_ = LRU; 

    std::list<std::pair<T, keyT>> cashe_;
    using listItr = typename std::list<std::pair<T, keyT>>::iterator;

    std::unordered_map<keyT, listItr> hash_;
    using hashItr = typename std::unordered_map<keyT, listItr>::iterator;

    typename std::list<std::pair<T, keyT>>::iterator itr_unuse_page_ = cashe_.begin (); //only for LFU
    T unuse_page_;                                                                      //only for LFU

    void output_cashe_ (void)
    {       
        fprintf (log_cashe_, "in cashe: ");
            
    	listItr itr_elem = cashe_.begin ();
    	while (itr_elem != cashe_.end ())
    	{
        	fprintf (log_cashe_, "{%d, %d} ", itr_elem->first, itr_elem->second);

        	itr_elem++;
    	}

    	fprintf (log_cashe_, "\n");
    } 

//-------------------------------------------------------------------------------------------------
//--------------------------------------Cashe-type-LRU---------------------------------------------
//-------------------------------------------------------------------------------------------------
    template <typename F> bool update_as_LRU (const keyT key, const F slow_get_page)
    {
        hashItr hash_itr_page = hash_.find (key);

        if (hash_itr_page == hash_.end ())
        {
            if (check_full ())
            {
                hash_.erase (cashe_.back().second);
                cashe_.pop_back ();
            }

            cashe_.push_front (std::pair (slow_get_page (key), key));
            hash_[key] = cashe_.begin ();

            output_cashe_ ();
            return false;
        }


        listItr page_itr = hash_itr_page->second;
        if (page_itr != cashe_.begin ())
            cashe_.splice (cashe_.begin (), cashe_, page_itr);

        output_cashe_ ();
        return true;
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
