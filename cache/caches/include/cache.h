#ifndef CACHE_HPP
#define CACHE_HPP

#include <iostream>
#include <list>
#include <vector>
#include <map>
#include <unordered_map>
#include <cassert>
#include <cstdio>
#include <functional>
#include <climits>
#include <set>

namespace caches
{
    
static const int START_FREQ = 1;

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

template <typename T, typename keyT = int> 
struct cache_perfect final
{
private:
    size_t capacity_ = 10;

    struct elem_t
    {
        T page;
        keyT key;
    };

    struct comparator
    {
        bool operator () (const std::pair<unsigned, elem_t>& a, const std::pair<unsigned, elem_t>& b) const
        {
            return (a.first > b.first);
        }
    };
    
    std::multimap<unsigned, elem_t, std::greater<unsigned>> cache_;
    using cacheItr = typename std::multimap<unsigned, elem_t, std::greater<unsigned>>::iterator;
    
    std::unordered_map<keyT, cacheItr> hash_;

    std::unordered_map<keyT, unsigned> prev_use_hash_;
    using prev_use_hashItr = typename std::unordered_map<keyT, unsigned>::iterator; 

    std::vector<unsigned> first_pass_keys_ (std::vector<keyT>& keys)
    {
        std::vector<unsigned> freq_use (keys.size (), UINT_MAX);

        for (unsigned num_key = 0, size = keys.size (); num_key != size; ++num_key)
        {
            keyT key = keys.at(num_key);
            prev_use_hashItr check_prev = prev_use_hash_.find (key);

            if (check_prev != prev_use_hash_.end ())
            {
                unsigned num_prev_use = check_prev->second;
            
                freq_use.at (num_prev_use) = num_key - num_prev_use;
            }

            prev_use_hash_[key] = num_key;
        }

        return freq_use;
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
        std::vector<unsigned> to_next_use = first_pass_keys_ (keys);

        int hits = 0;
        for (unsigned num_key = 0, size = keys.size (); num_key != size; ++num_key)
        {
            keyT key = keys.at (num_key);
           
            auto hit = hash_.find (key);
            if (hit == hash_.end ())
            {                   
                unsigned to_next = to_next_use.at (num_key);
                
                if (check_full_ ())
                {
                    cacheItr unusable_key_itr = cache_.begin ();
                    if (unusable_key_itr->first < to_next)
                        continue;

                    keyT unusable_key = unusable_key_itr->second.key;
                    
                    cache_.erase (unusable_key_itr);
                    hash_.erase (unusable_key);
                }

                elem_t elem {slow_get_page (key), key};
                
                auto res = cache_.insert ({to_next, elem});
                hash_.insert ({key, res});
            }

            else
            {
                cacheItr elem_itr = hit->second; 

                auto node_handle = cache_.extract (elem_itr);
                node_handle.key () = to_next_use.at (num_key);
                
                auto res = cache_.insert (std::move (node_handle));
                hash_[key] = res;
               
                hits++;
            }
        }

        return hits;
    }
};

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

#endif
