#ifndef __VTABLE_HPP__
#define __VTABLE_HPP__

#include "../invoker.hpp"
#include "LocalVTable.hpp"
#include "RemoteVTable.hpp"
// #include "JoinedVTable.hpp"
#include "JumpVTable.hpp"
#include "erased.hpp"

namespace prim{

template< typename Tag >
using LocalVT = LocalVTable< Tag, Erased<void*> >;

template< typename Tag >
using RemoteVT = RemoteVTable< Tag, Erased<void*> >;

template< typename Tag, typename... Ts >
using JumpVT = JumpVTable< Tag, Erased<void*>, Ts... >;
  
// template< typename... Tags >
// using JoinVT = JoinedVTable< Tags... >;

  
template< typename Interface >
LocalVT< Interface > localVT( Interface ){ return {}; };

template< typename Interface >
RemoteVT< Interface > remoteVT( Interface ){ return {}; };

// template< typename... Tags >
// JoinedVT< Tags... > joinVT( Tags... ){ return {}; };

template< typename Interface, typename... Ts >
JumpVT< Interface, Ts... > joinVT( Interface ){ return {}; };

}
  
#endif // __VTABLE_HPP__

