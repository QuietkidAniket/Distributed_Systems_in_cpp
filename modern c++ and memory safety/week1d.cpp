/**
  std::std::shared_ptr: Cooperative Ownership

  Concept: std::std::shared_ptr represents shared ownership. 
  It says, "I own this object, but others might own it too."

  The object is destroyed only when the last std::shared_ptr pointing to it is destroyed or reset.

  Copying is allowed: When you copy a std::shared_ptr, a "reference count" (ref count) is incremented.

  Destruction: When a std::shared_ptr dies, it decrements the ref count. If the count hits 0, the object is deleted.

  Performance Cost: Unlike unique_ptr, std::shared_ptr has overhead:

  Control Block: It allocates a separate "Control Block" on the heap to store the reference count.

  Atomic Operations: Incrementing/decrementing the ref count is thread-safe (atomic), which is slightly slower than normal arithmetic.

  Key Operations:
    std::std::make_shared<T>(args): Crucial. Allocates the object AND the control block in one memory chunk (better cache locality and performance than new).  
    ptr.use_count(): Returns the number of owners (mostly for debugging).
*/


#include <iostream>

struct SharedFile {
    std::string name;
    
    SharedFile(std::string n) : name(n) {
        std::cout << "  [OPEN] File '" << name << "' opened.\n";
    }
    
    ~SharedFile() {
        std::cout << "  [CLOSE] File '" << name << "' closed (Ref Count hit 0).\n";
    }
};

int32_t main(){
    // process 1 creates the file
    std::cout << "--- Create Shared Ptr (P1) ---\n";
    std::shared_ptr<SharedFile> p1 = std::make_shared<SharedFile>("config.json");
    std::cout << "Ref Count: " << p1.use_count() << "\n";

    {
        std::cout << "\n--- P2 joins and shares ownership ---\n";
        std::shared_ptr<SharedFile> p2 = p1; // Copy allowed! Ref count increases.
        std::cout << "Ref Count: " << p1.use_count() << "\n";
        
        std::cout << "\n--- P3 joins ---\n";
        std::shared_ptr<SharedFile> p3 = p1;
        std::cout << "Ref Count: " << p1.use_count() << "\n";
        
        std::cout << "\n--- P2 and P3 die (End of inner scope) ---\n";
    } // p2 and p3 go out of scope here.

    std::cout << "Ref Count: " << p1.use_count() << "\n";
    
    std::cout << "\n--- P1 dies (End of Function) ---\n";
} // p1 dies, count -> 0, file closed.
