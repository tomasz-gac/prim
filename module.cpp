#include <iostream>
#include <sstream>
#include <string>
#include <cmath>
#include "poly/poly.hpp"

template< typename To >
struct to_ : Signature< To () >{  };

template< typename T, typename To >
constexpr auto invoke< const to_<T>, To > = []( const T& v ){ return v;  };

template< typename T>
constexpr auto invoke< const to_<std::string>, T > =
  []( const T& v ){ return std::to_string(v);  };

template< typename From >
struct from_ : Signature< void (From) >{ };

template< typename T, typename From >
constexpr auto invoke< from_<From>, T > = []( T& v, From value ){ v.get() = value; };

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

struct A{
  A( const A&  ){ std::cout << "A C-C" << std::endl; };
  A(       A&& ){ std::cout << "A M-C" << std::endl; };
  A() = default;
  
};

struct proof
  : Signature< void (A,A&,A) >
{  };

template< typename T >
constexpr auto invoke< proof, T > = []( T&, auto... a ){ std::cout << "non-const" << std::endl; };

template< typename T >
constexpr auto invoke< const proof, const T > = []( const T&, auto... a ){ std::cout << "const" << std::endl; };


using proof_i = Interface< const proof, proof >;

int main()
{
  // using conversion_t = std::string;
  // int i = 1;
  // convertible_to<conversion_t> cnv = i;
  // std::cout << cnv.get() << std::endl;
  // conversion_t tmp;
  // std::cin >> tmp;
  // cnv = tmp;
  // i += 1;
  // tmp = cnv.get();
  // std::cout << tmp <<std::endl;

  A a;
  Poly< proof_i > test1 = 1;
  // test1.call<proof>( a1 );
  test1.call<proof>( a,a,a );
  // test2.call<proof>( a2 );
  // test2.call<proof>( std::move(a2) );

  return 0;
}


