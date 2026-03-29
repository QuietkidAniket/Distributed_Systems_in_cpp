#include <vector>
#include <iostream>
#include <memory>

struct LeakyNode{
  std::string name;

  std::shared_ptr<LeakyNode> neighbour;
  LeakyNode(std::string n): name(n) {
    std::cout << "[Construct] LeakyNode " << name << " created.\n";
  }
  ~LeakyNode(){
    std::cout << "[Destruct] LeakyNode " << name << " destroyed.\n";
  }
};

struct FixedNode{
  std::string name;
  std::weak_ptr<FixedNode> neighbour;
  FixedNode(std::string n) : name(n) { 
    std::cout << "  [Construct] FixedNode " << name << " created.\n"; 
  }

  ~FixedNode() { 
    std::cout << "  [Destruct]  FixedNode " << name << " destroyed (Memory Freed!).\n"; 
  }
};


int main(){

}