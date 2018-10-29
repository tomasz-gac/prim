#include <iostream>
#include "poly/poly.hpp"
#include "test/tracker.hpp"
#include "test/memory.hpp"

struct Storable
  : decltype( copy() + move() + destroy() + storage() )
{  };

struct A{
  A() = default;
  A( int ) try
    : A( A())
    {  }
  catch( ... ){
    std::cout << "A catch" << std::endl;
    throw;
  }
  A( A&& ) { throw std::logic_error("throw");  }
  A( const A& ){  }
};

struct B : A{
  B() try
    : A(1){
      std::cout << "B ok" << std::endl;
    } catch (...) { std::cout << "B catch" << std::endl; }
};

int main()
{
  Tracker tracker;
  // TODO : Obsluga wyjatkow konstruktorow
  // TODO : destruktor test poly
  {
    // std::cout << std::boolalpha;
    // Poly< RemoteVT<Storable> > t{ in_place<Guard<A>>(), tracker };
    // Poly< RemoteVT<Storable> > t2{ in_place<Guard<A>>(), tracker};
    // std::cout << "t: " << &t << " t2: " << &t2 << std::endl;
    // try{
    //   t = std::move(t2);
    // } catch (std::logic_error e){
    //   std::cout << "Exception : " << e.what() << std::endl;
    // }
    // static_assert( !std::is_nothrow_move_constructible<decltype(t)>::value,
    // 		   "Poly falsly assumed to be not nothrow copy constructible" );
    // static_assert( !std::is_nothrow_copy_constructible<decltype(t)>::value,
    // 		   "Poly falsly assumed to be not nothrow copy constructible" );

    // std::cout << "Invalid test" << std::endl;
    
    // Poly< RemoteVT<Storable> > t3{ t };

    // std::cout << "Invalid test end" << std::endl;
    B b;
  }
  assert( tracker.objects.count() == 0 );
  
  // test_memory();
  std::cout << "passed" << std::endl;
									 
  return 0;
}
