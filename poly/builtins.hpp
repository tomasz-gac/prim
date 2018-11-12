#ifndef __BUILTINS_HPP__
#define __BUILTINS_HPP__

#include "invoker.hpp"
#include "vtable/Invalid.hpp"

namespace poly{

struct copy : declare< copy, void (const poly::T&,  void*) >{ };

template< bool is_noexcept >
struct move_ : declare< move_<is_noexcept>, void (    T&,  void* ) >{ };

using move = move_<false>;
using move_noexcept = move_<true>;

struct destroy : declare< destroy, void ( const T& ) >{  };

struct type_id : declare< type_id, const std::type_info& ( const T& ) >{  };

struct storage_info;
struct storage : declare< storage, storage_info ( const T& ) >{  };

struct address_of : declare< address_of, const void*( const T& )>{  };

template< typename T >
void invoke( copy, const T& v, void* ptr ){ new (ptr) T( v ); }

template< typename T >
void invoke( move, T& v, void* ptr ){ new (ptr) T( std::move(v) ); }

template< typename T >
void invoke( move_noexcept, T& v, void* ptr ) noexcept {
  static_assert( std::is_nothrow_move_constructible<T>::value,
  		 "Class provided for Poly is not nothrow move constructible" );
  new (ptr) T( std::move(v) );
}

template< typename T >
void invoke( destroy, const T& v ){ v.~T(); }

template< bool AlwaysFalse = false  >
void invoke( destroy, Invalid& ){
  static_assert( AlwaysFalse, "Cannot destroy object of type Invalid" );
}

template< typename T >
const std::type_info& invoke( type_id, const T& ){ return typeid(T); }

struct storage_info{
private:
  storage_info( size_t s, size_t a )
    : size(s), alignment(a) {  }
public:
  template< typename T >
  static storage_info get()
  { return { sizeof(T), alignof(T) }; }
  
  const std::size_t size;
  const std::size_t alignment;
};


template< typename T >
storage_info invoke( storage, const T& ){ return storage_info::get<T>(); }

template< typename T >
const void* invoke( address_of, const T& v ){ return reinterpret_cast<const void*>(&v); }

}
#endif // __BUILTINS_HPP__
