#include <iostream>
#include "prim/identity.hpp"
#include "example/variant.hpp"
#include "helpers.hpp"

#include <chrono>
#include <vector>

class Terminal{};

using rule_base = Variant< class Terminal, Recursive<class Handle>, Recursive<class Not>,
			   Recursive<class Optional>, Recursive<class Alternative>, Recursive<class Sequence>,
			   Recursive<class Repeat>, Recursive<class Push> >;

class Rule : public rule_base{
  using rule_base::rule_base;
};

class Handle{};
class Not{};
class Optional{};
class Alternative{};
class Sequence{};
class Repeat{};
class Push{};


int main()
{
  auto start = std::chrono::high_resolution_clock::now();
  
  auto finish = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = finish - start;
  auto print = []( auto& v ){ std::cout << T2Str(v) << std::endl; };

  Terminal t; Handle h; Optional o;
  Rule r{ in_place<Recursive<Handle>>(), h};
  r.accept(print);
  Rule q(r);
  std::cout << "Constructed" << std::endl;
  q.accept(print);

  prim::identity< int, prim::common_basic_t<int> > i{ 123 };
  std::cout << i.get() << std::endl;

  std::cout << "Passed : " << elapsed.count() << "s" << std::endl;
  return 0;
}
