#include <iostream>
#include <string>
#include "poly/poly.hpp"

struct print
  : Signature<void ()>
{  };

struct to_string
  : Signature< std::string () >
{  };

template< typename T >
constexpr auto invoke< print, T > = []( T& v ){ v += 1; std::cout << v << "nc" << std::endl; };

template< typename T >
constexpr auto invoke< print, const T > = []( const T& v ){ std::cout << v << "c" << std::endl; };

template< typename T >
constexpr auto invoke< to_string, const T > = []( const T& v ){ return std::to_string( v ); };

using printable = Interface< const to_string, const print >;

int main()
{
  const Poly< printable > s = 1;
  std::cout << s.call<to_string>() << std::endl;

  return 0; //"hello, world";
}


