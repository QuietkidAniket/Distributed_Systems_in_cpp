#include <memory>
#include <unordered_map>
#include <iostream>
#include <mutex>
#include <functional>

/**
 * Motive : To practice Modern C++ conventions and Deoxygen styled Comments 
 * @file LRU_with_thread_safety.cpp
 * @author Aniket Kundu
 * @brief A thread-safe LRU (Least Recently Used) Cache Implementation
 * @version 0.1
 * @date 2026-03-28
*/

template<typename T>
class Node{
  public:
  int key;
  T val;
  Node<T> *prev, *next;
  Node(int k , T v): key(k), val(v), prev(nullptr), next(nullptr) {}
};

/**
 * @brief A Least Recently Used (LRU) Cache.
 * Stores key-value pairs with a fixed capacity. 
 * When capacity is reached,
 * the least recently accessed item is evicted.
 * @tparam T The type of data to be stored in the cache.
 * @note Time complexity: Get and Put operations are O(1) average.
 */
template<typename T>
class LRU{
  private:
    int capacity;
    std::unordered_map<int, std::unique_ptr<Node<T>>> cache;
    std::unique_ptr<Node<T>> head, tail;

    mutable std::mutex mtx;

    void removeNode(Node<T>* node){           // remove the node from the Linked list
      node->prev->next = node->next;
      node->next->prev = node->prev;
    }
    void addToTail(Node<T>* node){            // add just before tail in Linked List
      node->next = tail.get();
      node->prev= tail->prev;
      tail->prev->next = node;
      tail->prev = node;
    }
  public:
    LRU(int cap) : 
      capacity(cap) 
    {                                     // default constructor
      tail = std::make_unique<Node<T>>(0, T());
      head = std::make_unique<Node<T>>(0, T());
      tail->prev = head.get();
      head->next = tail.get();
    }
    // no destructor is needed as unique_ptr is used
    
    LRU(const LRU<T>& other): 
    capacity(other.capacity)
    {                                     // copy constructor
      tail = std::make_unique<Node<T>>(0, T());
      head = std::make_unique<Node<T>>(0, T());

      head->next =  tail.get();
      tail->prev =  head.get();

      // creation lock
      std::lock_guard<std::mutex> lock(other.mtx);
      Node<T>* cur = other.head->next;
      while(cur != other.tail.get()){
        this->put(cur->key, cur->val);
        cur = cur->next;
      }
      // creation unlock
    }
    LRU(LRU<T>&& other) noexcept : capacity(0) 
    {                                        // move constructor
      // nullify the input object raw pointers
      std::lock_guard<std::mutex> lock(other.mtx);
      tail = std::make_unique<Node<T>>(0, T());
      head = std::make_unique<Node<T>>(0, T());

      head->next =  tail.get();
      tail->prev =  head.get();

      swap(other);
      // creation unlock
    }
    
    /**
     * @brief Unified Assignment Operator
     * though we pass by value, the compiler first creates a local instance for "other"
     * just prior to entry into the function. If rvalue is involved, 
     * then the compiler automatically makes a call to the move constructor
     * , same goes for lvalue - copy constructor is called
     * @note This is the golden standard followed by industry pros
     *  no longer the need for manual garbage value handling, and exception safe
     */
    LRU& operator=(LRU<T> other){ // smart compiler targets the respective constructors
      // creation lock
      std::lock_guard<std::mutex> lock(mtx);
      this->swap(other);
      return *this;
      // creation unlock
    }

    /**
     * @brief a helper for swapping the values in case of copy and move
     */
    void swap(LRU<T>& other) noexcept { 
      std::swap(capacity, other.capacity);
      std::swap(cache, other.cache);
      std::swap(head, other.head);
      std::swap(tail, other.tail);
    }
  
    /**
     * @brief Puts a key-value pair into the cache
     * If the key exists, the value is updated,
     * and the entry is brought to the "Most Recently Used" (MRU) position.
     * If the key doesnt exist, a new key-value pair is made and inserted 
     * at the MRU position,
     * and if capacity is being exceeded then the LRU element is removed
     * to accomodate the incoming new key-value pair
     * @param key The unique identifier for the cached item.
     * @param val The value of the cached item.
     * @return void 
     */
    void put(int key, T val){
      std::lock_guard<std::mutex> lock(mtx);
      if(cache.find(key) != cache.end()){
        Node<T>* node = cache[key].get();
        node->val = val;
        removeNode(node);
        addToTail(node);
      }else{
        //lock 
        if(capacity <= cache.size()){             // remove the least recently used if capacity is exceeded
          Node<T>* lru = head->next;
          removeNode(lru);            // removes the lru from linked list
          cache.erase(lru->key);      // automatically deletes lru, as unique_ptr
        }
        auto newnode = std::make_unique<Node<T>>(key, val);
        Node<T>* rawptr  = newnode.get(); // always secure the raw pointer separate using get()
        cache[key] = std::move(newnode); 
        addToTail(rawptr);     // add the raw pointer to linked list
      }
    }

    /**
     * @brief Retrieves a value from the cache.
     * If the key exists, it is moved to the "Most Recently Used" position.
     * @param key The unique identifier for the cached item.
     * @return T The cached value, or a default-constructed T if not found.
     */
    T get(int key){ 
      // lock
      std::lock_guard<std::mutex> lock(mtx);
      if(cache.find(key) != cache.end()){
        auto node = cache[key].get();
        removeNode(node);
        addToTail(node);
        return node->val;
      }else{
        return T();
      }
      // unlock
    }

    void display(){
      // lock
      std::lock_guard<std::mutex> lock(mtx);
      Node<T>* cur = head->next;
      while(cur != tail.get()){
        std::cout << "[" << cur->key <<", "<<cur->val<<"]\n";
        cur = cur->next;
      }
      std::cout<<std::endl;
      // unlock
    }
};


int main(){
  LRU<std::string> obj(3);
  obj.put(1, "First Entry");
  obj.put(2, "Second Entry");
  obj.put(3, "Third Entry");
  obj.display();
  std::cout << obj.get(2) << std::endl;
  obj.display();
  obj.put(4, "Fourth Entry");
  obj.display();
  std::cout << obj.get(3) << std::endl;
  obj.display();
}