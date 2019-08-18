#ifndef __BIND_HPP__
#define __BIND_HPP__

namespace typelist{

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

template< std::size_t, template< typename... > class Op, typename... Ts >
struct bind;

template< template< typename... > class Op, typename... Ts >
std::enable_if_t<
  (size<Op>() > sizeof...(Ts) )
  , bind< size<Op>() - sizeof...(Ts), Op, Ts... >
> curry();


template< template< typename... > class Op, typename... Ts >
struct bind< 1, Op, Ts... >{
  template< typename T1 >
  using type = Op<Ts..., T1 >;
  
  template< typename T1 >
  struct value__{
    static constexpr auto value = Op<Ts..., T1 >::value;
  };

  template< typename T1 >
  using value = value__<T1>;
};

template< template< typename... > class Op, typename... Ts >
struct bind< 2, Op, Ts... >{
  template< typename T1, typename T2 >
  using type = Op<Ts..., T1, T2 >;
  
  template< typename T1, typename T2 >
  struct value__{
    static constexpr auto value = Op<Ts..., T1, T2 >::value;
  };

  template< typename T1, typename T2 >
  using value = value__<T1, T2>;
};

template< template< typename... > class Op, typename... Ts >
struct bind< 3, Op, Ts... >{
  template< typename T1, typename T2, typename T3 >
  using type = Op<Ts..., T1, T2, T3 >;
  
  template< typename T1, typename T2, typename T3 >
  struct value__{
    static constexpr auto value = Op<Ts..., T1, T2, T3 >::value;
  };

  template< typename T1, typename T2, typename T3 >
  using value = value__<T1, T2,T3>;
};

template< template< typename... > class Op, typename... Ts >
struct bind< 4, Op, Ts... >{
  template< typename T1, typename T2, typename T3, typename T4 >
  using type = Op<Ts..., T1, T2, T3, T4 >;
  
  template< typename T1, typename T2, typename T3, typename T4 >
  struct value__{
    static constexpr auto value = Op<Ts..., T1, T2, T3, T4 >::value;
  };

  template< typename T1, typename T2, typename T3, typename T4 >
  using value = value__<T1, T2,T3,T4>;
};


}

#endif // __BIND_HPP__
