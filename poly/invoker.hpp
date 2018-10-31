#ifndef __SIGNATURE_HPP__
#define __SIGNATURE_HPP__

#include <type_traits>
#include "typelist.hpp"
#include "placeholder.hpp"

namespace poly{

struct Invalid;

namespace tl = typelist;
  
template< typename... Tags >
struct Interface : tl::unique_typelist<Tags...>{
public:
  using interface = Interface;

// private:  
//   static assert_unique_elements< Interface >
//     assert_unique_tags;
};

template< typename I >
using interface_t = typename I::interface;

template< typename T >
using overloads_t = typename T::overloads;

template< typename... Ts >
using join_t =
  tl::remove_duplicates_t< tl::concat_t< Interface<>, interface_t< Ts >... > >;


template< typename Interface, typename... Interfaces >
join_t< Interface, Interfaces... > interface( Interface, Interfaces... ){ return {}; }

template< typename Interface, typename Invoker, typename... Invokers >
constexpr bool supports(){
  return tl::disjunction<
    typename Interface::template contains< Invoker >,
    typename Interface::template contains< Invokers >...
    >();
}

// Type that encodes a signature for a given invoker
template< typename Tag, typename... >
class Invoker;

template< typename Tag, typename Return, typename... Args >
struct Invoker< Tag, Return(Args...)>
  : Interface< Tag >
  , tl::unique_typelist< Return(Args...)>
{
private:
  struct generate_overloads;
  
public:
  using overloads = typename generate_overloads::type;

  // template< typename... Ts >
  // static Return invoke( Ts&&... args ){
  //   return ::invoke( Tag(), std::forward<Ts>(args)... );
  // }
};

// Type that encodes a set of overloaded operations
template< typename Tag, typename Sig, typename... Sigs >
struct Invoker< Tag, Sig, Sigs... >
  : Interface< Tag >
  , tl::unique_typelist< Sig, Sigs... >
{
public:
  using overloads = tl::concat_t<
    overloads_t< Invoker< Tag, Sig> >, overloads_t<Invoker< Tag, Sigs>>... >;
  
private:
  // static assert_unique_elements< overloads >
  //   assert_unique_signatures;
};

template< typename T >
struct forward{
  using type = std::remove_reference_t<T>;
};

template< typename... >
struct signature_args;

template< typename Tag, typename R, typename args_t >
struct make_signature;

template< typename Tag, typename Return, typename... Args >
struct make_signature< Tag, Return, signature_args< Args... > >
{
  using type = Invoker< Tag, Return(Args...) >;
};

template< typename Tag, typename Return, typename args_t >
using make_signature_t = typename make_signature< Tag, Return, args_t >::type;

// helper template to contain overloads of an invoker
template< typename... >
struct overloads;

//template that computes a list of signatures required to perfect forward a function of signature T
template< typename Tag, typename Return, typename... Args >
struct Invoker< Tag, Return(Args...) >::generate_overloads{
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
    tl::concat_t<
      tl::takeWhile_t< typelist, Predicate >
    , tl::id_t< typelist, typename UnaryOp< tl::head_t<tl::dropWhile_t<typelist, Predicate>> >::type >
    , tl::tail_t< tl::dropWhile_t< typelist, Predicate > >
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
  using fork_value = tl::id_t< T , first_value_to_rvalue_t<T>, first_value_to_clvalue_t<T> >;
  // for each type in T, apply fork_value and flatten resulting list of lists
  template< typename T >
  using fork_values = tl::foldr_t< tl::map_t< T, fork_value >, tl::bind<2, tl::concat_t>::template type, tl::id_t<T> >;
  // number of types that need to be forked
  static constexpr auto N = tl::count< args, tl::Not<dont_overload>::template type >::value;

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
    tl::repack_t< //repack resulting list to "overloads" template
      tl::map_t< //list of signature_args -> list of signatures
        //apply fork_values N times to args repacked as signature_args< args >
	tl::foldr_t< tl::repeat_t< N, tl::bind<1,  fork_values > >, apply, tl::id_t<args, args> >
	, decltype(tl::curry< make_signature_t, Tag, Return >())::template type
      >
  , poly::overloads<>>;
};

template< typename... Tags1, typename... Tags2 >
tl::sum_unique_t< Interface< Tags1...>, Interface<Tags2...> >
operator+( const Interface< Tags1... >&, const Interface< Tags2... >& )
{ return {}; }

template< typename... Tags1, typename... Tags2 >
tl::diff_unique_t< Interface< Tags1...>, Interface< Tags2... > >
operator-( const Interface< Tags1... >&, const Interface< Tags2... >& )
{ return {}; }

}


#endif // __SIGNATURE_HPP__
