#ifndef __REMOTE_VTABLE_HPP__
#define __REMOTE_VTABLE_HPP__

#include "LocalVTable.hpp"

namespace poly{

// VTable that holds thunks remotely
template< typename Interface, typename erased_t = Erased<void*> >
class RemoteVTable{
public:
  using interface = interface_t<Interface>;
  using erased_type = erased_t;
private:
  
  using VTable = LocalVTable<Interface, erased_type>;
  template< typename Invoker >
  using TThunk = Thunk< Invoker, erased_type >;
  // Singleton reference
  VTable* vtable_;

  RemoteVTable( VTable& vtbl )
    : vtable_( &vtbl )
  {  };
  
public:
  // Makes the VTable for type T using a singleton pattern
  template< typename T = void >
  static RemoteVTable make(){
    static VTable instance = VTable::template make<T>();
    
    return { instance };
  }

  template< typename To >
  explicit operator LocalVTable<To, erased_type>() const
  { return static_cast< LocalVTable<To, erased_type>>( vtable_ ); }


  template< typename Tag >
  const TThunk< Tag >& operator[]( const Tag& ) const {
    return get< Tag >();
  }

  template< typename Tag >
  const TThunk< Tag >& get() const {
    return vtable_->template get< Tag >();
  }

  bool operator==( const RemoteVTable& other ) const {
    return *vtable_ == *other.vtable_;
  }

  bool operator!=( const RemoteVTable& other ) const {
    return *vtable_ != *other.vtable_;
  }
};

}

#endif // __REMOTE_VTABLE_HPP__
