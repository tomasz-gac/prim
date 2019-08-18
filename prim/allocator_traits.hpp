#ifndef __ALLOCATOR_TRAITS_HPP__
#define __ALLOCATOR_TRAITS_HPP__
namespace prim{
  
template< typename FromAlloc, typename ToAlloc >
struct allocator_traits{
  // if set to true, then FromAlloc can be moved to ToAlloc
  // Moving is done via FromAlloc's member function:
  // bool FromAlloc::*move_to( ToAlloc& )
  // Returns true if successful, false otherwise
  static constexpr bool optimize_move = false;
};
}

#endif // __ALLOCATOR_TRAITS_HPP__
