#ifndef __VTABLE_HPP__
#define __VTABLE_HPP__

#include "invoker.hpp"
#include "EraseVoidPtr.hpp"
#include "LocalVTable.hpp"
#include "RemoteVTable.hpp"
#include "JoinedVTable.hpp"

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

template< typename Interface >
LocalVT< Interface > localVT( Interface ){ return {}; };

template< typename Interface >
RemoteVT< Interface > remoteVT( Interface ){ return {}; };

template< typename... Tags >
JoinedVT< Tags... > joinVT( Tags... ){ return {}; };

#endif // __VTABLE_HPP__

