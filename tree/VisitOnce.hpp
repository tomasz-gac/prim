#ifndef __VISIT_ONCE_HPP__
#define __VISIT_ONCE_HPP__

#include "node.hpp"
#include <unordered_set>
#include <tuple>

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
  template< typename T, typename F, typename G >
  void once( T& node, F f, G g = [](const auto&){} ){
    bool inserted;
    std::tie( std::ignore, inserted ) =
      visited_.emplace( static_cast< const void*>(& VisitOnce::get( node ) ) );
    if( inserted ){		// node is unique
      f( VisitOnce::get( node ) );
    } else {
      g( VisitOnce::get( node ) );
    }
  }

  const std::unordered_set< const void* >& visited() const { return visited_; }
  
private:
  std::unordered_set< const void* > visited_;
};




#endif   // __VISIT_ONCE_HPP__
