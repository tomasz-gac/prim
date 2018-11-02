#include <iostream>
// #include "example/maybe.hpp"
// #include "example/variant.hpp"
// #include "test/memory.hpp"
// #include "test/test_jumpVTable.hpp"
// #include "poly/vtable/vtable.hpp"
#include "poly/pointer.hpp"
#include "poly/vtable/vtable.hpp"

struct addr : poly::Invoker< addr, void* ( poly::T&, poly::T& ) >{  };

template< typename T >
void* invoke( addr, T& v, T& v2 ){
  return reinterpret_cast<void*>(&v2);
}

int main()
{
  int i = 0;
  void* ptr = reinterpret_cast<void*>(&i);
  poly::Pointer< poly::RemoteVT< addr > > ii = &i;
  auto pptr = addr::call( ii, poly::unwrap(ii) );
  std::cout << pptr << " " << ptr << std::endl;
  
  // test_variant();
  // test_maybe();
  // test_memory();
  // test_JumpVTable();
  std::cout << "Passed" << std::endl;
									 
  return 0;
}
