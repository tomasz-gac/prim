#include <iostream>
#include "example/maybe.hpp"
#include "example/variant.hpp"
#include "test/memory.hpp"
#include "test/test_jumpVTable.hpp"
#include "poly/vtable/vtable.hpp"

#include <chrono>



int main()
{
  auto start = std::chrono::high_resolution_clock::now();

  test_variant();
  test_maybe();
  test_memory();
  test_JumpVTable();

  auto finish = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = finish - start;

  std::cout << "Passed : " << elapsed.count() << "s" << std::endl;
									 
  return 0;
}
