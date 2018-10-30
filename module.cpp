#include <iostream>
#include "test/memory.hpp"

struct JVT_test
  : Interface< as<int>, as<float>, as<double>, copy, move, destroy, storage >
{  };

using JT = JumpVT< JVT_test, int, float, double, int& >;

template< typename C, typename I >
void check_call( C&& c, const I& i ){
  auto info = c.template call<storage>();
  assert( sizeof(i) == info.size );
  assert( alignof(i) == info.alignment );
  assert( c.template call<as<I>>() == i );
}

int main()
{
  auto j_int     = impl_t<JT>::make<int>();
  auto j_float   = impl_t<JT>::make<float>();
  auto j_double  = impl_t<JT>::make<double>();
  auto j_int_ref = impl_t<JT>::make<int&>();

  assert( j_int.index()     == 0 );
  assert( j_float.index()   == 1 );
  assert( j_double.index()  == 2 );
  assert( j_int_ref.index() == 3 );

  int i =0;
  float f = 1;
  double b = 2;
  int j = 3;
  int& i_r = j;
  Invalid inv;

  View< JT > v = i;
  check_call( v, i );
  v = f;
  check_call( v, f );
  v = b; 
  check_call( v, b );
  v = i_r; 
  check_call( v, j );
  v = inv;
  bool thrown = false;
  try{
    check_call( v, i_r );
  } catch (const invalid_vtable_call& e ){
    thrown = true;
  }
  assert( thrown );
  test_memory();
  std::cout << "passed" << std::endl;
									 
  return 0;
}
