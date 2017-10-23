#ifndef __STORAGE_HPP__
#define __STORAGE_HPP__

template< typename >
class storage_traits;

template< typename T, typename Storage  >
constexpr bool fits(){
  return
    sizeof(T) <= sizeof( Storage ) &&
    alignof( T ) <= alignof( Storage ) &&
    alignof( Storage ) % alignof( T ) == 0;
}


template< std::size_t Size, std::size_t Align = 0 >
class static_storage{
public:
  static constexpr std::size_t size = Size;
  static constexpr std::size_t align = Align == 0 ? alignof( std::aligned_storage< size > ) : Align;

private:
  using storage_type = std::aligned_storage_t< size, align >;
public:
  template< typename T >
  T* allocate( 
  
private:
  storage_type buffer_;
};











#endif // __STORAGE_HPP__
