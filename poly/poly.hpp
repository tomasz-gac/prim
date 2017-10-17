#ifndef __POLY_HPP__
#define __POLY_HPP__

#include <memory>
#include <tuple>
#include "holder.hpp"

template< typename... Ts >
struct Interface{
  using interface = Interface<Ts...>;
};

template< typename Interface_type >
class Poly
{
private:
  using Interface = typename Interface_type::interface;
  using Holder_ = Holder_common< Interface >;
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
    in_typelist<Interface, std::remove_const_t<Invoker>>::value
  , return_t< Invoker > >
  {
    using invoker_t = std::remove_const_t<Invoker>;
    // auto& interface = *std::get< IHolder< invoker_t >* >(interface_);
    auto& interface = *static_cast< IHolder< invoker_t >* >(anchor_);    
    return interface.call( std::forward<Ts>(vs)... );
  }

  template< typename Invoker, typename... Ts >
  auto call( Ts&&... vs ) const ->
  std::enable_if_t<
    in_typelist<Interface, std::add_const_t<Invoker>>::value
  , return_t< Invoker > >
  {
    using invoker_t = std::add_const_t<Invoker>;
    // const auto& interface = *std::get< IHolder< invoker_t >* >(interface_);
    const auto& interface = *static_cast< IHolder< invoker_t >* >(anchor_);
    return interface.call( std::forward<Ts>(vs)... );
  }
  
  template< typename T >
  Poly( T v )
    : data_( std::make_unique<Holder<T>>(std::move(v)))
    // , interface_( data_->interface() )
    , anchor_( data_->getAnchor())
  {  }

  Poly( const Poly& other )
    : data_( other.data_->copy() )
    // , interface_( data_->interface() )
    , anchor_( data_->getAnchor() )
  {  }

  Poly( Poly&& other ) noexcept = default;

private:
  std::unique_ptr< Holder_ >    data_;
  // typename Holder_::interface_t interface_;
  impl__::Anchor* anchor_;
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
