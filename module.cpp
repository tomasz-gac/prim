#include <iostream>
#include "example/variant.hpp"
#include "helpers.hpp"

#include <chrono>
#include <vector>

using rule_base = Variant< class Terminal, class Handle, class Not,
			   class Optional, class Alternative, class Sequence,
			   class Repeat, class Push >;

class Rule : public rule_base{
  using rule_base::rule_base;
};


class Terminal{};
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
  Rule r( in_place<Terminal>(), t );
  r.accept(print);
  Rule q(r);
  std::cout << "Constructed" << std::endl;
  q.accept(print);

  std::cout << "Passed : " << elapsed.count() << "s" << std::endl;
  return 0;
}
