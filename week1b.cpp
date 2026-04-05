/**
 * @cite    : https://www.learncpp.com/cpp-tutorial/rvalue-references/
 * @author  : Aniket Kundu
 * @date    : 3rd February
 * @concept: R-values
 * @brief:
 * l-value reference      can be initialized with         can modify
 *   modifiable l-value           yes                         yes
 *   non-modifiable l-value       no                          no
 *    r-value                     no                          no
 * 
 * l-value reference to const     can be initialized with       can modify
 *    modifable l-value                   yes                       no
 *    non-modifable l-value               yes                       no
 *      r-value                           yes                       no
 * (helps in passing r-values or l-values directly into functions without making copies)
 * 
 * 
 * R-value references:
 * int&& res {5};
 * 
 * r-value reference      can be initialized with         can modify
 *   modifiable l-value           no                         no
 *   non-modifiable l-value       no                         no
 *    r-value                     yes                        yes
 * 
 * r-value reference to const     can be initialized with       can modify
 *    modifable l-value                   no                       no
 *    non-modifable l-value               no                       no
 *      r-value                           yes                      no
 *
 * 
 * 1. The Core Mechanics: Lvalues vs. Rvalues
    To understand Move, you must understand value categories:

    Lvalue (Left Value): An object that has a name and persists beyond a single expression (e.g., vector<int> a;). You cannot safely steal from it because the user might use a later.

    Rvalue (Right Value): A temporary object that does not have a name or is about to be destroyed (e.g., vector<int>{1, 2, 3} or the return value of a function). You can safely steal from it because nobody else can reference it.

    2. What is std::move?
    std::move is a lie. It does not move anything. It does not generate any executable code that moves data.

    It is strictly a cast. It takes an Lvalue (something you can't steal from) and casts it to an Rvalue Reference (T&&), effectively telling the compiler: "I promise I won't use this object again. You have my permission to plunder its resources."
  NOTES:
  Important Notes & Facts:
    . The noexcept Rule: Always mark your move constructors noexcept. If std::vector needs to resize, it will move elements to the new memory block only if the move constructor is noexcept. If it's not marked, std::vector will fall back to Copying everything to guarantee exception safety (Strong Exception Guarantee).

    . Valid but Unspecified State: After std::move(x), x is not necessarily "empty." It is in a "valid but unspecified" state. You can destroy it, or you can assign a new value to it, but you should not read its current value.

    . const blocks Move: If you have const string s = "hello";, std::move(s) will not move. It will cast it to const string&&. Since a move constructor requires a modifiable reference (string&&) to "steal" and nullify the source pointers, it cannot bind to const. It will silently fall back to the Copy Constructor.

    . The Rule of Five: If you implement one of the following, you likely need to implement all five to handle resource management correctly:
        Destructor
        Copy Constructor
        Copy Assignment Operator
        Move Constructor
        Move Assignment Operator

    . Small String Optimization (SSO): For small strings (usually < 15 or 22 chars), std::string stores data directly in the handle, not on the heap. Moving a small string acts like a copy (copying bytes) because there is no heap pointer to steal. Move semantics shines primarily with heap-allocated resources.
*/
#include <iostream>

class Buffer {
public:
    int* data;
    int size;

    // 1. Constructor
    Buffer(int n) {
        size = n;
        data = new int[n];
        for(int i=0; i<n; i++) data[i] = i * 10;
        std::cout << "[Default Construct] allocated " << size << " ints.\n";
    }

    // 2. Destructor
    ~Buffer() {
        if (data != nullptr) {
            std::cout << "[Destruct] freeing memory.\n";
            delete[] data;
        } else {
            std::cout << "[Destruct] nothing to free (was moved).\n";
        }
    }

    // 3. COPY Constructor (Deep Copy - Slow)
    Buffer(const Buffer& other) {
        size = other.size;
        data = new int[size]; // ALLOCATION! Heavy operation.
        memcpy(data, other.data, size * sizeof(int));
        std::cout << "[COPY] Deep copied data.\n";
    }

    // 4. MOVE Constructor (Shallow Copy - Fast)
    // Note the 'noexcept'. This is critical for STL containers to use move.
    Buffer(Buffer&& other) noexcept {
        // STEAL the resource
        data = other.data;
        size = other.size;

        // NULLIFY the source so its destructor doesn't kill the data we just stole
        other.data = nullptr; 
        other.size = 0;

        std::cout << "[MOVE] Stole pointer. Source is now empty.\n";
    }
};

int32_t main(){
    std::cout << "--- Create A ---\n";
    Buffer a(5);

    std::cout << "\n--- Copy A to B ---\n";
    // Calls Copy Constructor because 'a' is an lvalue
    Buffer b = a; 

    std::cout << "\n--- Move A to C ---\n";
    // std::move(a) casts 'a' to 'Buffer&&' (rvalue reference).
    // This triggers the Move Constructor.
    Buffer c = std::move(a);

    std::cout << "\n--- End of Scope ---\n";
}
