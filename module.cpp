#include <iostream>
#include "example/maybe.hpp"
#include "example/variant.hpp"
#include "test/memory.hpp"
#include "test/test_jumpVTable.hpp"

int main()
{
  test_variant();
  test_memory();
  test_JumpVTable();
  Maybe<int> mi = 3;
  Maybe<int> mn;
  std::cout << std::boolalpha;
  mn = mi;
  std::cout << mn.get() << std::endl;
  mn.get() = 2;
  std::cout << mn.get() << std::endl;
  std::cout << mn.empty() << std::endl;
  mn = {};
  try{
    std::cout << mn.get() << std::endl;
  } catch ( const empty_maybe_access& a ) {
    std::cout << "Empty" << std::endl;
  }
  std::cout << mn.empty() << std::endl;
  std::cout << "passed" << std::endl;
									 
  return 0;
}
