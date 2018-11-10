#ifndef __TEST_JUMP_VTABLE_HPP__
#define __TEST_JUMP_VTABLE_HPP__

#include "../poly/reference.hpp"
#include "../poly/vtable/vtable.hpp"
#include "../poly/builtins.hpp"
#include "../helpers.hpp"
#include <cassert>

struct JVT_test
  : poly::Interface< as<int>, as<float>, as<double>, poly::storage >
{  };

using JT = poly::JumpVT< JVT_test, int, float, double, int& >;

template< typename C, typename I >
void check_call( C&& c, const I& i ){
  auto info = poly::call<poly::storage>( *c );
  assert( sizeof(i) == info.size );
  assert( alignof(i) == info.alignment );
  assert( poly::call<as<I>>( *c ) == i );
}

int test_JumpVTable()
{
  auto j_int     = poly::impl_t<JT>::make<int>();
  auto j_float   = poly::impl_t<JT>::make<float>();
  auto j_double  = poly::impl_t<JT>::make<double>();
  auto j_int_ref = poly::impl_t<JT>::make<int&>();

  assert( j_int.index()     == 0 );
  assert( j_float.index()   == 1 );
  assert( j_double.index()  == 2 );
  assert( j_int_ref.index() == 3 );

  int i =0;
  float f = 1;
  double b = 2;
  int j = 3;
  int& i_r = j;
  poly::Invalid& inv = poly::Invalid::get();

  poly::Pointer< JT > v = &i;
  check_call( v, i );
  v = &f;
  check_call( v, f );
  v = &b; 
  check_call( v, b );
  v = &i_r; 
  check_call( v, j );
  v = &inv;
  bool thrown = false;
  try{
    check_call( v, i_r );
  } catch (const poly::invalid_vtable_call& e ){
    thrown = true;
  }
  assert( thrown );
									 
  return 0;
}

#endif // __TEST_JUMP_VTABLE_HPP__
