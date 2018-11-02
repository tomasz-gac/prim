#include <iostream>
#include "example/maybe.hpp"
#include "example/variant.hpp"
#include "test/memory.hpp"
#include "test/test_jumpVTable.hpp"

int main()
{
  test_variant();
  test_maybe();
  test_memory();
  test_JumpVTable();
  std::cout << "passed" << std::endl;
									 
  return 0;
}
