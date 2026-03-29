/**
  std::unique_ptr: The Sole Owner
    Concept: std::unique_ptr represents exclusive ownership.
    It is a smart pointer that says, "I own this object, and no one else does."

    When the unique_ptr goes out of scope, it automatically deletes the object it points to.

    Copying is banned: You cannot copy a unique_ptr (because two things cannot uniquely own the same object).

    Moving is allowed: You can transfer ownership using std::move.

    Performance: It has practically zero overhead compared to a raw pointer. It is just a raw pointer wrapper with a destructor.

    Key Operations:
      . std::make_unique<T>(args): The safest way to create one.
      . ptr.get(): Access the raw pointer (for observing, not owning).
      . ptr.release(): Give up ownership (returns the raw pointer, doesn't destroy it).
      . ptr.reset(): Destroys the current object and optionally takes a new one.

    How to use in lambda captures:
    
    auto task_lambda = [captured_req = std::move(req)]() {
        std::cout << "    (Inside Lambda) I own the object now.\n";
        captured_req->process();
    };
 */

#include <iostream>

struct Task {
    int id;
    std::string name;
    
    Task(int i, std::string n) : id(i), name(n) {
        std::cout << "[Construct] Task " << id << " (" << name << ") created.\n";
    }
    
    ~Task() {
        std::cout << "[Destruct] Task " << id << " destroyed.\n";
    }
    
    void print() {
        std::cout << " -> Working on: " << name << "\n";
    }
};

// Function taking ownership (Sink)
// The std::std::unique_ptr is passed by value (move required), consuming the caller's pointer.
void consume_task(std::unique_ptr<Task> t) {
    std::cout << "Inside worker function:\n";
    t->print();
    // 't' dies here, destroying the Task
}

int32_t main(){
    int n; 
    std::cin >> n; // Number of tasks
    
    // std::vector of std::unique_ptrs. 
    // Commonly used for polymorphic collections or managing hefty resources.
    std::vector<std::unique_ptr<Task>> vec;
    
    for(int i = 0; i < n; i++) {
        std::string s; std::cin >> s;
        // make_unique is cleaner than 'new'
        vec.push_back(std::make_unique<Task>(i+1, s));
    }

    std::cout << "\n--- Transferring Ownership ---\n";
    
    // We want to process the first task.
    // std::unique_ptr<Task> copy = vec[0]; // ERROR: Call to implicitly-deleted copy constructor
    
    // We must MOVE it. vec[0] becomes null (empty).
    std::unique_ptr<Task> tmp = std::move(vec[0]);
    
    if(vec[0] == nullptr) {
        std::cout << "std::std::vector index 0 is now empty (moved from).\n";
    }
    
    std::cout << "\n--- Passing to Function ---\n";
    // Moving 'tmp' into the function. 'tmp' loses ownership.
    consume_task(std::move(tmp));
    
    if(tmp == nullptr) {
        std::cout << "tmp variable is now empty (moved into function).\n";
    }

    std::cout << "\n--- Cleaning up remaining std::vector ---\n";
    // The std::vector destructor will destroy all remaining std::unique_ptrs automatically.
}
