#pragma once
#include <mutex>
#include <shared_mutex>
#include <memory>
#include <unordered_map>
#include <functional>
#include <queue>

template<typename K, typename V>
class ThreadSafeCache{
  private:
    std::unordered_map<K,V> cache;
    mutable std::shared_mutex mtx;
  public:
    void set(const K& key, const V& val){
      std::unique_lock<std::shared_mutex> lock(mtx);
      cache[key] = val;
    }
    V get(const K& key) const {
      std::shared_lock<std::shared_mutex> lock(mtx);
      auto ptr = cache.find(key);
      if(ptr == cache.end())return V();
      else return ptr->second;
    }
    void remove(const K& key){
      std::unique_lock<std::shared_mutex> lock(mtx);
      auto ptr = cache.find(key);
      if(ptr != cache.end()){
        cache.erase(ptr);
      }
    }
};