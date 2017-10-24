#ifndef __STORAGE_HPP__
#define __STORAGE_HPP__

#include <memory>

template< typename Interface, std::size_t Size, std::size_t Align = 0 >
class sbo_storage{
public:
  static constexpr std::size_t size = Size;
  static constexpr std::size_t align = Align == 0 ? alignof( std::aligned_storage< size > ) : Align;

private:
  using static_storage = std::aligned_storage_t< size, align >;
public:
  template< typename T >
  sbo_storage( T&& value ){
    auto buffer = &static_;
    size_t space = size;
    if( std::align( alignof(T), size, buffer, space ) ){
      data_ = new(&buffer) T(std::forward<T>(value) );
      is_static_ = true;
    } else {
      data_ = new T(std::forward<T>(value) );
      is_static_ = false;      
    }
  }

  ~sbo_storage()
  
private:
  bool is_static_;
  Interface* data_;
  static_storage storage_;  
};






#endif // __STORAGE_HPP__
