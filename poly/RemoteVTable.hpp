#ifndef __REMOTE_VTABLE_HPP__
#define __REMOTE_VTABLE_HPP__

#include "LocalVTable.hpp"

// VTable that holds thunks remotely
template< typename Interface, template< typename > class Transform >
class RemoteVTable{
public:
  using interface = interface_t<Interface>;
private:
  using VTable = LocalVTable<Interface, Transform>;
  template< typename Invoker >
  using TThunk = Thunk< Invoker, Transform >;
  // Singleton reference
  VTable& vtable_;

  RemoteVTable( VTable& vtbl )
    : vtable_( vtbl )
  {  };
  
public:
  // Makes the VTable for type T using a singleton pattern
  template< typename T = void >
  static RemoteVTable make(){
    static VTable instance = VTable::template make<T>();
    
    return { instance };
  }

  template< typename To >
  explicit operator LocalVTable<To, Transform>() const
  { return static_cast< LocalVTable<To, Transform>>( vtable_ ); }


  template< typename Tag >
  const TThunk< Tag >& operator[]( const Tag& ) const {
    return get< Tag >();
  }

  template< typename Tag >
  const TThunk< Tag >& get() const {
    return vtable_.template get< Tag >();
  }
};

#endif // __REMOTE_VTABLE_HPP__
