#include <iostream>
#include <typeinfo>
#include <cassert>
#include "poly/view.hpp"
#include "helpers.hpp"

struct printable :
  Interface< print, assign, type_id, as<int&>, as<float&>, as<bool> >
{
  ::print print;
  ::assign assign;
  ::type_id type_id;
};

struct convertible : Interface< as<float&>, as<int&>, as<bool> >{};

template< typename... Ts >
struct only{
  template< typename T >
  using type = typename disjunction< std::is_same< T, Ts >... >::type;
};

template< typename... Ts >
struct except{
  template< typename T >
  using type =
    std::integral_constant< bool, !disjunction< std::is_same< T, Ts >... >::type::value >;
};


template< typename ... >
struct print_ts;

int main()
{
  printable p; 
  std::cout << std::boolalpha;
  
  float f = 1.11;
  struct printfl : decltype( interface(print(), as<float>()) ){};
  using vtbl = decltype(make_vtable( local( printfl() ),
				     remote( printable()) ) );
  View< vtbl > fff = f;
  fff[ p.print ]();
    
  int s = 1;
  View< Local< printable > > i = s;
  View< Remote< printable > > ii = s;
  std::cout << sizeof( fff ) << " " << sizeof( i ) << " " << sizeof(ii) << std::endl;
  auto k = interface_cast<print>(i);
  auto c = interface_cast<convertible>(i);
  const auto& ci = i;
  k[ p.print ]();
  c[ as<int&>() ]() = 5;
  try{
    c[ as<float&>() ]() = 3.14;
  } catch ( std::bad_cast e ){    
    std::cout << "Exception : " << e.what() << std::endl;
  };
  std::cout << c[ as<bool>() ]() << std::endl;
  int s2 = 3;
  View< Remote<printable> > i2 = s2;
  i[ p.assign ]( i2 );
  k[ p.print ](); 
  std::cout << i[ p.type_id ]().name() << std::endl;
  i[ p.print ]();
  ci[ p.print ]();
  
  return 0;
}
