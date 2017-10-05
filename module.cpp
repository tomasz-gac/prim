#include <iostream>
#include "poly/poly.hpp"

struct print
  : Signature<void ()>
{  };

template< typename T >
constexpr auto Invoker< print, T > = []( T& v ){ v += 1; std::cout << v << "nc" << std::endl; };

template< typename T >
constexpr auto Invoker< print, const T > = []( const T& v ){ std::cout << v << "c" << std::endl; };

using print_i = Interface< print, const print >;

int main()
{
  const Poly< print_i > s = 1;
  s.call<print>();

  return 0; //"hello, world";
}


