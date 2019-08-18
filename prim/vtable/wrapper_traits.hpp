#ifndef __WRAPPER_TRAITS_HPP__
#define  __WRAPPER_TRAITS_HPP__

namespace prim{

template< typename T >
struct wrapper_traits{
  template< size_t index, typename U >
  static decltype(auto) get( U&& u ){
    static_assert( index == 0, "Cannot use enumerated placeholders for non-wrapper types" );
    return std::forward<U>(u);
  }
};

}

#endif //__WRAPPER_TRAITS_HPP__
