/**
 * @cite : https://www.learncpp.com/cpp-tutorial/introduction-to-smart-pointers-move-semantics/
 * @author : Aniket Kundu 
 * @date : 3rd February, 2026
 * @concept: Introduction to smart pointers and move semantics
 * @brief:
 * We need smart pointer cleaen up for preventing memory leaks in case of
 * early returns or exception throws before delete statement 
 * hence we need the smart pointers
 * Some rules of thumb:
 * . l-values persist after the end of the expression, whereas r-values do not
 * . A reference can be destroyed before the object it is referencing.
 * . The object being referenced can be destroyed before the reference.
 * . When an object being referenced is destroyed before a reference to it,
 *    the reference is left referencing an object that no longer exists. Such a reference is called a dangling reference. 
 *    Accessing a dangling reference leads to undefined behavior.
 * 
 * RAII :  Resource Acquisition is Initialisation
 *    Object's creation and destruction are tied to a resource being 
 *    acquired and released
 * 
 * we can use a class to manage and clean up our pointers
 *    As long as objects of that class were only created as local variables, 
 *    we could guarantee that the class would properly go out of scope 
 *    (regardless of when or how our functions terminate) 
 *    and the owned pointer would get destroyed.
 * 
 * A Smart pointer is a composition class that is designed to 
 *    manage dynamically allocated memory and ensure that memory gets
 *    deleted when the smart pointer object goes out of scope.
 *
 * but the default copy constructor or assignment operator does shallow copies
 * so if the pointer is assigned to another, the deallocation 
 * is not guaranteed for the new pointer
 * 
 * also the same problem exists for the pass by value functions
 * 
 * 
 * The solution to this is move semantics
 *    Move semantics means the class will transfer ownership of the object rather than making a copy.
 *    instead of having our copy constructor and assignment operator copy the pointer (“copy semantics”), 
 *    we instead transfer/move ownership of the pointer from the source to the destination object
 *
 * Still it doesnt solve all the problems
 *  and the auto_ptr class was removed from c++11  
 * 
*/
#include <iostream>

template<typename T> 
class Auto_ptr1{
public:
  T* m_ptr {};
  Auto_ptr1(T* ptr = nullptr) : m_ptr(ptr) {}
  ~Auto_ptr1(){ delete m_ptr; }

  Auto_ptr1(Auto_ptr1& a){ // move semantics
    m_ptr = a.m_ptr;
    a.m_ptr = nullptr;
  }
  // an assigment operator that implements move semantics
  Auto_ptr1& operator=(Auto_ptr1& a){
    if(&a == this)return *this;
    delete m_ptr;
    m_ptr = a.m_ptr;
    a.m_ptr = nullptr;
    return *this;
  }

  T& operator*() const { return *m_ptr; }
  T* operator->() const { return m_ptr; }  
  bool isNull(){ return m_ptr == nullptr; }
};
// Usage:
class Resource{
  public:
  Resource(){
    std::cout << "The resource has been allocated!" << std::endl;
  }
  ~Resource(){
    std::cout << "The resource has been deallocated!" << std::endl;
  }
};


int32_t main(){
  Auto_ptr1<Resource> res(new Resource());
  // do whatever we want
  // at the end the pointer will be deallocated automatically

  return 0;
}
