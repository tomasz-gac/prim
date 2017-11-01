#include <iostream>
#include <typeinfo>
// #include "example/graph_interface.hpp"
#include <cassert>
#include "poly/poly.hpp"

template< typename >
struct print_t;

template< typename T >
void print_type( T&& value ){
  std::cout << (std::is_const<std::remove_reference_t<T&&>>::value ? "const " : "")
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


struct destruct : Signature< void( const T& ) >{};
template< typename T>
constexpr auto invoke< const destruct, T > = []( const T& v){ v.~T(); };

struct copy : Signature< void( const T&, void* ) >{};
template< typename T >
constexpr auto invoke< const copy, T, std::enable_if_t< std::is_copy_constructible<T>::value> >
= []( const T& v, void* ptr){ new(ptr) T( v ); };

struct printable : Interface< const print, assign > {};

int main()
{
  Poly< printable > i = 1;
  const auto& ci = i;
  i.call< print >( i );
  auto j = Poly< printable >(3);
  const auto& cj = j;
  i.call< assign >( i, cj );
  ci.call< print >( i );
  i.call< assign >( i, std::move(j) );
  ci.call< print >( i );
  std::cout << sizeof( VTable<printable> ) << std::endl;
  return 0;
}
