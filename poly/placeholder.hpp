#ifndef __PLACEHOLDER_HPP__
#define __PLACEHOLDER_HPP__

#include "signature.hpp"

struct T{};

template< typename Eraser_t >
struct Erased;

template< typename T >
struct is_erased : std::false_type {};

template< typename Eraser_t >
struct is_erased< Erased<Eraser_t>> : std::true_type {};

template< typename T, typename Eraser_t >
struct erase_placeholder{
  using type = T;
};

template< typename Eraser_t >
struct erase_placeholder<::T, Eraser_t >{
  using type = Erased<Eraser_t>;
};

template< typename T, typename Eraser_t >
using erase_placeholder_t = typename erase_placeholder<T, Eraser_t>::type;

template< typename T, typename Unerased >
struct unerase_placeholder{
  using type = T;
};

template< typename Eraser_t, typename Unerased >
struct unerase_placeholder< Erased<Eraser_t>, Unerased >{
  using type = Unerased;
};

template< typename T, typename Unerased >
using unerase_placeholder_t = typename unerase_placeholder< T, Unerased >::type;

template< typename T, typename Unerased, typename = std::enable_if_t< !is_erased<T>{} > >
decltype(auto) unerase( T&& value ){
  return std::forward<T>(value);
}

template< typename Eraser_t, typename T >
Erased<Eraser_t> erase( T&& value ){
  return {std::forward<T>(value)};
}

template<>
struct Erased<void*>{
  using eraser_type = void*;

  template< typename T >
  Erased( T&& v )
    : value( reinterpret_cast< void* >(&v) )
  {  }

  Erased(const Erased& ) = default;
  Erased(      Erased&&) = default;

  void* value;
};

template< typename Unerased >
decltype(auto) unerase( Erased<void*> erased ){
  using raw = std::decay_t<Unerased>;
  return
    static_cast<Unerased&&>( *reinterpret_cast< raw* >( erased.value ) );
};



#endif // __PLACEHOLDER_HPP__
