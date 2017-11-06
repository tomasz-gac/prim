#ifndef __SIGNATURE_HPP__
#define __SIGNATURE_HPP__

#include <type_traits>
#include "typelist.hpp"
#include "placeholder.hpp"

// helper template to contain overloads for Holder implementation
template< typename... >
struct overloads;

// Type that encodes a signature for a given invoker
// Invokers are meant to derive from this type
template< typename >
class Signature;

template< typename Return, typename... Args >
struct Signature< Return(Args...)>{
private:
  struct generate_overloads;
  
public:
  using overloads_type = typename generate_overloads::type;
  using signature_type = Signature;
};

template< typename T >
using overloads_t = typename T::overloads_type;

template< typename T >
using signature_t = typename T::signature_type;

template< typename Tag, typename... Tags >
struct Overloaded
{
public:
  using signature_type = Overloaded;
  using overloads_type = concat_t< overloads_t< Tag >, overloads_t<Tags>... >;
private:
  static assert_unique_elements< overloads_type >
    assert_unique_signatures;
};


// Helper template for overload resolution of Overloaded base class
// This template is used for Signature< ... > Invokers
template< typename >
struct unknown_invoker;

// Implements resolve function for all overloads of a Signature
// Resolves into Invoker specified as template parameter of resolve function
template< typename Return, typename... Args, typename... Other >
struct unknown_invoker< overloads< Signature< Return(Args...) >, Other... > >
  : unknown_invoker< overloads< Other... > >
{
  using unknown_invoker< overloads< Other... > >::resolve;
  
  template< typename Invoker >
  static Invoker resolve( Args... );  
};

template< typename Return, typename... Args >
struct unknown_invoker< overloads< Signature< Return(Args...) > > >
{
  template< typename Invoker >
  static Invoker resolve( Args... );  
};

// Helper template for overload resolution of Overloaded base class
// This template is used for Overloaded< ... > Invokers
template< typename Invoker, typename = overloads_t< Invoker > >
struct overloaded_invoker;


// Implements resolve function for all overloads of a Overloaded< ... >
// Resolves into Invoker specified as a part of Overloaded base
template< typename Invoker, typename Return, typename... Args, typename... Other >
struct overloaded_invoker< Invoker, overloads< Signature< Return(Args...) >, Other... > >
  : overloaded_invoker< Invoker, overloads< Other... > >
{
  using overloaded_invoker< Invoker,overloads< Other... > >::resolve;
  
  template< typename >
  static Invoker resolve( Args... );  
};

template< typename Invoker, typename Return, typename... Args >
struct overloaded_invoker< Invoker, overloads< Signature< Return(Args...) > > >
{
  template< typename >
  static Invoker resolve( Args... );  
};

// Template that merges unknown_invoker and overloaded_invoker into a branch for resolution
template< typename Invoker >
struct invoker_resolution;

template< typename Return, typename... Args >
struct invoker_resolution< Signature<Return(Args...)> >
  : unknown_invoker< overloads_t<Signature<Return(Args...)>> >
{
  using unknown_invoker< overloads_t<Signature<Return(Args...)>> >::resolve;
};

template< typename Tag, typename... Tags >
struct invoker_resolution< Overloaded< Tag, Tags... > >
  : invoker_resolution< Overloaded< Tags... > >
  , overloaded_invoker< Tag >
{
  using invoker_resolution< Overloaded< Tags... > >::resolve;
  using overloaded_invoker< Tag >::resolve;
};

template< typename Tag >
struct invoker_resolution< Overloaded< Tag > >
  : overloaded_invoker< Tag >
{
  using overloaded_invoker< Tag >::resolve;
};

//Template that returns a specific invoker to pass to invoke<> based on Args
template< typename Invoker, typename... Args >
using resolve_invoker =
  decltype( invoker_resolution< signature_t< Invoker > >
	    ::template resolve<Invoker>(std::declval<Args>()...)
	    );


template< typename T >
struct forward{
  using type = std::remove_reference_t<T>;
};

template< typename... >
struct signature_args;

template< typename R, typename args_t >
struct make_signature;

template< typename Return, typename... Args >
struct make_signature< Return, signature_args< Args... > >
{
  using type = Signature< Return(Args...) >;
};

template< typename Return, typename args_t >
using make_signature_t = typename make_signature< Return, args_t >::type;


//template that computes a list of signatures required to perfect forward a function of signature T
template< typename Return, typename... Args >
struct Signature< Return(Args...) >::generate_overloads{
private:
  // T -> signature_args< T's args >
  using args = signature_args< Args... >;

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
  template< typename T >
  struct dont_overload
    : std::integral_constant< bool, !is_forwarded< T >::value >
  { };
  
  
  // U -> const T&
  template< typename T >
  using add_clvalue_reference =
    std::add_lvalue_reference<std::add_const_t< unforward<T> > >;

  // T -> T&&
  template< typename T >
  using add_rvalue_reference =
    std::add_rvalue_reference< unforward<T> >;
  
  //first value that fails dont_overload -> const&
  template< typename T >
  using first_value_to_clvalue_t =
    apply_first_not_t< T, dont_overload, add_clvalue_reference >;
  //first value that fails dont_overload -> &&
  template< typename T >
  using first_value_to_rvalue_t =
    apply_first_not_t< T, dont_overload, add_rvalue_reference >;
  //first value that fails dont_overload -> [ &&, const& ]
  template< typename T >
  using fork_value = id_t< T , first_value_to_rvalue_t<T>, first_value_to_clvalue_t<T> >;
  // for each type in T, apply fork_value and flatten resulting list of lists
  template< typename T >
  using fork_values = foldr_t< map_t< T, fork_value >, bind<2, concat_t>::template type, id_t<T> >;
  // number of types that need to be forked
  static constexpr auto N = count< args, Not<dont_overload>::template type >::value;

  #ifndef POLY_MAX_FORWARDED_ARGS
  #define POLY_MAX_FORWARDED_ARGS 3

  static_assert( N <= POLY_MAX_FORWARDED_ARGS,
  		 "Signature of the invoker exceeds maximum arguments to be forwarded" );

  #undef POLY_MAX_FORWARDED_ARGS
  #endif
  
  // helper template to apply a bound Op
  template< typename Op, typename T >
  using apply = typename Op::template type<T>;
  
public:
  using type =
    repack_t< //repack resulting list to "overloads" template
      map_t< //list of signature_args -> list of signatures
        //apply fork_values N times to args repacked as signature_args< args >
        foldr_t< repeat_t< N, bind<1,  fork_values > >, apply, id_t<args, args> >
      , decltype(curry< make_signature_t, Return >())::template type
      >
    , overloads<>>;
};

template< typename T >
struct type;

//calls generate_overloads on Interface if it is non-empty.
template< typename Interface >
using interface_overloads_t = typename
  std::conditional_t< length<Interface>::value == 0,
		      overloads<>,
		      overloads_t<head_or_t<Interface,Signature< void()>>> >;


#endif // __SIGNATURE_HPP__
