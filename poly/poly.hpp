#ifndef __POLY_HPP__
#define __POLY_HPP__

#include <memory>
#include "holder.hpp"

template< typename Interface >
class Poly;

template< typename... Invokers >
class Poly< Interface< Invokers... > >
{
private:
  using Interface = ::Interface< Invokers... >;
  using IHolder = IHolder< Interface >;
  template< typename T >
  using Holder = Holder<T, Interface>;
public:
  template< typename T >
  Poly& operator=( T v ){
    Poly< T > poly( std::move(v) );
    data_.reset( poly.data_.release() );
    return *this;
  }

  template< typename Invoker, typename... Ts >
  std::enable_if_t< implements<Invoker, Interface>::value, return_t< Invoker > >
  call( Ts&&... vs ){
    IHolder& data = *data_;
    using tag_t = std::remove_const_t<Invoker>;
    return ::call<tag_t>( data, std::forward<Ts>(vs)... );
  }

  template< typename Invoker, typename... Ts >
  std::enable_if_t< implements<Invoker, Interface>::value, return_t< Invoker > >
  call( Ts&&... vs ) const {
    const IHolder& data = *data_;
    using tag_t = std::add_const_t<Invoker>;
    return ::call<tag_t>( data, std::forward<Ts>(vs)... );
  }

  template< typename T >
  Poly( T v )
    : data_( std::unique_ptr<IHolder>( std::make_unique<Holder<T>>(std::move(v))) )
  { }
  
private:
  std::unique_ptr< IHolder > data_;
};

template< typename Invoker, typename... Invokers, typename... Ts >
return_t< Invoker > call( Poly< Interface< Invokers... > >& poly, Ts&&... vs ){
  return poly.template call<Invoker>( std::forward< Ts... >(vs)... );
}

template< typename Invoker, typename... Invokers, typename... Ts >
return_t< Invoker > call( const Poly< Interface< Invokers... > >& poly, Ts&&... vs ){
  return poly.template call<Invoker>( std::forward< Ts... >(vs)... );
}

#endif // __POLY_HPP__
