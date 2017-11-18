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

template< typename T, typename... Ts >
struct join{
  struct type : concat_t< interface_t<T>, interface_t< Ts >... >{  };
};

template< typename... Ts >
using join_t = typename join<Ts...>::type;


template< typename ... >
struct print_ts;

int main()
{
  using tl = std::tuple< float, int, bool, int, std::string >;
  print_ts< filter_t< tl, except<std::string>::template type > > fq;
  
  std::cout << std::boolalpha;
  printable p;
    
  int s = 1;
  View< printable, Local > i = s;
  View< printable, Remote > ii = s;
  std::cout << sizeof( i ) << " " << sizeof(ii) << std::endl;
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
  View< printable > i2 = s2;
  i[ p.assign ]( i2 );
  k[ p.print ](); 
  std::cout << i[ p.type_id ]().name() << std::endl;
  i[ p.print ]();
  ci[ p.print ]();
  
  return 0;
}
