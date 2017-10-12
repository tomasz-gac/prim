#ifndef __POLY_HPP__
#define __POLY_HPP__

#include <memory>
#include <tuple>
#include "holder.hpp"

template< typename... >
struct Interface;

template< typename Interface >
class Poly;

template< template< typename... > class typelist, typename... Invokers >
class Poly< typelist< Invokers... > >
{
private:
  using Interface = typelist< Invokers... >;
  using Holder_ = Holder_common< Interface >;
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
  auto call( Ts&&... vs ) ->
  std::enable_if_t<
    in_typelist<Interface, std::remove_const_t<Invoker>>::value
  , return_t< Invoker > >
  {
    using invoker_t = std::remove_const_t<Invoker>;
    auto& interface = *std::get< IHolder< invoker_t >* >(interface_);
    return interface.call( std::forward<Ts>(vs)... );
  }

  template< typename Invoker, typename... Ts >
  auto call( Ts&&... vs ) const ->
  std::enable_if_t<
    in_typelist<Interface, std::add_const_t<Invoker>>::value
  , return_t< Invoker > >
  {
    using invoker_t = std::add_const_t<Invoker>;
    const auto& interface = *std::get< IHolder< invoker_t >* >(interface_);
    return interface.call( std::forward<Ts>(vs)... );
  }

  template< typename T >
  Poly( T v )
    : data_( std::unique_ptr<Holder_>( std::make_unique<Holder<T>>(std::move(v))) )
    , interface_( std::make_tuple(
        &data_
	->template as_holder<T>()
	.template as_interface<Invokers>()...
       ) )
  {  }
  
private:
  std::unique_ptr< Holder_ >      data_;
  std::tuple< IHolder< Invokers >*... > interface_;
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
