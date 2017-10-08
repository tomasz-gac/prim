#ifndef __TYPELIST_HPP__
#define __TYPELIST_HPP__

#include "bind.hpp"

template< typename T >
struct id;

template< typename typelist, typename... Ts >
using id_t = typename id<typelist>::template type<Ts...>;

// Template helper that returns a head and tail of typelist
template< typename typelist >
struct split_front;

template< typename typelist >
using head_t = typename split_front< typelist >::head;

template< typename typelist >
using tail_t = typename split_front< typelist >::tail;

template< typename typelist >
struct split_back;

template< typename typelist >
using last_t = typename split_back< typelist >::last;

template< typename typelist >
using init_t = typename split_back< typelist >::init;

template< typename >
struct length;

template< typename typelist >
constexpr std::size_t len(){ return length<typelist>{}; }

template< typename... >
struct _;

template< typename T >
using fst_t = head_t< T >;

template< typename T >
using snd_t = head_t< tail_t< T > >;

template< typename typelist, typename enumerator = std::make_index_sequence< len<typelist>() > >
struct enumerate;

template< typename typelist >
using enumerate_t = typename enumerate< typelist >::type;

template< typename typelist_t, template< typename... > class UnaryOp >
struct map;

template< typename typelist_t, template< typename... > class UnaryOp >
using map_t = typename map< typelist_t, UnaryOp >::type;

template< typename typelist, typename... typelists >
class concat;

template< typename typelist, typename... typelists >
using concat_t = typename concat< typelist, typelists... >::type;

template< typename typelist
  , template< typename... > class BinaryOp
  , typename init
> struct foldr;

template<   typename typelist
	  , template< typename... > class BinaryOp
          , typename init >
using foldr_t = typename foldr< typelist, BinaryOp, init >::type;

template< typename typelist, template< typename... > class UnaryOp >
struct filter;

template< typename typelist, template< typename... > class UnaryOp >
using filter_t = typename filter< typelist, UnaryOp >::type;





template<class...> struct disjunction : std::false_type { };
template<class B1> struct disjunction<B1> : B1 { };
template<class B1, class... Bn>
struct disjunction<B1, Bn...> 
  : std::conditional<bool(B1::value), B1, disjunction<Bn...>>::type  { };

template< template< typename... > class typelist, typename... Ts >
struct id< typelist<Ts...> >{
  template< typename... Us >
  using type = typelist<Us...>;
};

template< template< typename... > class typelist, typename T, typename... Ts >
struct split_front< typelist< T, Ts... > >
{
  using head = T;
  using tail = typelist<Ts...>;
};

template< template< typename... > class typelist, typename T, typename... Ts >
struct split_back< typelist< Ts..., T > >
{
  using last = T;
  using init = typelist<Ts...>;
};

template< template< typename... > class typelist, typename... Ts >
struct length< typelist< Ts... > >
  : std::integral_constant< std::size_t, sizeof...(Ts) >
{  };

template< typename typelist >
struct is_null : std::integral_constant< bool, length<typelist>{} == 0 >
{  };

template< typename typelist >
constexpr bool null(){ return is_null<typelist>{}; }

template< std::size_t i >
struct index : std::integral_constant< std::size_t, i >
{  };

template< template< typename... > class typelist, typename... Ts
	  , template< typename T, T... > class enumerator, typename U, U... us
> struct enumerate< typelist< Ts... >, enumerator< U, us... > >
{
  using type = typelist< _< index< us >, Ts > ... >;
};

template<
  template< typename... > class typelist
, typename... Ts
, template< typename... > class UnaryOp
  > struct map< typelist< Ts... >, UnaryOp >{
  using type = typelist< typename UnaryOp<Ts>::type... >;
};

template<
  template< typename... > class typelist
, typename... Ts1, typename... Ts2
, typename... typelists
> struct concat< typelist<Ts1...>, typelist< Ts2... >, typelists... >
  : concat< typelist< Ts1..., Ts2...>, typelists... >
{  };

template< template< typename... > class typelist, typename... Ts >
struct concat< typelist<Ts...> >
{ using type = typelist<Ts...>;  };

template<
  template< typename... > class typelist
  , typename T, typename... Ts
  , template< typename... > class BinaryOp
  , typename result
> struct foldr< typelist< T, Ts... >, BinaryOp, result >
  : foldr< typelist< Ts...>, BinaryOp, typename BinaryOp<T, result>::type >
{  };

template<
  template< typename... > class typelist
  , template< typename... > class BinaryOp
  , typename result
> struct foldr< typelist< >, BinaryOp, result >
{ using type = result; };

template< template< typename... > class UnaryOp >
struct erase_if
{
  template< typename T >
  using type = std::conditional< UnaryOp<T>::value, ::_<T>, ::_<> >;
};

template< template< typename... > class Op >
struct ignore{
  template< typename >
  using type = Op<>;
};

template<
  template< typename... > class Predicate
, template< typename... > class Then
, template< typename... > class Else = ignore< Then >::template type
> struct apply_if{
  template< typename T >
  using type = std::conditional< Predicate<T>::value, Then<T>, Else<T> >;
};

template< typename typelist, template< typename... > class UnaryOp >
struct filter
  : foldr<
    map_t< typelist, apply_if<UnaryOp, id<typelist>::template type>::template type >
  , concat, id_t<typelist> >
{  };

template<   typename typelist
          , template< typename... > class Predicate
          , typename result = id_t<typelist>
          , bool terminate = Predicate< head_t< typelist > >::value
> struct split_pred;

template< template< typename... > class typelist, typename T, typename... Ts
          , template< typename... > class Predicate
          , typename... Us
> struct split_pred< typelist< T, Ts... >, Predicate, typelist< Us... >, true >
  :  std::conditional_t< Predicate< T >::value
  , split_pred< typelist<Ts...>, Predicate, typelist<Us..., T >, true >
  , split_pred< typelist<T, Ts...>, Predicate, typelist< Us... >, false >
     >
{  };

template< typename right_t
          , template< typename... > class Predicate
	  , typename left_t
> struct split_pred< right_t, Predicate, left_t, false >
{
  using left = left_t;
  using right = right_t;
};

template< typename typelist, template< typename... > class Predicate >
using takeWhile_t = typename split_pred< typelist, Predicate >::left;

template< typename typelist, template< typename... > class Predicate >
using dropWhile_t = typename split_pred< typelist, Predicate >::right;

// Template helper that checks if a given type is in typelist
template< typename typelist_t, typename T >
struct in_typelist;

template< template< typename... > class typelist, typename T, typename... Ts >
struct in_typelist< typelist<Ts...>, T >
  : disjunction< std::is_same< T, Ts >... >
{  };

template< template< typename... > class Predicate >
struct Not{
  template< typename... Ts >
  struct type
    : std::integral_constant<bool, !Predicate<Ts...>::value >
  {  };
};

#endif //__TYPELIST_HPP__
