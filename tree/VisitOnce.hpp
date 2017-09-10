#ifndef __VISIT_ONCE_HPP__
#define __VISIT_ONCE_HPP__

#include "node.hpp"
#include <unordered_set>

class VisitOnce
{
private:
  template< typename T >
  T& get( T& v ){ return v; }

  template< typename... Ts >
  Node< Ts...>& get( std::reference_wrapper< Node< Ts...> > node ){
    return node;
  }

  template< typename... Ts >
  const Node< Ts...>& get( const std::reference_wrapper< Node< Ts...> > node ){
    return node;
  }

public:
  template< typename T, typename F >
  void once( T& node, F f ){
    bool found;
    auto& unpackedNode = 
    std::tie( std::ingore, found ) =
      visited_.emplace( static_cast< const void*>(& VisitOnce<Derived>::get( node ) ) );

    if( !found ){
      f( VisitOnce<Derived>::get( node ) ) );
    }
  }

  const std::unordered_set< const void* >& visited() const { return visited_; }
  
private:
  std::unordered_set< const void* > visited_;
};




#endif   // __VISIT_ONCE_HPP__
