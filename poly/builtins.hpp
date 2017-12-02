#ifndef __BUILTINS_HPP__
#define __BUILTINS_HPP__

#include "invoker.hpp"

struct copy : Invoker< copy, void (void*, const T& ) >{  };
struct move : Invoker< move, void (void*,       T&& ) >{  };
struct destroy : Invoker< destroy, void ( const T& ) >{  };

struct assign      : Invoker< assign     , void ( T&, forward<T> ) > {  };
struct move_assign : Invoker< move_assign, void ( T&,       T&& ) > {  };
struct copy_assign : Invoker< move_assign, void ( T&, const T&  ) > {  };

struct type_id : Invoker< type_id, const std::type_info& ( const T& ) >{  };

struct storage_info;
struct storage : Invoker< storage, storage_info ( const T& ) >{  };

template< typename T >
void invoke( copy, void* ptr, const T& v ){ new (ptr) T( v ); }

template< typename T >
void invoke( move, void* ptr, T v ){ new (ptr) T( std::move(v) ); }

template< typename T >
void invoke( destroy, const T& v ){  v.~T(); }

template< typename T >
const std::type_info& invoke( type_id, const T& ){ return typeid(T); }

template< typename T, typename T2 >
void invoke( copy_assign, T& value, const T2&& v ){
  static_assert( std::is_same< std::decay_t<T>, std::decay_t<T2> >::value );
  value = v;
}

template< typename T, typename T2 >
void invoke( move_assign, T& value, T2&& v ){
  static_assert( std::is_same< std::decay_t<T>, std::decay_t<T2> >::value );
  value = std::move(v);
}

template< typename T, typename T2 >
void invoke( assign, T& value, T2&& v ){
  static_assert( std::is_same< std::decay_t<T>, std::decay_t<T2> >::value );
  value = std::forward<T2>(v);  
}

struct storage_info{
  std::size_t size;
  std::size_t alignment;
};

template< typename T >
storage_info invoke( storage, const T& ){ return { sizeof(T), alignof(T) }; }

#endif // __BUILTINS_HPP__
