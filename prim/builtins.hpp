#ifndef __BUILTINS_HPP__
#define __BUILTINS_HPP__

#include "invoker.hpp"
#include "vtable/Invalid.hpp"

namespace prim{

struct copy : declare< copy, void (const prim::T&,  void*) >{ };

template< bool is_noexcept >
struct move_ : declare< move_<is_noexcept>, void (    T&,  void* ) >{ };

using move = move_<false>;
using move_noexcept = move_<true>;

struct destroy : declare< destroy, void ( const T& ) >{  };

struct type_info;
struct type : declare< type, type_info ( const T& ) >{  };

struct address_of : declare< address_of, const void*( const T& )>{  };

template< typename T >
void invoke( copy, const T& v, void* ptr ){ new (ptr) T( v ); }

template< typename T >
void invoke( move, T& v, void* ptr ){ new (ptr) T( std::move(v) ); }

template< typename T >
void invoke( move_noexcept, T& v, void* ptr ) noexcept {
  static_assert( std::is_nothrow_move_constructible<T>::value,
  		 "Class provided for Prim is not nothrow move constructible" );
  new (ptr) T( std::move(v) );
}

template< typename T >
void invoke( destroy, const T& v ){ v.~T(); }

template< bool AlwaysFalse = false  >
void invoke( destroy, Invalid& ){
  static_assert( AlwaysFalse, "Cannot destroy object of type Invalid" );
}

struct type_info{
private:
  type_info( size_t s, size_t a, const std::type_info& i )
    : size(s), alignment(a), info(i) {  }
public:
  template< typename T >
  static type_info get()
  { return { sizeof(T), alignof(T), typeid(T) }; }
  
  const std::size_t size;
  const std::size_t alignment;
  const std::type_info& info;
};


template< typename T >
type_info invoke( type, const T& ){ return type_info::get<T>(); }

template< typename T >
const void* invoke( address_of, const T& v ){ return reinterpret_cast<const void*>(&v); }

}
#endif // __BUILTINS_HPP__
