#ifndef __REPR_VISITOR_HPP__
#define __REPR_VISITOR_HPP__

#include "AST.hpp"
#include <vector>
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
      result = ident();
      std::cout << "Visitation, visited:" << std::endl;
      for( auto n : visited_ )
	std::cout << n << std::endl;
      const Rule::INode* node_addr =  static_cast<const Rule::INode*>(&node);
      auto match = std::find( visited_.cbegin(), visited_.cend(), node_addr );
      
      if( match != visited_.cend() ){
	result += "<< Parser " + std::to_string(visited_.cend() - match) + " recursion>>";
      } else {
	visited_.push_back( node_addr );
	result += std::to_string( visited_.size()) + ":" + visit( node );
	std::cout << "visitation end, result = " + result << std::endl;
      } 
    }
     
    std::string visit( const Regex& node );
    std::string visit( const Alternative& node );
    std::string visit( const Sequence& node );

    std::string result;

    ReprVisitor( int depth = 0 )
      : result(), depth_(depth), visited_()
    { }
	
    ReprVisitor fork() const {
      return ReprVisitor( depth_+1 );
    }
    
    std::string ident() const {
      return std::string( depth_, '|' );
    }

    int depth_;
    std::vector< const Rule::INode* > visited_; //address
  };

}
#endif // __REPR_VISITOR_HPP__
