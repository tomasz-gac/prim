#ifndef __CONVERTIBLE_TO_HPP__
#define __CONVERTIBLE_TO_HPP__

#include "poly/value.hpp"
#include <sstream>
#include <string>


template< typename To >
struct to_ : Signature< To () >{  };

template< typename T, typename To >
constexpr auto invoke< const to_<T>, To > = []( const T& v ){ return v;  };

template< typename T>
constexpr auto invoke< const to_<std::string>, T > =
  []( const T& v ){ return std::to_string(v);  };

template< typename From >
struct from_ : Signature< void (forward<From>) >{ };

template< typename T, typename From >
constexpr auto invoke< from_<From>, T > = []( T& v, From value ){ v.get() = std::move(value); };

template< typename T >
constexpr auto invoke< from_<std::string>, T > =
  []( T& v, std::string value ){
    auto converter = std::stringstream(std::move(value));
    converter >> v;
  };

template< typename T >
class convertible_to
{
private:
  using convertible = Interface< const to_<T>, from_<T> >;
public:
  using conversion_type = T;
  
  template< typename U >
  convertible_to( U&& v )
    : poly_( std::ref(std::forward<U>(v)) )
  {  }

  operator T() const { return poly_.template call<to_<T>>(); }
  T get() const { return *this; }

  convertible_to& operator=( T value ){
    poly_.template call<from_<T>>( std::move(value) );
    return *this;
  }
  
private:
  Poly< convertible > poly_;  
};

#endif // __CONVERTIBLE_TO_HPP__
