#ifndef __POLY_ALLOCATOR_HPP__
#define __POLY_ALLOCATOR_HPP__

#include <cstddef>
#include <cassert>
#include "builtins.hpp"
#include "allocator_traits.hpp"

namespace poly{

struct HeapAllocator
{
protected:
  void* allocate( poly::storage_info storage ){
    auto buffer = std::malloc( storage.size );
    if( buffer == nullptr ) throw std::bad_alloc{};
    return buffer;
  }

  void deallocate( void* data ){ 
    std::free( data );
  }
};

constexpr bool alloc_move_view( HeapAllocator& from, HeapAllocator& to ){ return true; }

template<>
struct alloc_optimize_move<HeapAllocator,HeapAllocator>
  : std::true_type
{  };

  
template< size_t Size, size_t Alignment = alignof(std::max_align_t) >
class StackAllocator{
public:
  static constexpr size_t size = Size;
  static constexpr size_t alignment = Alignment;
private:
  std::aligned_storage_t< size, alignment > buffer_;

protected:
  void* allocate( poly::storage_info storage ){
    assert( storage.size <= size && storage.alignment <= alignment );
    return reinterpret_cast<void*>(&buffer_);
  }

  void deallocate( void* data ){
    assert( data == reinterpret_cast<void*>(&buffer_) );
    //failure means that data was not allocated using this allocator
  };
};

}

#endif // __POLY_ALLOCATOR_HPP__

