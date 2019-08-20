#ifndef __SBO_ALLOCATOR_HPP__
#define __SBO_ALLOCATOR_HPP__

#include <cstddef>
#include <cassert>

#include "allocator_traits.hpp"
#include "allocator.hpp"
#include "vtable/JumpVTable.hpp"
#include "builtins.hpp"

namespace prim{



  
template< std::size_t size, std::size_t align = alignof(std::max_align_t) >
class SboAllocator :
  private StackAllocator<size,align>,
  private HeapAllocator
{
private:
  using stack_alloc_t = StackAllocator<size,align>;
  using heap_alloc_t  = HeapAllocator;
  
public:
  SboAllocator(){  }

  void* allocate( prim::storage_info info ){
    if( void* ptr = this->stack_alloc_t::allocate_unsafe( info ) ){
      on_heap_ = false;
      return ptr;
    }
    on_heap_ = true;
    return this->heap_alloc_t::allocate( info );
  }

  void deallocate( void* ptr ){
    if( on_heap_ ) this->heap_alloc_t::deallocate( ptr );
    else this->stack_alloc_t::deallocate( ptr );
  }

  bool move_to( SboAllocator& other ){
    if( on_heap_ ){ 
      other.on_heap_ = true;
      return true;
    }
    return false;
  }
  

private:
  bool on_heap_ = false;
};

template<size_t s1, size_t a1, size_t s2, size_t a2>
struct allocator_traits<SboAllocator<s1, a1>, SboAllocator<s2, a2>>
{
  static constexpr bool optimize_move = true;
};

}

#endif // __SBO_ALLOCATOR_HPP__
