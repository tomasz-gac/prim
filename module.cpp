#include <iostream>
#include "prim/identity.hpp"
#include "example/variant.hpp"
#include "helpers.hpp"

#include <chrono>
#include <vector>

class Terminal{};
class Handle{};
class Not{};
class Optional{};
class Alternative{};
class Sequence{};
class Repeat{};
class Push{};

using rule_base = Variant< class Terminal, class Handle, class Not,
			   class Optional, class Alternative, class Sequence,
			   class Repeat, class Push >;

class Rule : public rule_base{
  using rule_base::rule_base;
};

template< typename T >
struct interface_of{
  using interface__ = 
    prim::Interface< prim::type, prim::destroy >;

  using interface_copy__ =
    std::conditional_t< std::is_copy_constructible<T>::value,
			typename interface__::template append<prim::copy>, interface__ >;

  using interface_move__ =
    std::conditional_t< std::is_move_constructible<T>::value,
			typename interface_copy__::template append<
			  prim::move_<std::is_nothrow_move_constructible<T>::value>
			  >,
			interface_copy__ >;
			
			

  using type = interface_move__;
};


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
