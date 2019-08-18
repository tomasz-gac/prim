#ifndef __WRAPPER_TRAITS_HPP__
#define  __WRAPPER_TRAITS_HPP__

namespace prim{

template< typename T >
struct wrapper_traits{

  // specialization for wrapper types requires that get<0>
  // returns the wrapper object itself so that it can be stored in value
  template< size_t index, typename U >
  static decltype(auto) get( U&& u ){
    static_assert( index == 0, "Cannot use enumerated placeholders for non-wrapper types" );
    return std::forward<U>(u);
  }
};

}

#endif //__WRAPPER_TRAITS_HPP__
