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
    FILE* log_cashe_ = fopen ("dump/log_cashe.txt", "w+");
    
    size_t capasity_ = 0;
    size_t size_ = 0;
 
    const int START_FREQ = 1;

    std::unordered_map<keyT, int> hash_freq_;     

    using hashItr = typename std::unordered_map<keyT, int>::iterator;
    using listItr = typename std::list<std::pair<T, keyT>>::iterator;

    std::unordered_map<keyT, listItr> hash_listItr_;

    std::unordered_map<int, std::list<std::pair<T, keyT>>> cashe_; 
    using casheItr = typename std::unordered_map<int, std::list<std::pair<T, keyT>>>::iterator;

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
    
    void output_list (casheItr cashe_itr)
    {
        auto itr = cashe_.find (cashe_itr->first);
        if (itr != cashe_.end ())
        {    	   
            listItr itr_end = itr->second.end ();
            for (listItr page_itr = itr->second.begin (); page_itr != itr_end; page_itr++)
                fprintf (log_cashe_, "{%d, %d} ", page_itr->first, page_itr->second);

            fprintf (log_cashe_, "\n");
        }

    }

    void cashe_dump_ ()
    {
        casheItr cashe_end = cashe_.end ();
        for (casheItr cashe_itr = cashe_.begin (); cashe_itr != cashe_end; cashe_itr++)
        {
            fprintf (log_cashe_, "freq %d: ", cashe_itr->first);
 
            output_list (cashe_itr);
        }

        fprintf (log_cashe_, "\n\n\n");
    }

public:
    cashe_lfu (size_t sz): capasity_ {sz} {};

    bool check_full () {return (size_ == capasity_);} 

    template <typename F> bool lookup_update (const keyT key, const F slow_get_page)
    {
        hashItr hash_itr_page = hash_freq_.find (key);

        fprintf (log_cashe_, "add %d\n", key);

        if (hash_itr_page == hash_freq_.end ())
        {
            if (check_full ())
            {
                int min_freq = find_min_freq_ ();
                int delete_key = (--cashe_[min_freq].end())->second;
    
                hash_freq_.erase (delete_key);
                hash_listItr_.erase (delete_key);
                cashe_[min_freq].pop_back ();

                size_--;
            }
        
            cashe_[START_FREQ].push_front ({slow_get_page (key), key});
         
            hash_freq_.emplace (key, START_FREQ);
            hash_listItr_.emplace (key, cashe_[START_FREQ].begin ());

            size_++;
            
            fprintf (log_cashe_, "mimo\n");
            cashe_dump_ ();
            return false;
        }
        
        int freq_p = hash_itr_page->second;
        int new_freq_p = freq_p + 1;

        cashe_[new_freq_p].splice (cashe_[new_freq_p].begin (), cashe_[freq_p], hash_listItr_[key]);
        if (!cashe_[freq_p].size ())
            cashe_.erase (freq_p);
    
        hash_freq_[key] = new_freq_p;
        hash_listItr_[key] = cashe_[new_freq_p].begin ();

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
