# C++ Distributed Systems Engineering

This repository documents my journey into a High-Performance Distributed Systems Engineering world. 

The goal of this curriculum is to stop writing code that looks like a contest submission and start writing safe, readable, and leak-proof systems code. The theoretical content and architectural concepts within this repository have been curated and synthesized from outstanding systems engineering books (like *Effective Modern C++*, *C++ Concurrency in Action*, and *Linux System Programming*) and trusted industry resources.

---

## Course Material & Theory

The theoretical foundation of this journey is split into four comprehensive documents:

* 📄 **[Phase 1: Modern C++ and Memory Safety](./Phase%201_%20Modern%20C%2B%2B%20and%20Memory%20Safety%20.pdf)** - De-programming "CP C++", RAII, Smart Pointers, and Move Semantics.
* 📄 **[Phase 2: Concurrency & Multithreading](./Phase%202_%20Concurrency%20&%20Multithreading.pdf)** - Thread management, Mutexes, Condition Variables, and Atomics.
* 📄 **[Phase 3: Linux Systems & Networking](./Phase%203_%20Linux%20Systems%20&%20Networking.pdf)** - File Descriptors, Socket Programming, and I/O Multiplexing (`epoll`/`poll`).
* 📄 **[Phase 4: Project - Multi-threaded Redis](./Phase%204_%20Project_%20Multi-threaded%20Redis.pdf)** - The capstone project integrating memory management, concurrency, and networking.

---

## The Distributed Systems Roadmap

### Phase 1: De-programming "CP C++" (Modern C++ & Memory Safety)
**Philosophy:** In systems interviews, using raw `new`/`delete` or `#define int long long` is an instant red flag. We use RAII to guarantee cleanup.

* **Week 1: The "No Raw Pointers" Policy:** Mastering `unique_ptr` (exclusive ownership), `shared_ptr` (reference counting), and `weak_ptr` (breaking cycles).
* **Week 2: The "Performance" Secret (Move Semantics):** Understanding R-value references (`&&`), Move Constructors, and `std::move` vs `std::forward` to avoid deep copying massive objects.
* **Week 3: Modern Error Handling & Lambdas:** Using `std::optional` and `std::variant` instead of magic numbers. Mastering capture lists (`[&]`, `[=]`) and `std::function` for async callbacks.
* **Week 4: Professional Build Tools:** Moving away from single-file compilation. Understanding CMake, Header Files (`.hpp`), Include Guards, and proper Namespace scoping.
* *Micro-Task:* Implement a custom `SmartPointer<T>` mimicking `std::unique_ptr` with template generics and move constructors.

### Phase 2: Concurrency & Multithreading
**Goal:** The heart of backend engineering. Handling 10k concurrent requests without crashing.

* **Thread Management & Data Races:** `std::thread`, `join()` vs `detach()`, `std::mutex`, `std::lock_guard`, and `std::unique_lock`.
* **Synchronization:** Using `std::condition_variable` to solve the Producer-Consumer problem without polling loops.
* **Atomics:** `std::atomic`, Compare-and-Swap (CAS), and Memory Ordering (Release-Acquire) for lock-free data structures.
* *Micro-Task:* Write a Thread-Safe Bounded Blocking Queue using condition variables.

### Phase 3: Linux Systems & Networking
**Goal:** Move from "C++ logic" to "talking to the OS".

* **File Descriptors:** Everything in Linux is a file (`read`, `write`, `open`, `close`).
* **Socket Programming:** `socket()`, `bind()`, `listen()`, `accept()`, and the TCP Handshake.
* **I/O Multiplexing:** Understanding why `epoll()` / `poll()` is superior to `select()` for handling 100k connections.
* **Serialization:** Packing binary data or using Protocol Buffers to send C++ objects over the network.
* *Micro-Task:* Write a High-Concurrency Chat Server using `epoll`/`poll`.

### Phase 4: The Capstone Project ("Mini-Redis")
**Goal:** Build a High-Performance Key-Value Store combining Phase 1, 2, and 3.

* **Week 1:** Single-threaded server parsing simple text commands (`SET`, `GET`).
* **Week 2:** Rewrite the network layer using the Event Loop pattern for non-blocking I/O.
* **Week 3:** Integrate a Worker Thread Pool to process requests asynchronously (Reactor Pattern).
* **Week 4:** Add Disk Persistence (snapshotting) so data survives server restarts.

### Phase 5: Interview Prep & System Design
* **Low-Level Design (LLD):** Designing thread-safe classes (e.g., an asynchronous logging library).
* **High-Level Design (HLD):** Load Balancers, Consistent Hashing, CAP Theorem, Database Sharding.

---

## Source Code Repository

The code in this repository is strictly compiled using production-grade flags:
`g++ -Wall -Wextra -Werror -fsanitize=address`

### Core Data Structures & Concurrency (Phases 1 & 2)
* [`week1a.cpp`](./week1a.cpp) -> Foundational drills on memory management.
* [`week1b.cpp`](./week1b.cpp) -> Smart pointer fundamentals.
* [`week1c.cpp`](./week1c.cpp) -> RAII implementation examples.
* [`week1d.cpp`](./week1d.cpp) -> Move semantics and R-value references.
* [`graph_cycle_breaker.cpp`](./graph_cycle_breaker.cpp) -> Week 1 Assignment: Solving circular dependency memory leaks using `std::weak_ptr`.
* [`week2_bank_account.cpp`](./week2_bank_account.cpp) -> Demonstrating data races and mutex locking.
* [`blocking_queue.hpp`](./blocking_queue.hpp) -> A thread-safe Producer-Consumer queue using `std::condition_variable`.
* [`thread_pool.hpp`](./thread_pool.hpp) -> Custom implementation of a worker thread pool.
* [`ThreadSafeCache.hpp`](./ThreadSafeCache.hpp) -> Production-grade Read-Write lock mechanism using `std::shared_mutex`.
* [`LRU_cache_thread_unsafe.cpp`](./LRU_cache_thread_unsafe.cpp) -> Baseline LRU Cache implementation.
* [`LRU_cache_thread_safe.cpp`](./LRU_cache_thread_safe.cpp) -> Concurrent LRU Cache protected by fine-grained locking.

### Networking & Distributed Systems (Phases 3 & 4)
* [`multithreaded_tcp_echo_server.cpp`](./multithreaded_tcp_echo_server.cpp) -> Baseline multi-threaded socket server.
* [`high_concurrency_chat_room.cpp`](./high_concurrency_chat_room.cpp) -> Phase 3 Micro-Task: I/O Multiplexing event loop.
* [`HTTP_router.cpp`](./HTTP_router.cpp) -> Foundational HTTP parsing and routing logic.
* [`customer.cpp`](./customer.cpp) -> Client-side endpoint interaction logic.
* [`distributed_cache.cpp`](./distributed_cache.cpp) -> Networked wrapper for the thread-safe cache.
* [`redis.cpp`](./redis.cpp) -> Phase 4 (Week 1): Single-threaded Key-Value store baseline.
* [`multithreaded_redis.cpp`](./multithreaded_redis.cpp) -> **Phase 4 Capstone:** The complete Reactor Pattern implementation. An I/O multiplexed main thread handing off `GET`/`SET` workloads to a Thread Pool interacting with a `std::shared_mutex` protected cache.
