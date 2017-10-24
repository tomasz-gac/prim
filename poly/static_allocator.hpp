#ifndef __STATIC_ALLOCATOR_HPP__
#define __STATIC_ALLOCATOR_HPP__

#include <memory>

  template< size_t... i >
  struct print_v;


template< typename T, typename Storage >
constexpr bool fits(){
  if( alignof(Storage) > alignof(T) )
    return (sizeof(Storage) >= sizeof(T));
  constexpr size_t offset =
       alignof(T) - alignof(Storage);
  print_v<offset, sizeof(Storage), sizeof(T), alignof(T), alignof(Storage)> s;
  return sizeof(Storage) > (offset + sizeof(T));
};

template< size_t Size, size_t Align = 0 >
class static_allocator{
public:
  static constexpr size_t size = Size;
  static constexpr size_t align = Align == 0 ? alignof( std::aligned_storage<size> ) : Align;

private:
  using storage_type = std::aligned_storage< size, align >;

public:
  template< typename T, typename... Args >
  T* allocate( Args&&... args ){
    static_assert( fits< T, storage_type >(), "The type T does not fit into the buffer" );
  }

private:
  storage_type buffer_;
};


#endif // __STATIC_ALLOCATOR_HPP__
