#ifndef __IMPLEMENTATION_HPP__
#define __IMPLEMENTATION_HPP__

namespace poly{

template< typename T >
using impl_t =  typename T::implementation;

template< typename Tag >
struct Implementation{
  using implementation = Tag;
};

}
#endif // __IMPLEMENTATION_HPP__
