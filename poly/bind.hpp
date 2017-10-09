#ifndef __BIND_HPP__
#define __BIND_HPP__

template< template< typename > class Op >
static constexpr std::size_t size(){ return 1; }

template< template< typename,typename > class Op >
static constexpr std::size_t size(){ return 2; }

template< template< typename,typename,typename > class Op >
static constexpr std::size_t size(){ return 3; }

template< template< typename,typename,typename,typename > class Op >
static constexpr std::size_t size(){ return 4; }

template< template< typename,typename,typename,typename,typename > class Op >
static constexpr std::size_t size(){ return 5; }

template< template< typename,typename,typename,typename,typename,typename > class Op >
static constexpr std::size_t size(){ return 6; }

template< template< typename,typename,typename,typename,typename,typename,typename > class Op >
static constexpr std::size_t size(){ return 7; }

template< template< typename,typename,typename,typename,typename,typename,typename,typename > class Op >
static constexpr std::size_t size(){ return 8; }

template< template< typename,typename,typename,typename,typename,typename,typename,typename,typename > class Op >
static constexpr std::size_t size(){ return 9; }

template< template< typename... > class Op, typename... Ts >
struct bind{
  template< typename... Us >
  using type = typename Op<Ts..., Us... >::type;
  
  template< typename... Us >
  struct value__{
    static constexpr auto value = Op<Ts..., Us... >::value;
  };

  template< typename... Us >
  using value = value__<Us...>;
};



#endif // __BIND_HPP__
