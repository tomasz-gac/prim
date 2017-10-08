#ifndef __SIGNATURE_HPP__
#define __SIGNATURE_HPP__

#include <type_traits>
#include "typelist.hpp"

template< typename... >
struct signature_args;

// Type that encodes a signature for a given invoker
// Invokers are meant to derive from this type
template< typename >
class Signature;

template< typename Return, typename... Args >
struct Signature< Return(Args...)>{
  using return_type = Return;
  using signature_type = Signature;
  using args_type = signature_args< Args... >;
};

template< typename T >
using signature_t = typename T::signature_type;

template< typename T >
using return_t = typename T::return_type;

template< typename T >
using args_t = typename T::args_type;

template< typename T, typename U >
struct value{ using rvalue = T; using lvalue = U; };

template< typename T >
struct make_overload{ using type = value<T&&, const T&>; };

template< typename T >
struct make_overload<T&>{ using type = T&; };

template< typename T >
struct make_overload<T&&>{ using type = T&&; };

template< typename T >
using marked_signature_t = map_t< typename T::args_type, make_overload>;






#endif // __SIGNATURE_HPP__
