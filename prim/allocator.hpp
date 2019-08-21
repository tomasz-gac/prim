#ifndef __PRIM_ALLOCATOR_HPP__
#define __PRIM_ALLOCATOR_HPP__

#include <cstddef>
#include <cassert>
#include <memory>
#include "builtins.hpp"
#include "allocator_traits.hpp"

namespace prim{

struct HeapAllocator
{
protected:
  template< typename ToAlloc >
  constexpr bool move_to( ToAlloc& other ){ return true; }

  void* allocate( prim::type_info storage ){
    auto buffer = std::malloc( storage.size );
    if( buffer == nullptr ) throw std::bad_alloc{};
    return buffer;
  }

  void deallocate( void* data ){ 
    std::free( data );
  }
};

template<>
struct allocator_traits<HeapAllocator,HeapAllocator>
{
  static constexpr bool optimize_move = true;
};


  
template< size_t Size, size_t Alignment = alignof(std::max_align_t) >
class StackAllocator{
protected:
  static constexpr size_t size = Size;
  static constexpr size_t alignment = Alignment;

  void* allocate_unsafe( prim::type_info storage ){
    void* ptr = reinterpret_cast<void*>(&buffer_);
    size_t sz = size;
    std::align(storage.alignment, storage.size, ptr, sz );
    assert( ptr );
    return ptr;
  }

  void* allocate( prim::type_info storage ){
    void* ptr = allocate_unsafe( storage );
    assert( ptr );
    return ptr;
  }

  void deallocate( void* data ){
    const void* begin = reinterpret_cast<void*>(&buffer_);
    const void* end   = reinterpret_cast<void*>(&buffer_+1);
    assert( begin <= data && data < end  );
    //failure means that data was not allocated using this allocator
  };
private:
  std::aligned_storage_t< size, alignment > buffer_;

};

}

#endif // __PRIM_ALLOCATOR_HPP__

