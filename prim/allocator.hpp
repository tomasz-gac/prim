#ifndef __PRIM_ALLOCATOR_HPP__
#define __PRIM_ALLOCATOR_HPP__

#include <cstddef>
#include <cassert>
#include "builtins.hpp"
#include "allocator_traits.hpp"

namespace prim{

struct HeapAllocator
{
protected:
  template< typename ToAlloc >
  constexpr bool move_to( ToAlloc& other ){ return true; }

  void* allocate( prim::storage_info storage ){
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

  static constexpr bool can_store( size_t size_, size_t align_ ){
    return (size_ <= size && align_ <= alignment);
  }

  void* allocate( prim::storage_info storage ){
    assert( can_store( storage.size, storage.alignment ) );
    return reinterpret_cast<void*>(&buffer_);
  }

  void deallocate( void* data ){
    assert( data == reinterpret_cast<void*>(&buffer_) );
    //failure means that data was not allocated using this allocator
  };
private:
  std::aligned_storage_t< size, alignment > buffer_;

};

}

#endif // __PRIM_ALLOCATOR_HPP__

