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
 
    const int   start_freq  = 1;

    //keyT -> frequency including
    std::unordered_map<keyT, int> hash_;     

    using hashItr   = typename std::unordered_map<keyT, int>::iterator;
    using listItr   = typename std::list<std::pair<T, keyT>>::iterator;

    std::unordered_map<keyT, listItr> hash_list_itr_;

    //frequency -> list with pages with this frequency
    std::unordered_map<int, std::list<std::pair<T, keyT>>> cashe_; 
    using casheItr  = typename std::unordered_map<int, std::list<std::pair<T, keyT>>>::iterator;

    int find_min_freq_ ()
    {
        int min_freq = 0;
        while (true)
        {
            if (cashe_.find (min_freq) != cashe_.end ())
                return min_freq;

            min_freq++;
        }
    }

    void cashe_dump_ ()
    {
        casheItr cashe_itr = cashe_.begin ();
        casheItr cashe_end = cashe_.end ();

        while (cashe_itr != cashe_end)
        {
            fprintf (log_cashe_, "freq %d: ", cashe_itr->first);
 
            auto itr = cashe_.find (cashe_itr->first);
            if (itr != cashe_.end ())
            {    	    
                listItr itr_elem = itr->second.begin ();
    	        while (itr_elem != itr->second.end ())
    	        {
        	        fprintf (log_cashe_, "{%d, %d} ", itr_elem->first, itr_elem->second);

        	        itr_elem++;
    	        }

                fprintf (log_cashe_, "\n");
            }

            cashe_itr++;
        }

        fprintf (log_cashe_, "\n\n\n");
    }

public:
    cashe_lfu (size_t sz): sz_ {sz} {};

    bool check_full () {return (size == sz_);} 

    template <typename F> bool lookup_update (const keyT key, const F slow_get_page)
    {
        hashItr hash_itr_page = hash_.find (key);

        fprintf (log_cashe_, "add %d\n", key);

        if (hash_itr_page == hash_.end ())
        {
            if (check_full ())
            {
                int min_freq = find_min_freq_ ();
                int delete_key = (--cashe_[min_freq].end())->second;
    
                hash_.erase (delete_key);
                hash_list_itr_.erase (delete_key);
                cashe_[min_freq].pop_back ();

                size--;
            }
        
            cashe_[start_freq].push_front ({slow_get_page (key), key});
         
            hash_.emplace (key, start_freq);
            hash_list_itr_.emplace (key, cashe_[start_freq].begin ());

            size++;
            
            fprintf (log_cashe_, "mimo\n");
            cashe_dump_ ();
            return false;
        }
        
        int freq_p = hash_itr_page->second;
        int new_freq_p = freq_p + 1;

        cashe_[new_freq_p].splice (cashe_[new_freq_p].begin (), cashe_[freq_p], hash_list_itr_[key]);
        if (!cashe_[freq_p].size ())
            cashe_.erase (freq_p);
    
        hash_[key] = new_freq_p;
        hash_list_itr_[key] = cashe_[new_freq_p].begin ();

        fprintf (log_cashe_, "hit\n");
        cashe_dump_ ();
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
        listItr itr_end  = cashe_.end ();
    	while (itr_elem != itr_end)
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

            cashe_.push_front ({slow_get_page (key), key});
            hash_[key] = cashe_.begin ();

            //cashe_dump_ ();
            return false;
        }


        listItr page_itr = hash_itr_page->second;
        if (page_itr != cashe_.begin ())
            cashe_.splice (cashe_.begin (), cashe_, page_itr);

        //cashe_dump_ ();
        return true;
    }
};

};
