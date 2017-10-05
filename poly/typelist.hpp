#ifndef __TYPELIST_HPP__
#define __TYPELIST_HPP__

template<class...> struct disjunction : std::false_type { };
template<class B1> struct disjunction<B1> : B1 { };
template<class B1, class... Bn>
struct disjunction<B1, Bn...> 
  : std::conditional<bool(B1::value), B1, disjunction<Bn...>>::type  { };


// Template helper that returns a head of typelist or default
template< template< typename... > class typelist, typename T, typename U = void >
struct head_of
{ using type = U;  };

template< template< typename... > class typelist, typename T, typename... Ts, typename U >
struct head_of< typelist, typelist< T, Ts... >, U >
{ using type = T; };

template< template< typename... > class typelist, typename T, typename U = void >
using head_t = typename head_of< typelist, T, U >::type;

// Template helper that checks if a given type is in typelist
template< template< typename... > class typelist, typename typelist_t, typename T >
struct in_typelist;

template< template< typename... > class typelist, typename T, typename... Ts >
struct in_typelist< typelist, typelist<Ts...>, T >
  : disjunction< std::is_same< T, Ts >... >
{  };

template<
  template< typename... > class typelist
, typename typelist_t
, template< typename > class UnaryOp
  > struct transform;

template<
  template< typename... > class typelist
, typename... Ts
, template< typename > class UnaryOp
  > struct transform< typelist, typelist< Ts... >, UnaryOp >{
  using type = typelist< typename UnaryOp< Ts >::type... >;
};

template<   template< typename... > class typelist
	  , typename typelist_t
	  , template< typename > class UnaryOp
> using transform_t = typename transform< typelist, typelist_t, UnaryOp >::type;








#endif //__TYPELIST_HPP__
