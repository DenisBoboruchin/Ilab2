#pragma once

#include <iostream>
#include <list>
#include <unordered_map>
#include <assert.h>

namespace cashes
{

//-------------------------------------------------------------------------------------------------
//--------------------------------------Cashe-type-LFU---------------------------------------------
//-------------------------------------------------------------------------------------------------
template <typename T, typename keyT = int> struct cashe_lfu
{
private:
    FILE*       log_cashe_  = fopen ("dump/log_cashe.txt", "w+");
    
    size_t      sz_         = 0;
    size_t      size        = 0;
 
    //keyT -> frequency including
    std::unordered_map<keyT, int> hash_;     

    using hashItr  = typename std::unordered_map<keyT, int>::iterator;

    //frequency -> list with pages with this frequency
    std::unordered_map<int, std::list<std::pair<T, keyT>>> cashe_; 

public:
    cashe_lfu (size_t sz): sz_ {sz} {};

    bool check_full () {assert (size == sz_);} 

    template <typename F> bool lookup_update (const keyT key, const F slow_get_page)
    {
        hashItr hash_itr_page = hash_.find (key);

        if (hash_itr_page == hash_.end ())
        {
            if (check_full ())
            {

                //cashe_[find_min_freq_ ()].
            }
        }

        return true;
    }

};

//-------------------------------------------------------------------------------------------------
//--------------------------------------Cashe-type-LRU---------------------------------------------
//-------------------------------------------------------------------------------------------------
template <typename T, typename keyT = int> struct cashe_lru 
{
private:
    FILE*       log_cashe_  = fopen ("dump/log_cashe.txt", "w+");

    size_t      sz_         = 0;

    std::list<std::pair<T, keyT>> cashe_;
    using listItr = typename std::list<std::pair<T, keyT>>::iterator;

    std::unordered_map<keyT, listItr> hash_;
    using hashItr = typename std::unordered_map<keyT, listItr>::iterator;

    void cashe_dump_ (void)
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

public:
    cashe_lru (size_t sz): sz_ {sz} {};
   
    bool check_full () const 
    {
        int csize = cashe_.size ();
        assert (csize == hash_.size ()); 

        return (csize == sz_);
    }
	
    template <typename F> bool lookup_update (const keyT key, const F slow_get_page)
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

            cashe_dump_ ();
            return false;
        }


        listItr page_itr = hash_itr_page->second;
        if (page_itr != cashe_.begin ())
            cashe_.splice (cashe_.begin (), cashe_, page_itr);

        cashe_dump_ ();
        return true;
    }
};

};  
