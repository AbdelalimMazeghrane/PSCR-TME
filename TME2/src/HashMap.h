#pragma once

#include <vector>
#include <forward_list>
#include <utility>
#include <cstddef>

template<typename K, typename V>
class HashMap {
public:
    // Entry stores a const key and a mutable value
    struct Entry {
        const K key;
        V value;
        Entry(const K& k, const V& v) : key(k), value(v) {}
    };

    using Bucket = std::forward_list<Entry>;
    using Table  = std::vector<Bucket>;

    // Construct with a number of buckets (must be >= 1)
    HashMap(std::size_t nbuckets = 1024) : buckets_(nbuckets), count_(0){}

    // Return pointer to value associated with key, or nullptr if not found.
    // Only iterate the appropriate bucket.
    V* get(const K& key){
        std::size_t index = std::hash<K>{}(key) % buckets_.size();
        Bucket& bucket = buckets_[index];
        
            
        for(Entry& x : bucket){
            if(x.key == key){
                return &x.value;
            }
        }
            
        
        return nullptr;
    }

    // Insert or update (key,value).
    // Returns true if an existing entry was updated, false if a new entry was inserted.
    bool put(const K& key, const V& value){
        size_t index=std::hash<K>{}(key)%buckets_.size();
        Bucket& bucket=buckets_[index];
        for(Entry& x : bucket){
            if(x.key == key){
                x.value=value;
                return true;
            }
        }
        bucket.push_front(Entry(key,value));
        count_++;
        return false;
    }

    // Current number of stored entries
    std::size_t size() const{
        return count_;
    }

    // Convert table contents to a vector of key/value pairs.
    std::vector<std::pair<K,V>> toKeyValuePairs() const{
        std::vector<std::pair<K,V>> v;
        for(size_t i=0;i<buckets_.size();i++){
            if(!buckets_[i].empty()){
                for(const Entry& x : buckets_[i]){
                    v.push_back(std::make_pair(x.key,x.value));
                }
            }
        }
        std::sort(v.begin(), v.end(),
          [](const std::pair<std::string,int>& a,
             const std::pair<std::string,int>& b) {
                return a.second > b.second; // tri décroissant
          });
        return v;
    }

    // Optional: number of buckets
    // std::size_t bucket_count() const;

private:
    Table buckets_;
    std::size_t count_ = 0;
};
