#ifndef __EITHER_HPP__
#define __EITHER_HPP__

#include "poly/poly.hpp"

template< typename >
class Visitor;

template< typename... Ts >
class Either;

template< typename... Ts >
struct accept : Signature< void( Visitor<Either<Ts...>>& ) > {};

template< typename... Ts, typename T >
constexpr auto invoke< accept<Ts...>, T > =
  []( T& value, Visitor<Either<Ts...>>& visitor ){
    visitor( value );
  };

template< typename... Ts, typename T >
constexpr auto invoke< const accept<Ts...>, T > =
  []( const T& value, Visitor<Either<Ts...>>& visitor ){
    visitor( value );
  };



template< typename... Ts >
class Either{
private:  
  using either_interface = Interface< accept< Ts... >, const accept< Ts... > >;

public:
  using visitor_type = Visitor< Either >;
  
  template< typename T, typename = std::enable_if_t< in_typelist< Either, T >::value, void > >
  Either( T v )
    : either_( std::move(v) )
  {  }

  void accept( visitor_type& visitor ){
    either_.template call< ::accept< Ts... > >(visitor);
  };

  void accept( visitor_type& visitor ) const {
    either_.template call< ::accept< Ts... > >(visitor);
  };

private:
  Poly< either_interface > either_;
};


template< typename T >
struct visit : Signature< void( T& )  > {};

template< typename T, typename F >
constexpr auto invoke< visit<T>, F > =
  []( F& visitor, auto& visited ){
  return visitor( visited );
  };

template< typename... Ts >
class Visitor< Either<Ts...> >{
private:
using visitor_interface =
  Interface<
    visit< std::remove_const_t<Ts> >...
  , visit< std::add_const_t<Ts> >...
  >;
public:
  template< typename T >
  void operator()( T& v ){
    visitor_.template call<visit<T>>( v );
  };

  template< typename T >
  Visitor( T&& v )
    : visitor_( std::forward<T>(v) )
  {  }
  
private:
  Poly< visitor_interface > visitor_;
};


#endif // __EITHER_HPP__
