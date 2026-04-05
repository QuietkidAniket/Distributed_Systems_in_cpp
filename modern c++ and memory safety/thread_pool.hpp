#pragma once
#include <thread>
#include <mutex>
#include <functional>
#include <vector>
#include <queue>

class ThreadPool {
   private:
    std::vector<std::thread> workers;         // The actual threads
    std::queue<std::function<void()>> tasks;  // The queue of tasks

    std::mutex queue_mutex;             // Protects the queue
    std::condition_variable condition;  // Wakes up sleeping threads
    bool stop = false;                  // Flag to shut down the pool

   public:
    // Constructor: Spin up 'N' threads
    ThreadPool(size_t num_threads) {
        for (size_t i = 0; i < num_threads; ++i) {
            workers.emplace_back([this] {
                while (true) {
                    std::function<void()> task;

                    // Scope block for the mutex lock
                    {
                        std::unique_lock<std::mutex> lock(this->queue_mutex);

                        // Wait until there is a task OR the pool is stopping
                        this->condition.wait(lock, [this] {
                            return this->stop || !this->tasks.empty();
                        });

                        // If stopping and no tasks left, exit the thread loop
                        if (this->stop && this->tasks.empty()) {
                            return;
                        }

                        // Grab the task
                        task = std::move(this->tasks.front());
                        this->tasks.pop();
                    }
                    // Lock is released here

                    // Execute the task outside the lock so other threads aren't blocked
                    task();
                }
            });
        }
    }

    // Add a new task to the queue
    void enqueue(std::function<void()> task) {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            tasks.push(std::move(task));
        }
        condition.notify_one();  // Wake up one sleeping worker thread
    }

    // Destructor: Clean up threads when the pool is destroyed
    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            stop = true;
        }
        condition.notify_all();  // Wake up ALL threads so they can finish and exit

        for (std::thread& worker : workers) {
            worker.join();  // Wait for all threads to finish
        }
    }
};

// // --- Example Usage ---
// int main() {
//     std::cout << "Starting Thread Pool with 4 threads...\n";
//     ThreadPool pool(4);

//     // Throw 10 tasks at the pool
//     for(int i = 1; i <= 10; ++i) {
//         pool.enqueue([i] {
//             std::cout << "Task " << i << " is executing on thread "
//                       << std::this_thread::get_id() << "\n";

//             // Simulate some work taking 1 second
//             std::this_thread::sleep_for(std::chrono::seconds(1));
//         });
//     }

//     std::cout << "All tasks enqueued. Main thread waiting...\n";

//     // Give the pool time to finish before main() exits
//     std::this_thread::sleep_for(std::chrono::seconds(4));
//     std::cout << "Done.\n";

//     return 0;
// }
