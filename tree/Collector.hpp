#ifndef __COLLECTOR_HPP__
#define __COLLECTOR_HPP__

#include "tree.hpp"
#include "VisitOnce.hpp"
#include <vector>

template< typename >
class Collector;

template< typename... Ts >
class Collector< Node< Ts... > >
  : public visitor< Collector< Node< Ts... > > >
{
public:
  template< typename T >
  void operator()( T& node ){

    auto onUnique = [this]( T& node ){
      for( auto it = children_begin( node ); it != children_end( node ); ++it )
	visit( *it );
      result.emplace_back( std::move( node ) );
      
    };

    auto onVisited = [this]( T& node ){
      
      
    };
    visit_.once( node, onUnique, onVisited );
  }



  std::vector< Node< Ts... > > result;
private:
  VisitOnce visit_;
};



#endif //  __COLLECTOR_HPP__
