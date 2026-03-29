/** 
  std::weak_ptr: The Observer
    Concept: std::weak_ptr is a "non-owning" reference to an object managed by shared_ptr.

    It does not increase the reference count.

    It is used to break Circular Dependencies (The "Deadly Embrace").

    You cannot access the object directly. You must convert it to a shared_ptr (using .lock()) to check if the object still exists.

    The Problem: Cyclic Dependency If Object A has a shared_ptr to B, and Object B has a shared_ptr to A, their reference counts will never reach zero. They will keep each other alive forever, causing a memory leak.

    The Solution: Make one of the pointers a weak_ptr.

    weak_ptr Example (Breaking Cycles)
    We will create a graph node structure. A Parent owns Children, but Children only observe their Parent (to avoid a cycle).
*/

