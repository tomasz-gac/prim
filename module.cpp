#include <iostream>
#include <typeinfo>
#include <cassert>
#include "poly/view.hpp"
#include "poly/vtable.hpp"

template< typename >
struct Invoker;

template< typename Return, typename... Args >
struct Invoker< Return(Args...) >
  : public Signature< Return(Args...) >
{  };

template< typename T >
void print_type( T&& value ){
  std::cout << (std::is_const<std::remove_reference_t<T&&>>::value ? "const " : "")
	    << (std::is_volatile<std::remove_reference_t<T&&>>::value ? "volatile " : "")
	    << typeid(value).name() 
	    << (std::is_rvalue_reference<T&&>::value ? "&&" : "&") << " == " << value << std::endl;
}

struct print : Invoker< void(const T&) >{  };
struct assign : Signature< void( T&, forward<T> ) > {  };

template< typename T >
void invoke( Tag< const print >, T&& value ){
  print_type( std::forward<decltype(value)>(value) );
}

template< typename T, typename T2 >
void invoke( Tag< assign >, T& value, T2&& v ){
  std::cout << "assign" << std::endl;
  print_type( std::forward<decltype(v)>(v) );
  value = std::forward<decltype(v)>(v);
};

struct copy : Signature< void( const T&, void* ) >{};

template< typename T, typename = std::enable_if_t< std::is_copy_constructible<T>::value> >
void invoke( Tag< const copy >, const T& v, void* ptr){
  new(ptr) T( v );
}

struct printable : Interface< const print, assign > {};

int main()
{
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
