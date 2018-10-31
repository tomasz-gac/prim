#ifndef __STATIC_ALLOCATOR_HPP__
#define __STATIC_ALLOCATOR_HPP__

#include <memory>
#include <cassert>

template< typename T, typename Storage >
constexpr bool fits(){
  if( alignof(Storage) >= alignof(T) )
    return (sizeof(Storage) >= sizeof(T));
  constexpr size_t offset =
       alignof(T) - alignof(Storage);
  return sizeof(Storage) >= (offset + sizeof(T));
};

template< size_t Size, size_t Align = 0 >
class static_allocator{
public:
  static constexpr size_t size = Size;
  static constexpr size_t align = Align == 0 ? alignof( std::aligned_storage<size> ) : Align;

private:
  using storage_type = std::aligned_storage_t< size, align >;

public:
  template< typename T, typename... Args >
  T* allocate( Args&&... args ){
    static_assert( fits< T, storage_type >(), "The type T does not fit into the buffer" );
    void* ptr = reinterpret_cast< void* >(&buffer_);
    std::size_t sz = size;
    auto aligned = std::align( alignof(T), size, ptr, sz );
    assert( aligned );
    return new( ptr ) T(std::forward<Args>(args)...);    
  }

  template< typename T >
  void deallocate( T* ){ };

private:
  storage_type buffer_;
  void* current_;
};


#endif // __STATIC_ALLOCATOR_HPP__
