#include <iostream>
#include "test/memory.hpp"

#include "poly/JumpVTable.hpp"

using JT = JumpVTable< Storable, EraseVoidPtr, int, float, bool, int& >;

int main()
{
  test_memory();

  auto j_int = JT::make<int>();
  auto j_float = JT::make<float>();
  auto j_bool = JT::make<bool>();
  auto j_int_ref = JT::make<int&>();

  assert( j_int.index_     == 0 );
  assert( j_float.index_   == 1 );
  assert( j_bool.index_    == 2 );
  assert( j_int_ref.index_ == 3 );

  std::cout << "passed" << std::endl;
									 
  return 0;
}
