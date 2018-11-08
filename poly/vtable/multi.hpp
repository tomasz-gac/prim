#ifndef __MULTI_HPP__
#define __MULTI_HPP__

#include <tuple>
#include "wrapper_traits.hpp"

namespace poly{

template< typename... Ts >
struct Multi{
  std::tuple<Ts...> data;
};

template< typename... Ts >
struct wrapper_traits< Multi<Ts...> >{
  template< size_t index, typename U >
  static decltype(auto) get(U&& u){
    return std::get<index>( u.data );
  }
};
  
}

#endif //  __MULTI_HPP__
