#include <iostream>
#include "poly/poly.hpp"
#include "test/tracker.hpp"
#include "test/memory.hpp"

struct Storable
  : decltype( copy() + move() + destroy() + storage() )
{  };

struct A{
  A() = default;
  A( A&& ) { throw std::logic_error("throw");  }
  A( const A& ){  }
};

int main()
{
  Tracker tracker;
  // TODO : Obsluga wyjatkow konstruktorow
  // TODO : destruktor test poly
  {
    std::cout << std::boolalpha;
    Poly< RemoteVT<Storable> > t{ in_place<Guard<A>>(), tracker };
    Poly< RemoteVT<Storable> > t2{ in_place<Guard<A>>(), tracker};
    try{
      t = std::move(t2);
    } catch (std::logic_error e){ }
    static_assert( !std::is_nothrow_move_constructible<decltype(t)>::value,
		   "Poly falsly assumed to be not nothrow constructible" );
  }
  assert( tracker.objects.count() == 0 );
  
  test_memory();
  std::cout << "passed" << std::endl;
									 
  return 0;
}
