#include <iostream>
#include <vector>

class Cust{
  private:
    std::string first;
    std::string last;
    int         val;
  public:
    /**
     * NOTE: 
     * explicit-> disallows implicit coversions
     *          requires that the user makes the conversions by themselves 
     *          rather than automatically by the compiler
     * initializer list based initialisation works even with explicit
     * unlike copy list based initialisation
     * hence list based init. is safer
     * i.e. prefer vector<int> a{1,2,3,4,5}; over vector<int> a = {1,2,3,4,5};
    */ 
    explicit Cust(const std::string& f, const std::string& l, int v):
    first(f), last(l), val(v) {

    }
    explicit Cust(std::string&& f, std::string&& l, int v):
    first(std::move(f)), last(std::move(l)), val(v) {

    }
    explicit Cust(): first(""), last(""), val(0){

    }
  };
void foo (Cust& obj){           // write access
  std::cout << "pass by reference, can bind to only non-const named objects" << std::endl;
}
void foo (const Cust& obj){     // read-only access
  std::cout << "pass by reference, can bind to anything " << std::endl;
}
void foo (Cust&& obj){          // move access
  std::cout << "move by reference, can bind to objects without names (rvalues) or with std::move()" << std::endl;
}

class new_vector{
  public:
  /** @concept: demonstration of perfect forwarding */
  template<typename... Args>
  void emplace_back(Args&&... args){
    // place in memory ( T(std::forward<Args>(args) ...) );
  }
};

const std::string& returnConstRefString(){
  return "This is the const string reference";
}
const std::string& returntmpString(){
  return "This is the temporary string";
}
int32_t main(){
  std::vector<Cust> cv;
  cv.push_back(Cust{"Art", "Lee", 42}); // implicitly move semantics is generated
  std::string first{"Jii"}, last{"Cook"};
  cv.emplace_back(std::move(first), last, 39); // implicitly move semantics is generated

  std::string s{"some lvalue"};
  // universal/forwarding reference 
  auto&& a = s;

  // but wont work for auto&, as cannot bind non-const lvalue to rvalue
  auto&& b = returnConstRefString(); 
  const auto& b2 = returnConstRefString();

  auto&& c = returntmpString();

  /* Demonstration of rvalue references */
  const Cust customer1{"Aniket", "Kundu", 21};
  foo(customer1);                     // calls the const pass by reference foobar
  foo(Cust{"John", "Doe", 40});       // calls the move semantics based foobar
  foo(std::move(customer1));
}