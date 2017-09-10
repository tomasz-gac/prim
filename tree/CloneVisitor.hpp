#ifndef __CLONE_VISITOR_HPP__
#define __CLONE_VISITOR_HPP__

#include "node.hpp"
#include <unordered_map>

template< typename T >
class CloneVisitor;

template< typename... Ts >
class CloneVisitor< Node< Ts... > >
  : public visitor< CloneVisitor< Node<Ts...> > >
{
public:
  template< typename T >
  void operator()( T& node ){
    for( auto child_it = children_begin( node ); child_it != children_end( node ); ++child_it ){
      const void* originalNodeAddress = static_cast<const void*>(&(**child_it));

      auto match = visited_.find( originalNodeAddress );
      
      if( match == visited_.cend() ){ // unique node
	*child_it = registered_clone( *child_it );
    	this->visit( *child_it );
      } else {			// node already cloned
    	*child_it = match->second; 	// no recursion - already copied this branch
      }
    }
  }

  CloneVisitor( Node<Ts...> cloned )
    : visited_()
    , result( registered_clone( cloned  ) )
  {
    this->visit( result );
  };  

  

private:
  Node<Ts...> registered_clone( const Node<Ts...>& original ){
    auto clone = original->clone();
    visited_.emplace( std::make_pair( static_cast<const void*>(&**original), clone  ) );
    return std::move( clone );
  }
  
  std::unordered_map< const void*, Node<Ts...> > visited_; // original node -> cloned tree
public:
  Node<Ts...> result;
};

template< typename... Ts >
Node<Ts...> clone( const Node<Ts...>& original ){
  return CloneVisitor<Node<Ts...>>( original ).result;
}


#endif // __CLONE_VISITOR_HPP__
