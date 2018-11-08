#ifndef __MULTI_HPP__
#define __MULTI_HPP__

#include <tuple>

namespace poly{

template< typename T >
struct is_multi_impl : std::false_type {  };

template< typename... Ts >
struct Multi{
  std::tuple<Ts...> data;
};

template< typename... Ts >
struct is_multi_impl<Multi<Ts...>>
  : std::false_type
{  };

template< typename T >
static constexpr bool is_multi =
  is_multi_impl<std::remove_const_t<std::decay_t<T>>>::value;
  
}

#endif //  __MULTI_HPP__
