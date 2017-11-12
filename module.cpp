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

struct print  : Invoker< print, void (const T&) >{  };
struct assign : Invoker< assign, void ( T&, forward<T> ) > {  };
struct test   : Invoker< test, void (const int&)
			     , void (const double&) >{  };
struct type_id : Invoker< type_id, const std::type_info& ( const T& ) >{};

template< typename T >
struct as : Invoker< as<T>, T( ::T& ) >{  };

struct printable :
  Interface< print, assign, type_id, as<int&>, as<float&>, as<bool> >
{
  ::print print;
  ::assign assign;
  ::type_id type_id;
};
struct convertible : Interface< as<int&>, as<float&>, as<bool> >{};


template< typename T >
void invoke( print, const T& value ){
  print_type( std::forward<const T&>(value) );
}

template< typename T, typename T2 >
void invoke( assign, T& value, T2&& v ){
  value = std::forward<T2>(v);
}

template< typename T >
T& invoke( as<T&>, T& value ){ return value; }

template< typename T, typename U >
T& invoke( as<T&>, U& ){ throw std::bad_cast(); }

template< typename T, typename U >
T invoke( as<T>, const U& value ){ return value; }

template< typename T >
void invoke( test, const T& val ){ std::cout << typeid(T).name() << " " << val << std::endl; }

template< typename T >
const std::type_info& invoke( type_id, const T& ){ return typeid(T); }

template< typename T >
struct Invoke{};

int main()
{
  std::cout << std::boolalpha;
  printable p;
    
  int s = 1;
  View< printable > i = s;
  View< print > k = interface_cast<print>(i);
  View< convertible > c = interface_cast<convertible>(i);
  const auto& ci = i;
  i[ p.print ]();
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
  // auto vtbl = Local< test >::make();
  // vtbl[ test() ]( 1337.14 );
  // vtbl[ test() ]( 1337 );
  
  return 0;
}
