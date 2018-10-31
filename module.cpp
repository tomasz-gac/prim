#include <iostream>
#include "test/memory.hpp"
#include "test/test_jumpVTable.hpp"

int main()
{
  int i = 0;
  test_memory();
  test_JumpVTable();
  std::cout << "passed" << std::endl;
									 
  return 0;
}
