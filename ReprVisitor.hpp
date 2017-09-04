#ifndef __REPR_VISITOR_HPP__
#define __REPR_VISITOR_HPP__

#include "AST.hpp"
#include <vector>
#include <iostream>
#include <algorithm>
#include <string>

template< typename T >
struct print_type;

namespace AST{
  
  class ReprVisitor 
  {
  public:
    template< typename T >
    void operator()( const T& node ){
      result = std::string();	// requires empty result on entry to recurse
      ++depth_;			// increase depth for ident
      auto text = ident();
      // check whether node was already visited
      auto match = std::find( visited_.cbegin(), visited_.cend(), &node );
      if( match != visited_.cend() ){ // node has already been visited
	auto index = std::distance( visited_.cbegin(), match ) + 1;
	text += "<<Parser " + std::to_string( index  ) + " recursion>>";
      } else {			     // node visited for the first time
	visited_.push_back( &node ); // add the node's address 
	auto id = visited_.size();   // id is the current size of visited_ vector
	text += std::to_string( id ) + ":" + visit( node ); // handle specific nodes
      }
      --depth_;			// decrease depth of ident
      result = std::move(text);	// return by member
    }
     
    std::string visit( const Regex& node );
    std::string visit( const Alternative& node );
    std::string visit( const Sequence& node );

    std::string result;
    
  private:
    template< typename T >
    std::string visitChildren( const T& node ){
      auto text = std::string();
      for( auto it = children_cbegin(node); it != children_cend( node ); ++it ){
    	auto av = Rule::adaptVisitor( *this );
    	it->accept( av );
    	text += "\n" + this->result;
      }
      return text;
    }

    
    // template< typename InputIt >
    // std::string visitChildren( InputIt begin, InputIt end ){
    //   auto text = std::string();
    //   for( auto& it = begin; it != end; ++it ){
    // 	auto av = Rule::adaptVisitor( *this );
    // 	it->accept( av );
    // 	text += "\n" + this->result;
    //   }
    //   return text;
    // }
    
    std::string ident() const {
      if( depth_ == 0 ) return std::string();
      return std::string( depth_-1, '|' ) + "+";
    }

    int depth_ = -1;
    std::vector< const Rule::INode* > visited_; // visit each node once
  };

}
#endif // __REPR_VISITOR_HPP__
