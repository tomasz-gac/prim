#ifndef __POLY_HPP__
#define __POLY_HPP__

#include <memory>
#include <tuple>
#include "holder.hpp"

template< typename... Ts >
struct Interface{
  using interface_type = Interface<Ts...>;
};

template< typename Interface_type >
class Poly
{
public:
  using interface_type = typename Interface_type::interface_type;
private:
  template< typename T >
  using Holder = Holder<T, Interface_type>;
public:
  template< typename T >
  Poly& operator=( T v ){
    return *this = Poly( std::move(v) );
  }
  
  Poly& operator=( const Poly&  other ){ return *this = Poly( other ); }
  Poly& operator=(       Poly&& other ) noexcept = default;
  
  

  template< typename Invoker, typename... Ts >
  auto call( Ts&&... vs ) ->
  std::enable_if_t<
    in_typelist<interface_type, std::remove_const_t<Invoker>>::value
  , return_t< Invoker > >
  {
    impl__::type<Invoker>* invoker = nullptr;
    return data_->call( invoker, std::forward<Ts>(vs)... );
  }

  template< typename Invoker, typename... Ts >
  auto call( Ts&&... vs ) const ->
  std::enable_if_t<
    in_typelist<interface_type, std::add_const_t<Invoker>>::value
  , return_t< Invoker > >
  {
    impl__::type<const Invoker>* invoker = nullptr;
    return data_->call( invoker, std::forward<Ts>(vs)... );
  }
  
  template< typename T >
  Poly( T v )
    : data_( std::make_unique<Holder<T>>(std::move(v)))
  {  }

  Poly( const Poly& other )
    : data_( other.data_->copy() )
  {  }

  Poly( Poly&& other ) noexcept = default;

private:
  std::unique_ptr< IHolder< interface_type > > data_;
};

template< typename Invoker, typename Interface, typename... Ts >
return_t< Invoker > call( Poly< Interface >& poly, Ts&&... vs ){
  return poly.template call<Invoker>( std::forward< Ts... >(vs)... );
}

template< typename Invoker, typename Interface, typename... Ts >
return_t< Invoker > call( const Poly< Interface >& poly, Ts&&... vs ){
  return poly.template call<Invoker>( std::forward< Ts... >(vs)... );
}

#endif // __POLY_HPP__
