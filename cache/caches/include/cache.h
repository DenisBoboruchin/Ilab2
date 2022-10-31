#pragma once

#include <iostream>
#include <list>
#include <vector>
#include <map>
#include <unordered_map>
#include <cassert>
#include <cstdio>
#include <functional>

namespace caches
{
    
const int START_FREQ = 1;

//-------------------------------------------------------------------------------------------------
//--------------------------------------cache-type-LFU---------------------------------------------
//-------------------------------------------------------------------------------------------------

template <typename T, typename keyT = int>
struct cache_lfu final
{
private:
    size_t capacity_ = 10;

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
    cache_lfu (size_t capacity): capacity_ {capacity} {}; 

    template <typename F> bool lookup_update (const keyT key, const F slow_get_page)
    {        
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
            hash_list_[key] = first_pair_itr->second.begin ();

            size_++;
            return false;
        }
        
        listItr itr_elem = hit->second;
        listsItr itr_pair = itr_elem->lists_itr_;
           
        listsItr itr_next_pair = std::next(itr_pair, 1);

        if ((itr_next_pair == lists_.end ()) || ((itr_next_pair)->first != itr_pair->first + 1))
            itr_next_pair = lists_.insert (itr_next_pair, {itr_pair->first + 1, std::list<elem_> {}});
    
        std::list<elem_>& list_elem = itr_next_pair->second;

        list_elem.splice (list_elem.begin (), itr_pair->second, itr_elem);  
        itr_elem->lists_itr_ = itr_next_pair;
               
        if (!itr_pair->second.size ())
            lists_.erase (itr_pair);

        hash_list_[key] = list_elem.begin ();

        return true;
    }
};  

//-------------------------------------------------------------------------------------------------
//------------------------------------cache-type-perfect-------------------------------------------
//-------------------------------------------------------------------------------------------------

template <typename T, typename keyT = int> 
struct cache_perfect final
{
private:
    size_t capacity_ = 10;
    
    using vectorItr = typename std::vector<keyT>::iterator;

    struct elem_
    {
        T page;
        keyT key;
    };

    std::map<vectorItr, elem_, std::greater<vectorItr>> cache_;
    using cacheItr = typename std::map<vectorItr, elem_>::iterator;

    std::unordered_map<keyT, cacheItr> hash_;
    using hashItr = typename std::unordered_map<keyT, cacheItr>::iterator; 

    vectorItr check_utility_ (std::vector<keyT>& keys, vectorItr check_num_itr)
    {
        keyT key = *check_num_itr;

        vectorItr count_itr = check_num_itr + 1;
        vectorItr end = keys.end();

        for (; count_itr != end; ++count_itr)
        {
            if (key == *count_itr)
                return count_itr;
        }

        return count_itr;
    }

    template <typename F>
    void key_not_found (vectorItr check_num_itr, std::vector<keyT>& keys, F slow_get_page)
    {
        vectorItr num_next_activation = check_utility_ (keys, check_num_itr);
        keyT key = *(check_num_itr);

        if (num_next_activation != keys.end ())
        {
            if (check_full_ ())
            {
                vectorItr num_next_useless_activation = cache_.begin ()->first;
                
                if (num_next_useless_activation > num_next_activation)
                {
                    cacheItr erase = cache_.begin ();
                    cache_.erase (erase);
                    hash_.erase (erase->second.key);
                }
                else
                    return;
            }

            elem_ add_elem {slow_get_page (key), key};
            cache_[num_next_activation] =  add_elem;
            hash_[key] = cache_.find (num_next_activation);
        }
    }

    bool check_full_ ()
    {
        return (cache_.size () == capacity_);
    }
    
public:
    cache_perfect (size_t capacity): capacity_ {capacity} {};
 
    template <typename F>
    int lookup_update (std::vector<keyT>& keys, F slow_get_page)
    {
        vectorItr check_num_itr = keys.begin ();
        vectorItr end_itr = keys.end ();

        int hits = 0;
        while (check_num_itr != end_itr)
        {
            keyT key = *(check_num_itr);
            hashItr hit = hash_.find (key);
                
            if (hit != hash_.end ())
            {               
                vectorItr num_next_activation = check_utility_ (keys, check_num_itr);
                
                cacheItr erase = hit->second;

                elem_ elem = erase->second;
                cache_.erase (erase);
                
                if (num_next_activation != keys.end ())
                {    
                    cache_[num_next_activation] = elem;
                    hash_[key] = cache_.find (num_next_activation);   
                }

                else
                    hash_.erase (key);
                
                hits++;
            }

            else
                key_not_found (check_num_itr, keys, slow_get_page);

            check_num_itr++;
        }

        return hits;
    }
};

//-------------------------------------------------------------------------------------------------
//--------------------------------------cache-type-LRU---------------------------------------------
//-------------------------------------------------------------------------------------------------
template <typename T, typename keyT = int> 
struct cache_lru final
{
private:
    size_t capacity_ = 10;

    std::list<std::pair<T, keyT>> cache_;
    using listItr = typename std::list<std::pair<T, keyT>>::iterator;

    std::unordered_map<keyT, listItr> hash_;
    using hashItr = typename std::unordered_map<keyT, listItr>::iterator;

    void cache_dump_ (void)
    {       
        printf ("in cache: ");
            
    	listItr itr_elem = cache_.begin ();
        listItr itr_end  = cache_.end ();
    	while (itr_elem != itr_end)
    	{
        	printf ("{%d, %d} ", itr_elem->first, itr_elem->second);

        	itr_elem++;
    	}

    	printf ("\n");
    } 

public:
    cache_lru (size_t capacity): capacity_ {capacity} {};
   
    bool check_full () const 
    {
        int csize = cache_.size ();
        assert (csize == hash_.size ()); 

        return (csize == capacity_);
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

            return false;
        }


        listItr page_itr = hash_itr_page->second;
        if (page_itr != cache_.begin ())
            cache_.splice (cache_.begin (), cache_, page_itr);

        return true;
    }
};

};
