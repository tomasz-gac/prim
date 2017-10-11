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

template< typename R, typename args_t >
struct make_signature;

template< typename Return, typename... Args >
struct make_signature< Return, signature_args< Args... > >
{
  using type = Signature< Return(Args...) >;
};

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
struct fork_value{
  using type = id_t< T , first_value_to_rvalue_t<T>, first_value_to_clvalue_t<T> >;
};

template< typename T >
struct fork_values{
  using type = foldr_t< map_t< T, fork_value >, concat, id_t<T> >;
};

template< typename T >
static constexpr auto count_values = count< T, Not<std::is_reference>::template type >::value;

template< typename Op, typename T >
struct appl{
  using type = typename Op::template type<T>;
};

template< typename T >
struct generate_overloads{
private:
  using args = args_t<T>;
  static constexpr auto N = count_values< args >;
  
public:
  using type = map_t<
    foldr_t< repeat_t< N, bind1< fork_values > >, appl, id_t<args, args> >
  , bind< make_signature, return_t<T> >::template type
  >;
};


#endif // __SIGNATURE_HPP__
