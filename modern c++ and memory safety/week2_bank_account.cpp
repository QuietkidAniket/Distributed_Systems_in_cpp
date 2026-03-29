#include <mutex>
#include <thread>
#include <iostream>
#include <functional>

class BankAccount{
  public:
  std::mutex mtx;
  int bal;
  BankAccount(int b) : bal(b) {}
};

void safeTransfer(BankAccount& from, BankAccount& to, int val){
  std::scoped_lock(from.mtx, to.mtx);
  from.bal -= val;
  to.bal += val;
}

int main(){
  BankAccount acc1(100);
  BankAccount acc2(100);

  std::jthread t1(safeTransfer, std::ref(acc1), std::ref(acc2), 10);
  std::jthread t2(safeTransfer, std::ref(acc2), std::ref(acc1), 20);
  std::cout <<"Account 1 balance : " << acc1.bal <<std::endl;
  std::cout <<"Account 2 balance : " << acc2.bal <<std::endl;
  return 0;
}