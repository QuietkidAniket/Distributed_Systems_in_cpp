#include <iostream>
#include <queue>
#include <memory>
#include <functional>
#include <thread>

template<typename T>
class BlockQueue{
  private:
    std::queue<T> q;
    std::mutex mtx;
    std::condition_variable cv;
    bool stop = false;
  public:
    void push(T item){
      {
        std::lock_guard<std::mutex> lock(mtx);
        q.push(std::move(item));
      }
      cv.notify_one();
    }
    
    std::optional<T> pop(){
      std::lock_guard<std::mutex> lock(mtx);
      cv.wait(lock, [this](){
        return !q.empty() || stop;
      });
      if(q.empty() && stop)return std::nullopt;
      T item = q.front();
      q.pop();
      return item;
    }
    
    void abort(){
      {
        std::lock_guard<std::mutex> lock(mtx);
        stop = true;
      }
      cv.notify_all();
    }

};