#include <iostream>
#include <typeinfo>
#include <cassert>
#include "poly/view.hpp"

template< typename T >
void print_type( T&& value ){
  std::cout << (std::is_const<std::remove_reference_t<T&&>>::value ? "const " : "")
	    << (std::is_volatile<std::remove_reference_t<T&&>>::value ? "volatile " : "")
	    << typeid(value).name() 
	    << (std::is_rvalue_reference<T&&>::value ? "&&" : "&") << " == " << value << std::endl;
}

struct print : Signature< void (const T&) >{  };
struct assign : Signature< void ( T&, forward<T> ) > {  };

template< typename T >
void invoke( print, const T& value ){
  print_type( std::forward<decltype(value)>(value) );
}

template< typename T, typename T2 >
void invoke( assign, T& value, T2&& v ){
  std::cout << "assign" << std::endl;
  print_type( std::forward<T2>(v) );
  value = std::forward<T2>(v);
};

template< typename T >
struct as : Signature< T&( ::T& ) >{  };

template< typename T, typename U >
T& invoke( as<T>, U& value ){
  return value;
};

struct printable : Interface< print, assign > {};

template< typename T >
struct test : Signature< void (const T&) >{};

template< typename T >
void invoke( test<T>, const T& val ){
  std::cout << val << std::endl;
}

int main()
{
  int s = 1;
  View< printable > i = s;
  View< printable > k = i;
  k[ print() ](k);
  const auto& ci = i;
  i[ print() ]( i );
  int r = 3;
  auto j = View< printable >(r);
  const auto& cj = j;
  i[assign()]( i, cj );
  ci[print()]( i );
  i[assign()]( i, std::move(j) );
  ci[print()]( i );
  auto tvtbl = Local< Interface< test<int>, test<double> > >::make();
  tvtbl[ test<double>() ]( 1337.14 );
  
  return 0;
}
