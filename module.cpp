
#include <iostream>
#include <typeinfo>
#include "test.hpp"
#include "poly/poly.hpp"
#include "poly/allocator.hpp"

template< typename ... >
struct print_ts;


struct A{ ~A(){ std::cout << "~A():" << this << std::endl; } };

void invoke( print, const A& a ){
  std::cout << "A:" << &a  << std::endl;
};


int main()
{

  {
    using p = decltype( print() + destroy() +  copy() + move() + ::storage() );
    Poly< RemoteVT< p > > Int{ in_place<int>(), 33};
    Int.call<print>();
    Poly< RemoteVT< p >, StackAllocator<1> > Ant{in_place<A>()};
    Ant.call<print>();
    Poly< RemoteVT< p >, StackAllocator<1> > Ant2 = Ant;
    Ant2.call<print>();
    Poly< RemoteVT< p >, StackAllocator<1> > Ant3 = std::move(Ant);
    Ant3.call<print>();    
    
    std::cout << "end" << std::endl;
  }
  
  test_view();
  return 0;
}
