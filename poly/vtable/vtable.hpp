#ifndef __VTABLE_HPP__
#define __VTABLE_HPP__

#include "../invoker.hpp"
#include "LocalVTable.hpp"
#include "RemoteVTable.hpp"
#include "JoinedVTable.hpp"
#include "JumpVTable.hpp"

namespace poly{

template< typename Tag >
struct LocalVT
  : Implementation< LocalVTable< Tag, void* > >, Tag
{ using pointer_type = void*; };

template< typename Tag >
struct RemoteVT
  : Implementation< RemoteVTable< Tag, void* > >, Tag
{ using pointer_type = void*; };

template< typename... Tags >
struct JoinedVT
  : Implementation< JoinedVTable< Tags... > >
  , join_t< interface_t<Tags>... >
{ using pointer_type = void*; };

template< typename Tag, typename... Ts >
struct JumpVT
  : Implementation< JumpVTable< Tag, void*, Ts... > >
  , Tag
{ using pointer_type = void*; };

template< typename Interface >
LocalVT< Interface > localVT( Interface ){ return {}; };

template< typename Interface >
RemoteVT< Interface > remoteVT( Interface ){ return {}; };

template< typename... Tags >
JoinedVT< Tags... > joinVT( Tags... ){ return {}; };

template< typename Interface, typename... Ts >
JumpVT< Ts... > joinVT( Interface ){ return {}; };

}
  
#endif // __VTABLE_HPP__

