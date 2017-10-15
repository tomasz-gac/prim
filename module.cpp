#include <iostream>
#include "graph.hpp"

template< typename T >
struct test : Signature< void(T&) > {};

template< typename T, typename U >
constexpr auto invoke< test<T>, U > =
  []( U& v, T& t){
    std::cout << v(t) << std::endl;
  };

template< typename T, typename U >
constexpr auto invoke< const test<T>, U > =
  []( const U& v, T& t){
    std::cout << v(t) << std::endl;
  };

template< typename... Ts >
using test_interface = Interface< test<Ts>..., const test<Ts>... >;

struct test_i
  : test_interface< int, float, bool, double >
{  };

int main()
{
  using graph = Graph< void, int, float, bool >;
  using visitor = Visitor< graph >;
  auto g = graph::make<Terminal>(int(3));
  visitor v = []( auto& v){ std::cout <<  v.value << std::endl;  };
  g.accept(v);

  
  // Poly< test_i > poly = []( auto& i ){ return i+1; };
  
  return 0;
}


