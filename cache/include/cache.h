#pragma once

#include <iostream>
#include <list>
#include <unordered_map>
#include <assert.h>
#include <cstdio>

namespace caches
{

struct hits
{
    int hits_lru = 0;
    int hits_lfu = 0;
    int hits_beauty = 0;
};

hits get_hits ();
int slow_get_page_int (int key);
char slow_get_page_char (int key);

const size_t DEFAULT_CAPACITY = 10;
const int START_FREQ = 1;

//-------------------------------------------------------------------------------------------------
//--------------------------------------cache-type-LFU---------------------------------------------
//-------------------------------------------------------------------------------------------------

template <typename T, typename keyT = int> struct cache_lfu_lists
{
private:
    FILE* log_cache_  = fopen ("../dump/log_cache2.txt", "w+");

    size_t capacity_ = DEFAULT_CAPACITY;
    size_t size_ = 0;   
       
    struct elem_;
    using listsItr = typename std::list<std::pair<int, std::list<elem_>>>::iterator;

    struct elem_
    {
        T page_;
        keyT key_page_;
        
        listsItr lists_itr_;
    };

    std::list<std::pair<int, std::list<elem_>>> lists_;
    using listItr = typename std::list<elem_>::iterator;

    std::unordered_map<keyT, listItr> hash_list_;
    using hashItr = typename std::unordered_map<keyT, listItr>::iterator;

    bool check_full_ () const
    {
        return size_ == capacity_;
    }

    void cache_dump_ ()
    {
        listsItr end_lists = lists_.end ();
        for (listsItr pair = lists_.begin (); pair != end_lists; ++pair)
        {
            printf ("freq_list: %d\n", pair->first);
            printf ("size_list: %ld\n", pair->second.size ());
            listItr elem = pair->second.begin ();
            listItr last = pair->second.end ();
            for (; elem != last; ++elem)
            {
                printf ("{%d} ", elem->page_);
            }

            printf ("\n");
        }

        printf ("\n");
    }

public:
    cache_lfu_lists (size_t capacity = DEFAULT_CAPACITY): capacity_ {capacity} {}; 

    template <typename F> bool lookup_update (const keyT key, const F slow_get_page)
    {
        printf ("add %d\n", key);
        cache_dump_ ();
        
        hashItr hit = hash_list_.find (key);
        if (hit == hash_list_.end ())
        {
            if (check_full_ ())
            { 
                listsItr unuse_pair_itr = lists_.begin ();  

                listItr unuse_page_itr = --((unuse_pair_itr->second).end ());
                keyT unuse_key = unuse_page_itr->key_page_;

                hash_list_.erase (unuse_key);
                unuse_pair_itr->second.pop_back ();

                if (unuse_pair_itr->first != START_FREQ)
                    if (!unuse_pair_itr->second.size ())
                        lists_.pop_front ();

                size_--;
            }   
            
            listsItr first_pair_itr = lists_.begin ();
            if (first_pair_itr->first != START_FREQ)
            {
               lists_.push_front ({START_FREQ, std::list<elem_> {}});

               first_pair_itr = lists_.begin ();
            }

            first_pair_itr->second.push_front ({slow_get_page (key), key, first_pair_itr});
            printf ("size started: %ld\n", first_pair_itr->second.size ());
            hash_list_[key] = first_pair_itr->second.begin ();

            size_++;
            return false;
        }
        
        listItr itr_elem = hit->second;
        listsItr itr_pair = itr_elem->lists_itr_;
        printf ("size cringe: %ld\n", itr_pair->second.size ());
            
        listsItr itr_next_pair = std::next(itr_pair);
        int& freq = itr_pair->first;

        if ((itr_next_pair == lists_.end ()) || ((itr_next_pair)->first != freq + 1))
        {
            itr_next_pair = lists_.insert (itr_next_pair, {freq + 1, std::list<elem_> {}});
        }
        std::list<elem_>& list_elem = itr_next_pair->second;

        printf ("elem: %d\n", itr_elem->page_);
        printf ("size before: %ld\n", itr_pair->second.size ());
        
        //auto endlist = itr_pair->second.end ();
        //for (auto elem = itr_pair->second.begin (); elem != endlist)

        list_elem.splice (list_elem.begin (), itr_pair->second, itr_elem);  
        printf ("size after: %ld\n", itr_pair->second.size ());

        
        if (!itr_pair->second.size ())
            lists_.erase (itr_pair);

        hash_list_[key] = list_elem.begin ();

        return true;
    }
};  


template <typename T, typename keyT = int> struct cache_lfu_hashes
{
private:
    FILE* log_cache_ = fopen ("../dump/log_cache_hashes.txt", "w+");
    
    size_t capasity_ = DEFAULT_CAPACITY;
    size_t size_ = 0;
        
    std::unordered_map<keyT, int> hash_freq_;     
    
    using hashItr = typename std::unordered_map<keyT, int>::iterator;
    using listItr = typename std::list<std::pair<T, keyT>>::iterator;
    
    std::unordered_map<keyT, listItr> hash_listItr_;
    
    std::unordered_map<int, std::list<std::pair<T, keyT>>> cache_; 
    using cacheItr = typename std::unordered_map<int, std::list<std::pair<T, keyT>>>::iterator;
    
    int find_min_freq_ ()
    {
        int min_freq = 0;
        while (true)
        {
            if (cache_.find (min_freq) != cache_.end ())
                return min_freq;
                
            min_freq++;
        }
    }
    
    void output_list_ (cacheItr cache_itr)
    {
        auto itr = cache_.find (cache_itr->first);
        if (itr != cache_.end ())
        {    	   
            listItr itr_end = itr->second.end ();
            for (listItr page_itr = itr->second.begin (); page_itr != itr_end; page_itr++)
                fprintf (log_cache_, "{%d, %d} ", page_itr->first, page_itr->second);

            fprintf (log_cache_, "\n");
        }
    }

    void cache_dump_ ()
    {
        cacheItr cache_end = cache_.end ();
        for (cacheItr cache_itr = cache_.begin (); cache_itr != cache_end; cache_itr++)
        {
            fprintf (log_cache_, "freq %d: ", cache_itr->first);
 
            output_list_ (cache_itr);
        }

        fprintf (log_cache_, "\n\n\n");
    }

public:
    cache_lfu_hashes (size_t sz): capasity_ {sz} {};

    bool check_full () {return (size_ == capasity_);} 

    template <typename F> bool lookup_update (const keyT key, const F slow_get_page)
    {
        hashItr hash_itr_page = hash_freq_.find (key);

        fprintf (log_cache_, "add %d\n", key);

        if (hash_itr_page == hash_freq_.end ())
        {
            if (check_full ())
            {
                int min_freq = find_min_freq_ ();
                int delete_key = (--cache_[min_freq].end())->second;
    
                hash_freq_.erase (delete_key);
                hash_listItr_.erase (delete_key);
                cache_[min_freq].pop_back ();

                size_--;
            }
        
            cache_[START_FREQ].push_front ({slow_get_page (key), key});
         
            hash_freq_.emplace (key, START_FREQ);
            hash_listItr_.emplace (key, cache_[START_FREQ].begin ());

            size_++;
            
            fprintf (log_cache_, "mimo\n");
            cache_dump_ ();
            return false;
        }
        
        int freq_p = hash_itr_page->second;
        int new_freq_p = freq_p + 1;

        cache_[new_freq_p].splice (cache_[new_freq_p].begin (), cache_[freq_p], hash_listItr_[key]);
        if (!cache_[freq_p].size ())
            cache_.erase (freq_p);
    
        hash_freq_[key] = new_freq_p;
        hash_listItr_[key] = cache_[new_freq_p].begin ();

        fprintf (log_cache_, "hit\n");
        cache_dump_ ();
        return true;
    }

};


//-------------------------------------------------------------------------------------------------
//--------------------------------------cache-type-LRU---------------------------------------------
//-------------------------------------------------------------------------------------------------
template <typename T, typename keyT = int> struct cache_lru 
{
private:
    FILE*       log_cache_  = fopen ("dump/log_cache.txt", "w+");

    size_t      sz_         = 0;

    std::list<std::pair<T, keyT>> cache_;
    using listItr = typename std::list<std::pair<T, keyT>>::iterator;

    std::unordered_map<keyT, listItr> hash_;
    using hashItr = typename std::unordered_map<keyT, listItr>::iterator;

    void cache_dump_ (void)
    {       
        fprintf (log_cache_, "in cache: ");
            
    	listItr itr_elem = cache_.begin ();
        listItr itr_end  = cache_.end ();
    	while (itr_elem != itr_end)
    	{
        	fprintf (log_cache_, "{%d, %d} ", itr_elem->first, itr_elem->second);

        	itr_elem++;
    	}

    	fprintf (log_cache_, "\n");
    } 

public:
    cache_lru (size_t sz): sz_ {sz} {};
   
    bool check_full () const 
    {
        int csize = cache_.size ();
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
                hash_.erase (cache_.back().second);
                cache_.pop_back ();
            }

            cache_.push_front ({slow_get_page (key), key});
            hash_[key] = cache_.begin ();

            //cache_dump_ ();
            return false;
        }


        listItr page_itr = hash_itr_page->second;
        if (page_itr != cache_.begin ())
            cache_.splice (cache_.begin (), cache_, page_itr);

        //cache_dump_ ();
        return true;
    }
};

};
