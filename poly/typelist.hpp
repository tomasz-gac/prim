#ifndef __TYPELIST_HPP__
#define __TYPELIST_HPP__

#include "bind.hpp"

template< typename T >
struct id;

template< typename typelist, typename... Ts >
using id_t = typename id<typelist>::template type<Ts...>;

template< typename From, typename To >
struct repack;

template<
  template< typename... > class tl1, typename... Ts
, template< typename... > class tl2, typename... Us
> struct repack< tl1<Ts...>, tl2<Us...> >{
  using type = tl2<Ts...>;
};

template< typename typelist1, typename typelist2 >
using repack_t = typename repack< typelist1, typelist2 >::type;


// Template helper that returns a head and tail of typelist
template< typename typelist >
struct split_front;

template< typename typelist >
using head_t = typename split_front< typelist >::head;

template< typename typelist >
using tail_t = typename split_front< typelist >::tail;

template< typename... >
struct _;

template< typename typelist_t, template< typename > class UnaryOp >
struct map;

template< typename typelist_t, template< typename > class UnaryOp >
using map_t = typename map< typelist_t, UnaryOp >::type;

template< typename typelist, typename... typelists >
class concat;

template< typename typelist, typename... typelists >
using concat_t = typename concat< typelist, typelists... >::type;

template< typename typelist >
struct flatten;

template< typename T >
using flatten_t = typename flatten<T>::type;

template< typename typelist
  , template< typename, typename > class BinaryOp
  , typename init
> struct foldr;

template<   typename typelist
          , template< typename, typename > class BinaryOp
          , typename init >
using foldr_t = typename foldr< typelist, BinaryOp, init >::type;

template< typename typelist, template< typename > class UnaryOp >
struct filter;

template< typename typelist, template< typename > class UnaryOp >
using filter_t = typename filter< typelist, UnaryOp >::type;

template< std::size_t N, typename T, typename = _<> >
struct repeat;

template< std::size_t N, typename T, typename typelist = _<> >
using repeat_t = typename repeat< N, T, typelist >::type;




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

template<
  template< typename... > class typelist
, typename... Ts
, template< typename > class UnaryOp
  > struct map< typelist< Ts... >, UnaryOp >{
  using type = typelist< UnaryOp<Ts>... >;
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
  , template< typename, typename > class BinaryOp
  , typename result
> struct foldr< typelist< T, Ts... >, BinaryOp, result >
  : foldr< typelist< Ts...>, BinaryOp, BinaryOp<T, result> >
{  };

template<
  template< typename... > class typelist
  , template< typename, typename > class BinaryOp
  , typename result
> struct foldr< typelist< >, BinaryOp, result >
{ using type = result; };

template< template< typename... > class UnaryOp >
struct erase_if
{
  template< typename T >
  using type = std::conditional< UnaryOp<T>::value, ::_<T>, ::_<> >;
};

template< template< typename... > class typelist, typename... Ts >
struct flatten< typelist< Ts... > >
  : concat< Ts... >
{  };

template< template< typename... > class Op >
struct ignore{
  template< typename >
  using type = Op<>;
};

template<
  template< typename > class Predicate
, template< typename > class Then
, template< typename > class Else = ignore< Then >::template type
> struct apply_if{
  template< typename T >
  using type = std::conditional< Predicate<T>::value, Then<T>, Else<T> >;
};

template< typename typelist, template< typename > class UnaryOp >
struct filter
  : foldr<
    map_t< typelist, apply_if<UnaryOp, id<typelist>::template type>::template type >
  , bind< 2, concat>::template type, id_t<typelist> >
{  };

template< typename T, T v >
struct v_{
  using type = T;
  static constexpr T value = v;
};

template< template< typename > class Predicate >
struct apply{
  template< typename T >
  using type = v_< std::remove_cv_t<decltype(Predicate<T>::value)>, Predicate<T>::value >;
};

template<
  typename typelist
, template< typename > class Predicate
, typename passed = id_t< typelist >
, typename values = map_t< typelist, apply<Predicate>::template type >
> struct split_pred;

template<
  template< typename... > class typelist, typename T, typename... Ts,
  template< typename > class Pred, typename... Us,
  bool... tail >
struct split_pred<
  typelist<T, Ts...>, Pred, typelist<Us...>
, typelist< v_<bool, true >, v_< bool, tail >... >
  > : split_pred< typelist< Ts... >, Pred, typelist< Us..., T >, typelist< v_<bool, tail >... > >
{  };

template<
  template< typename... > class typelist, typename... Ts,
  template< typename > class Pred, typename... Us,
  bool... tail >
struct split_pred< typelist< Ts...>, Pred, typelist<Us...>
, typelist< v_<bool, false >, v_< bool, tail >... >
>{
  using take = typelist< Us... >;
  using drop = typelist< Ts... >;
};

template<
  template< typename... > class typelist,
  template< typename > class Pred, typename... Us
>
struct split_pred< typelist<>, Pred, typelist<Us...>, typelist< > >
{
  using take = typelist< Us... >;
  using drop = typelist<>;
};


template< typename typelist, template< typename > class Predicate >
using takeWhile_t = typename split_pred< typelist, Predicate >::take;

template< typename typelist, template< typename > class Predicate >
using dropWhile_t = typename split_pred< typelist, Predicate >::drop;

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

template< std::size_t N, typename T, typename... Ts, template< typename... > class typelist >
struct repeat< N, T, typelist< Ts... > >
  : repeat< N-1, T, typelist< Ts..., T > >
{  };

template< typename T, typename... Ts, template< typename... > class typelist >
struct repeat< 0, T, typelist< Ts... > >{
  using type = typelist<Ts...>;
};

template< typename T, T... v >
struct sum;

template< typename T, T v1, T... vs >
struct sum< T, v1, vs... > : std::integral_constant< T, v1 + sum< T, vs... >::value >
{  };

template< typename T >
struct sum< T > : std::integral_constant< T, 0 >
{  };


template< typename typelist, template< typename... > class Pred >
struct count;

template< template< typename... > class typelist, typename... Ts, template< typename... > class Pred >
struct count< typelist< Ts... >, Pred >
  : sum< std::size_t, (Pred<Ts>::value ? 1 : 0 )... >
{  };

#endif //__TYPELIST_HPP__
