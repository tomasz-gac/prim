#ifndef __CLONE_VISITOR_HPP__
#define __CLONE_VISITOR_HPP__

#include "tree.hpp"
#include <vector>

template< typename T >
class CloneVisitor;


template< typename... Ts >
class CloneVisitor< Tree<Ts... > >{
private:
  using Tree = Tree<Ts...>;
public:
  template< typename T >
  void operator()( const T& node ){
    // TODO : finish
    auto( it = children_begin( node ); it != children_end( node ); ++it ){
      *it = it->clone();
      auto adapter = Tree::adaptVisitor( *this );
      it->accept( adapter );
    }
    
    
  }

  CloneVisitor( Tree cloned )
    : result( cloned->clone() )
  { }
  
  Tree result;
  

private:
  std::vector< const Tree*> visited_;
};



#endif // __CLONE_VISITOR_HPP__
