#ifndef __POLY_HPP__
#define __POLY_HPP__

#include <memory>
#include <tuple>
#include "holder.hpp"

template< typename... Ts >
class Interface{
public:
  using interface_type = Interface<Ts...>;
private:  
  static assert_unique_elements< Interface >
    assert_unique_tags;
};

template< typename Interface_type >
class Poly
{
public:
  using interface_type = typename Interface_type::interface_type;
private:
  template< typename T >
  using Holder = Holder<T, Interface_type>;

  std::unique_ptr< IHolder< interface_type > > data_;
public:
  template< typename T >
  Poly& operator=( T v ){
    return *this = Poly( std::move(v) );
  }
  
  Poly& operator=( const Poly&  other ){ return *this = Poly( other ); }
  Poly& operator=(       Poly&& other ) noexcept = default;
  
  

  template<
    typename Invoker
    , typename = std::enable_if_t< in_typelist<interface_type, std::remove_const_t<Invoker>>::value >
    , typename... Ts
  > auto call( Ts&&... vs )
  {
    type<Invoker>* invoker = nullptr;
    return data_->call( invoker, std::forward<Ts>(vs)... );
  }

  template<
    typename Invoker
  , typename = std::enable_if_t< in_typelist<interface_type, std::add_const_t<Invoker>>::value >
  , typename... Ts
  > auto call( Ts&&... vs ) const
  {
    type<const Invoker>* invoker = nullptr;
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
};

template< typename Invoker, typename Interface, typename... Ts >
//return_t< Invoker >
auto call( Poly< Interface >& poly, Ts&&... vs ){
  return poly.template call<Invoker>( std::forward< Ts... >(vs)... );
}

template< typename Invoker, typename Interface, typename... Ts >
//return_t< Invoker >
auto call( const Poly< Interface >& poly, Ts&&... vs ){
  return poly.template call<Invoker>( std::forward< Ts... >(vs)... );
}

#endif // __POLY_HPP__
