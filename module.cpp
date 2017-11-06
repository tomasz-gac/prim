#include <iostream>
#include <typeinfo>
#include <cassert>
#include "poly/view.hpp"
#include "poly/vtable.hpp"

template< typename T >
void print_type( T&& value ){
  std::cout << (std::is_const<std::remove_reference_t<T&&>>::value ? "const " : "")
	    << (std::is_volatile<std::remove_reference_t<T&&>>::value ? "volatile " : "")
	    << typeid(value).name() 
	    << (std::is_rvalue_reference<T&&>::value ? "&&" : "&") << " == " << value << std::endl;
}

struct print : Signature< void(const T&) >{  };

struct assign : Signature< void( T&, forward<T> ) > {  };

template< typename T >
constexpr auto invoke< const print, const T > = []( auto&& value ){
  print_type( std::forward<decltype(value)>(value) );
};

template< typename... >
using void_t = void;

template< typename T >
constexpr auto invoke< assign, T > =
[]( T& value, auto&& v ) {
  std::cout << "assign" << std::endl;
  print_type( std::forward<decltype(v)>(v) );
  value = std::forward<decltype(v)>(v);
};

struct copy : Signature< void( const T&, void* ) >{};
template< typename T >
constexpr auto invoke< const copy, T, std::enable_if_t< std::is_copy_constructible<T>::value> >
= []( const T& v, void* ptr){ new(ptr) T( v ); };

struct printable : Interface< const volatile print, assign > {};

template< size_t n >
struct N : Signature< void( N<n> ) >{};

template< size_t i, typename T >
constexpr auto invoke< volatile N<i>, T > = []( auto ){ std::cout << "V" << i << std::endl; };

template< size_t i, typename T >
constexpr auto invoke< N<i>, T > = []( auto ){ std::cout << i << std::endl; };

template< size_t... is >
struct Ns : Overloaded< N< is >... >{};

int main()
{
  // using o = Ns< 1,2,3,4 >;
  // auto t = Local< o, volatile N<5>, N<6> >::make<int>();
  // call<volatile N<5>>( t, N<5>() );
  // call<o>( t, N<1>() );
  
  int s = 1;
  View< printable > i = s;
  const auto& ci = i;
  i.call< print >( i );
  int r = 3;
  auto j = View< printable >(r);
  const auto& cj = j;
  i.call< assign >( i, cj );
  ci.call< print >( i );
  i.call< assign >( i, std::move(j) );
  ci.call< print >( i );
  return 0;
}
