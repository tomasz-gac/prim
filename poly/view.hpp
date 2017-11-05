#ifndef __VIEW_HPP__
#define __VIEW_HPP__

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
class View
{
public:
  using interface_type = typename Interface_type::interface_type;
private:
  // template< typename T >
  // using VTable = VTable_impl<T, Interface_type>;

  using VTable = repack_t< interface_type, Local<> >;

  VTable vtable_;
  void* data_;
  
public:
  template< typename T >
  View& operator=( T v ){
    return *this = View( std::move(v) );
  }
  
  View& operator=( const View&  other ){ return *this = View( other ); }
  View& operator=(       View&& other ) noexcept = default;

  template<
    typename Invoker
  , typename = std::enable_if_t< in_typelist<interface_type, std::remove_const_t<Invoker>>::value >
  , typename... Ts 
  > auto call( Ts&&... vs )
  {
    return ::call<Invoker>(vtable_, unpack(std::forward<Ts>(vs))... );
  }

  template<
    typename Invoker
  , typename = std::enable_if_t< in_typelist<interface_type, std::add_const_t<Invoker>>::value >
  , typename... Ts
  > auto call( Ts&&... vs ) const
  {
    return ::call<const Invoker>(vtable_, unpack(std::forward<Ts>(vs))... );
  }

  
  template< typename T >
  View( T& v )
    : vtable_( VTable::template make<T>() )
    , data_( reinterpret_cast< void* >(&v) )
  {  }

  View( const View& other )
    : vtable_( other.vtable_ )
    , data_( other.data_ )
  {  }

  View( View&& other ) noexcept = default;
private:
  template< typename U >
  static decltype(auto) unpack( U&& value ){
    return unpack_impl( std::is_same< std::decay_t<U>, View >(), std::forward<U>(value) );
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

#endif // __POLY_HPP__
