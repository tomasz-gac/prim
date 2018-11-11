#include <iostream>
#include "example/maybe.hpp"
#include "example/variant.hpp"
#include "test/memory.hpp"
#include "test/test_jumpVTable.hpp"
#include "poly/vtable/vtable.hpp"

#include <chrono>

struct Ref :
  poly::declare< Ref, void (poly::T&, const poly::T& ), void (poly::T&, poly::T&& )>
{  };
template< typename U, typename T >
void invoke( Ref, U& u, const T& v ){
  std::cout << "const & = " << v << std::endl;
}

template< typename U, typename T >
void invoke( Ref, U& u, T&& v ){
  std::cout << "&& = " << v << std::endl;
}

template< typename... >
struct p_t;

int main()
{
  auto start = std::chrono::high_resolution_clock::now();
  int i = 0;
  using Thunk_tuple = poly::Thunk< Ref, void* >::thunk_tuple;
  auto th = poly::Thunk< Ref, void* >::make<int>();
  th( poly::Erased<void*>(), std::move(poly::Erased<void*>()) );
  
  poly::Pointer< poly::LocalVT< Ref > > ptr  = &i;
  poly::Pointer< poly::LocalVT< Ref > > ptr2 = &i;

  auto& cp = ptr2;

  poly::call<Ref>( *ptr, *cp );
  // poly::call<Ref>( *ptr, std::move(*ptr2) );

  // test_variant();
  // test_maybe();
  // test_memory();
  // test_JumpVTable();

  auto finish = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = finish - start;

  std::cout << "Passed : " << elapsed.count() << "s" << std::endl;
									 
  return 0;
}
