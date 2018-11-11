#ifndef __SBO_ALLOCATOR_HPP__
#define __SBO_ALLOCATOR_HPP__

#include <cstddef>
#include <cassert>

#include "allocator_traits.hpp"
#include "allocator.hpp"
#include "vtable/JumpVTable.hpp"
#include "builtins.hpp"

namespace poly{



  
template< std::size_t size, std::size_t align = alignof(std::max_align_t) >
class SboAllocator :
  private StackAllocator<size,align>,
  private HeapAllocator
{
private:
  struct allocate_   : poly::Invoker< allocate_,   void* ( poly::T&, const poly::storage_info& ) >{  };
  struct deallocate_ : poly::Invoker< deallocate_, void ( poly::T&, void* ) >{  };

  template< typename Alloc >
  friend void* invoke( allocate_, Alloc& alloc, const poly::storage_info& info ){
    return alloc.allocate( info );
  }

  template< typename Alloc >
  friend void invoke( deallocate_, Alloc& alloc, void* ptr ){
    alloc.deallocate( ptr );
  }

  struct sbo_interface
    : poly::Interface< allocate_, deallocate_ >
  {  };

  using stack_alloc_t = StackAllocator<size,align>;
  using heap_alloc_t  = HeapAllocator;
  using vtable_t = poly::JumpVT< sbo_interface, stack_alloc_t, heap_alloc_t >;
  using vtable_impl_t = poly::impl_t<vtable_t>;
  
public:
  SboAllocator()
    : this_( static_cast<stack_alloc_t*>(this) )
  {  }

  void* allocate( poly::storage_info info ){
    if( stack_alloc_t::can_store( info.size, info.alignment ) ){
      this_ = static_cast<stack_alloc_t*>(this);
    } else {
      this_ = static_cast<heap_alloc_t*>(this);
    }
    return poly::call<allocate_>( *this_, info );
  }

  void deallocate( void* ptr ){
    poly::call<deallocate_>(*this_, ptr);
  }

  bool move_to( SboAllocator& other ){
    if( this_.vtable().index() == 1 ){ 
      other.this_ = this_;
      return true;
    }
    return false;
  }
  

private:
  poly::Pointer< vtable_t > this_;
};

template<size_t s1, size_t a1, size_t s2, size_t a2>
struct allocator_traits<SboAllocator<s1, a1>, SboAllocator<s2, a2>>
{
  static constexpr bool optimize_move = true;
};

}

#endif // __SBO_ALLOCATOR_HPP__
