#ifndef __VTABLE_HPP__
#define __VTABLE_HPP__

#include "invoker.hpp"
#include "EraseVoidPtr.hpp"
#include "LocalVTable.hpp"
#include "RemoteVTable.hpp"
#include "JoinedVTable.hpp"
#include "JumpVTable.hpp"

template< typename Tag >
struct LocalVT
  : Implementation< LocalVTable< Tag, EraseVoidPtr > >, Tag
{ };

template< typename Tag >
struct RemoteVT
  : Implementation< RemoteVTable< Tag, EraseVoidPtr > >, Tag
{ };

template< typename... Tags >
struct JoinedVT
  : Implementation< JoinedVTable< Tags... > >
  , join_t< interface_t<Tags>... >
{  };

template< typename Tag, typename... Ts >
struct JumpVT
  : Implementation< JumpVTable< Tag, EraseVoidPtr, Ts... > >
  , Tag
{  };

template< typename Interface >
LocalVT< Interface > localVT( Interface ){ return {}; };

template< typename Interface >
RemoteVT< Interface > remoteVT( Interface ){ return {}; };

template< typename... Tags >
JoinedVT< Tags... > joinVT( Tags... ){ return {}; };

template< typename Interface, typename... Ts >
JumpVT< Ts... > joinVT( Interface ){ return {}; };

#endif // __VTABLE_HPP__

