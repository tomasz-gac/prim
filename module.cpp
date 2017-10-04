#include <iostream>
#include "poly/poly.hpp"

struct print
  : Signature<void ()>
{  };

template< typename T >
constexpr auto Invoker< print, T > = []( T& v ){ std::cout << v << "nc" << std::endl; };

// template< typename T >
// constexpr auto Invoker< print, const T > = []( const T& v ){ std::cout << v << "c" << std::endl; };

using print_i = Interface< const print, print >;

int main()
{
 Poly< print_i > s = 1;
 s.call<print>();
  
  return 0; //"hello, world";
}


