#ifndef __HELPERS_HPP__
#define __HELPERS_HPP__

#include <string>

template< typename T >
std::string T2Str( T&& value ){
  return std::string() + (std::is_const<std::remove_reference_t<T&&>>::value ? "const " : "")
    + (std::is_volatile<std::remove_reference_t<T&&>>::value ? "volatile " : "")
    + typeid(value).name() 
    + (std::is_rvalue_reference<T&&>::value ? "&&" : "&");
}

template< typename T >
void print_type( T&& value ){
  std::cout << T2Str( std::forward<T>(value) ) <<" == " << value << std::endl;
}

struct print  : prim::Invoker< print, void (const prim::T&) >{  };

template< typename T >
struct as : prim::Invoker< as<T>, T( prim::T& ) >{  };

template< typename T >
void invoke( print, const T& value ){
  print_type( std::forward<const T&>(value) );
}


template< typename T >
T& invoke( as<T&>, T& value ){ return value; }

template< typename T, typename U >
T& invoke( as<T&>, U& ){ throw std::bad_cast(); }

template< typename T, typename U >
T invoke( as<T>, const U& value ){ return value; }

#endif // __HELPERS_HPP__
