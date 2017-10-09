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

template<
  typename typelist
, template< typename... > class Predicate
, template< typename... > class UnaryOp
>
using apply_first_not_t =
  concat_t<
    takeWhile_t< typelist, Predicate >
  , id_t< typelist, typename UnaryOp< head_t<dropWhile_t<typelist, Predicate>> >::type >
  , tail_t< dropWhile_t< typelist, Predicate > >
>;

template< typename T >
using add_clvalue_reference = std::add_lvalue_reference<std::add_const_t< T > >;

template< typename T >
using first_value_to_clvalue_t =
  apply_first_not_t< T, std::is_reference, add_clvalue_reference >;

template< typename T >
using first_value_to_rvalue_t =
  apply_first_not_t< T, std::is_reference, std::add_rvalue_reference >;

template< typename T >
using fork_value_t = id_t< T, first_value_to_rvalue_t<T>, first_value_to_clvalue_t<T> >;



#endif // __SIGNATURE_HPP__
