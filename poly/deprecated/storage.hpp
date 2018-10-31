#ifndef __STORAGE_HPP__
#define __STORAGE_HPP__

#include <memory>
#include "vtable.hpp"

struct destruct : Signature< void( const T& ) >{  };

template< typename T >
constexpr auto invoke< const destruct, T > = []( const T& object ){ object.~T(); };

template< typename Interface >
class Manager{
public:
  void* data;
  VTable< Interface > vtable;

  Manager( void* data_, VTable< Interface > vtable_ )
    : data(data), vtable(vtable_)
  {  }
protected:
  ~Manager() = default;
};

template< typename Interface >
struct Ownership : Manager< Interface >
{
  static_assert( in_typelist< Interface, const destruct >::value, "Interface has to support destruction");
  using Manager<Interface>::Manager;

  ~Ownership(){
    vtable_.template call< const destruct >( data_ );
  }
};

template< typename Interface >
struct View : Manager< Interface >
{
  using Manager<Interface>::Manager;
};

template< std::size_t Size >
class sbo_storage{
public:
  static constexpr std::size_t size = Size;
  static constexpr std::size_t align = alignof( std::max_align_t );

private:
  using static_storage = std::aligned_storage_t< size, align >;
public:
  template< typename T >
  sbo_storage( T&& value ){
    if( sizeof( T ) <= size ){
      data_ = new(&buffer) T(std::forward<T>(value) );
      is_static_ = true;
    } else {
      data_ = new T(std::forward<T>(value) );
      is_static_ = false;      
    }
  }

private:
  bool is_static_;
  static_storage storage_;
};






#endif // __STORAGE_HPP__
