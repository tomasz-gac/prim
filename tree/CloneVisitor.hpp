#ifndef __CLONE_VISITOR_HPP__
#define __CLONE_VISITOR_HPP__

#include "tree.hpp"
#include <unordered_map>

template< typename T >
class CloneVisitor;

template< typename... Ts >
class CloneVisitor< Tree<Ts... > >{
private:
  using Tree  = Tree<Ts...>;
  using INode = typename Tree::INode;
public:
  template< typename T >
  // typename std::enable_if< !std::is_const< T >::value, void >::type
  void
  operator()( T& node ){
    for( auto it = children_cbegin( node ); it != children_cend( node ); ++it ) {
      auto adapter = Tree::adaptVisitor( *this );
      it->accept( adapter );
    }
    
    // for( auto child_it = children_begin( node ); child_it != children_end( node ); ++child_it ){
    //   const INode* originalNodeAddress = &(child_it->node());

    //   auto match = visited_.find( originalNodeAddress );
      
    //   if( match == visited_.cend() ){ // uniqie node
    // 	auto clone = (*child_it)->clone();
    // 	visited_[ originalNodeAddress ] = clone;
    // 	*child_it = clone;
    // 	auto adapter = Tree::adaptVisitor( *this );
    // 	child_it->accept( adapter );
    //   } else {			// node already cloned
    // 	*child_it = match->second; 	// no recursion - already copied this branch
    //   }
    // }
    
    
  }

  CloneVisitor( Tree cloned )
    : result( cloned->clone() )
  {
  };  
  Tree result;
  

private:
  std::unordered_map< const INode*, Tree > visited_; // original node -> cloned tree
};



#endif // __CLONE_VISITOR_HPP__
