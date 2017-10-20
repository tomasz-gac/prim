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

template< typename Return, typename args_t >
using make_signature_t = typename make_signature< Return, args_t >::type;

// apply UnaryOp on first element of typelist that fails Predicate
template<
  typename typelist
, template< typename > class Predicate
, template< typename > class UnaryOp
>
using apply_first_not_t =
  concat_t<
    takeWhile_t< typelist, Predicate >
  , id_t< typelist, typename UnaryOp< head_t<dropWhile_t<typelist, Predicate>> >::type >
  , tail_t< dropWhile_t< typelist, Predicate > >
>;

// helper template to contain overloads for Holder implementation
template< typename... >
struct overloads;

template< typename T >
struct forward{
  using type = std::remove_reference_t<T>;
};

template< typename T >
struct unforward__{ using type = T; };

template< typename T >
struct unforward__<forward<T>>{ using type = typename forward<T>::type; };

template< typename T >
using unforward = typename unforward__<T>::type;

template< typename T >
struct is_forwarded
  : std::integral_constant< bool, !std::is_same< T, unforward<T> >::value >
{  };

// helper predicate that decides if a given type needs to be forwarded by const& and &&
template< typename U >
struct dont_overload
  : std::integral_constant< bool, !is_forwarded< U >::value >
{ };

//template that computes a list of signatures required to perfect forward a function of signature T
template< typename T >
struct generate_overloads{
private:
  // U -> const U&
  template< typename U >
  using add_clvalue_reference =
    std::add_lvalue_reference<std::add_const_t< unforward<U> > >;

  // U -> U&&
  template< typename U >
  using add_rvalue_reference =
    std::add_rvalue_reference< unforward<U> >;
  
  //first value that fails dont_overload -> const&
  template< typename U >
  using first_value_to_clvalue_t =
    apply_first_not_t< U, dont_overload, add_clvalue_reference >;
  //first value that fails dont_overload -> &&
  template< typename U >
  using first_value_to_rvalue_t =
    apply_first_not_t< U, dont_overload, add_rvalue_reference >;
  //first value that fails dont_overload -> [ &&, const& ]
  template< typename U >
  using fork_value = id_t< U , first_value_to_rvalue_t<U>, first_value_to_clvalue_t<U> >;
  // for each type in U, apply fork_value and flatten resulting list of lists
  template< typename U >
  using fork_values = foldr_t< map_t< U, fork_value >, bind<2, concat_t>::template type, id_t<U> >;
  // T -> signature_args< T's args >
  using args = args_t<T>;
  // number of types that need to be forked
  static constexpr auto N = count< args, Not<dont_overload>::template type >::value;

  #ifndef POLY_MAX_FORWARDED_ARGS
  #define POLY_MAX_FORWARDED_ARGS 3

  static_assert( N <= POLY_MAX_FORWARDED_ARGS,
  		 "Signature of the invoker exceeds maximum arguments to be forwarded" );

  #undef POLY_MAX_FORWARDED_ARGS
  #endif
  
  // helper template to apply a bound Op
  template< typename Op, typename U >
  using apply = typename Op::template type<U>;
  
public:
  using type =
    repack_t< //repack resulting list to "overloads" template
      map_t< //list of signature_args -> list of signatures
        //apply fork_values N times to args repacked as signature_args< args >
        foldr_t< repeat_t< N, bind<1,  fork_values > >, apply, id_t<args, args> >
      , decltype(curry< make_signature_t, return_t<T> >())::template type
      >
    , overloads<>>;
};

#endif // __SIGNATURE_HPP__
