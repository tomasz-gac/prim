#ifndef __REPR_VISITOR_HPP__
#define __REPR_VISITOR_HPP__

#include "tree/tree.hpp"
#include <vector>
#include <iostream>
#include <algorithm>
#include <string>

class ReprVisitor
  : public const_visitor<ReprVisitor>
{
 public:
  template< typename T >
    void operator()( const T& node ){
    result = std::string();	// requires empty result on entry to recurse
    ++depth_;			// increase depth for ident
    auto text = ident();
      auto* node_addr = static_cast< const void* >(&node);
    // check whether node was already visited
      auto match = std::find( visited_.cbegin(), visited_.cend(), node_addr  );
    if( match != visited_.cend() ){ // node has already been visited
      auto index = std::distance( visited_.cbegin(), match ) + 1;
      text += "<<Node " + std::to_string( index  ) + " recursion>>";
    } else {			     // node visited for the first time
      visited_.push_back( node_addr ); // add the node's address 
      auto id = visited_.size();   // id is the current size of visited_ vector
      text += std::to_string( id ) + ":" + visit_node( node ); // handle specific nodes
    }
    --depth_;			// decrease depth of ident
    result = std::move(text);	// return by member
  }

  std::string result;
    
 private:
  template< typename T >
    std::string visit_node( const T& node ){
    auto text = "<class " + std::string(typeid( node ).name()) + ">";
    for( auto it = children_cbegin(node); it != children_cend( node ); ++it ){
      visit( *it );
      text += "\n" + this->result;
    }
    return text;
  }
    
  std::string ident() const {
    if( depth_ == 0 ) return std::string();
    return std::string( depth_-1, '|' ) + "+";
  }

  int depth_ = -1;
  std::vector< const void* > visited_; // visit each node once
};

#endif // __REPR_VISITOR_HPP__
