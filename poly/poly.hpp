#ifndef __POLY_HPP__
#define __POLY_HPP__

template< typename... >
struct print_ts;

#include <memory>
#include <tuple>
#include "vtable.hpp"

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
  // template< typename T >
  // using VTable = VTable_impl<T, Interface_type>;

  VTable<interface_type> vtable_;
  void* data_;
  
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
    return vtable_->call( invoker, unpack(std::forward<Ts>(vs))... );
  }

  template<
    typename Invoker
  , typename = std::enable_if_t< in_typelist<interface_type, std::add_const_t<Invoker>>::value >
  , typename... Ts
  > auto call( Ts&&... vs ) const
  {
    type<const Invoker>* invoker = nullptr;
    return vtable_->call( invoker, unpack(std::forward<Ts>(vs))... );
  }

  
  template< typename T >
  Poly( T v )
    : vtable_( VTable<interface_type>::template make<T>() )
    , data_( reinterpret_cast< void* >(new T( std::move( v ))))
  {  }

  Poly( const Poly& other )
    : vtable_( other.vtable_ )
    , data_( other.data_ )
  {  }

  Poly( Poly&& other ) noexcept = default;
private:
  template< typename U >
  static decltype(auto) unpack( U&& value ){
    return unpack_impl( std::is_same< std::decay_t<U>, Poly >(), std::forward<U>(value) );
  }
  
  template< typename U >
  static decltype(auto)
  unpack_impl( std::false_type, U&& v ){
    return std::forward<U>(v);
  }
  
  template< typename U >
  static copy_cv_ref_t<U&&, void*>
  unpack_impl( std::true_type, U&& v )
  {
    return static_cast< copy_cv_ref_t<U&&, void*> >(v.data_);
  }
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
