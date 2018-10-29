#include <iostream>
#include "poly/poly.hpp"
#include "test/tracker.hpp"
#include "test/memory.hpp"

struct Storable
  : decltype( copy() + move() + destroy() + storage() )
{  };

struct A{
  A() = default;
  A( A&& ) { throw std::logic_error("A move constructor throw");  }
  A( const A& ){  }
};

int main()
{
  Tracker tracker;
  {
    std::cout << std::boolalpha;
    Poly< RemoteVT<Storable> > t{ in_place<Guard<A>>(), tracker };
    Poly< RemoteVT<Storable> > t2{ in_place<Guard<A>>(), tracker};
    try{
      t = std::move(t2);
    } catch (std::logic_error e){    }
    // assert( t.valueless_by_exception() );
    bool thrown = false;
    try{
      t.call<storage>();
    } catch ( const invalid_vtable_call& e ){
      thrown = true;
    }
    assert( thrown );
    static_assert( !std::is_nothrow_move_constructible<decltype(t)>::value,
    		   "Poly falsly assumed to be not nothrow copy constructible" );
    static_assert( !std::is_nothrow_copy_constructible<decltype(t)>::value,
    		   "Poly falsly assumed to be not nothrow copy constructible" );

    Poly< RemoteVT<Storable> > t3{ t };
  }
  assert( tracker.objects.count() == 0 );
  
  test_memory();
  std::cout << "passed" << std::endl;
									 
  return 0;
}
