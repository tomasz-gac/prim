#ifndef __ALLOCATOR_TRAITS_HPP__
#define __ALLOCATOR_TRAITS_HPP__
namespace poly{
  
template< typename FromAlloc, typename ToAlloc >
constexpr bool alloc_move_view( FromAlloc& from, ToAlloc& to ){ return false; }

template< typename FromAlloc, typename ToAlloc >
struct alloc_optimize_move : std::false_type {};

}

#endif // __ALLOCATOR_TRAITS_HPP__
