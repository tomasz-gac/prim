#ifndef __AST_HPP__
#define __AST_HPP__


#include "tree.hpp"
#include <iostream>
 
namespace AST{
  using Rule = Tree< class Alternative, class Sequence, class Regex >;

  class Alternative
    : public Rule::Node<Alternative>
  {
   public:
    Alternative(  const Rule::INode& lhs, const Rule::INode& rhs ){
      children_[0] = lhs.ref();
      children_[1] = rhs.ref();
    }
    
    std::array< Rule::node_ptr, 2 > children_;
  };
  
  class Sequence
    : public Rule::Node<Sequence>
  {
  public:
    Sequence(  const Rule::INode& lhs, const Rule::INode& rhs ){
      children_[0] = lhs.ref();
      children_[1] = rhs.ref();
    }

    std::array< Rule::node_ptr, 2 > children_;    
  };
  class Regex : public Rule::Node<Regex>{ };

  class NodePrinter 
  {
  public:
     void operator()( Regex& Node );
     void operator()( const Regex& Node );
  
     void operator()( Alternative& Node );  
     void operator()( const Alternative& Node );
  
     void operator()( Sequence& Node );  
     void operator()( const Sequence& Node );

     // void operator()( Rule::NodeTerminal& Node );
     // void operator()( const Rule::NodeTerminal& Node );
  
     // void operator()( Rule::NodeUnary& Node );
     // void operator()( const Rule::NodeUnary& Node );
  
     // void operator()( Rule::NodeBinary& Node );
     // void operator()( const Rule::NodeBinary& Node );

  private:
    template< typename T >
    void visit_children( T& node ){
      auto v = Rule::adaptVisitor(*this);
      for( auto& node : node.children_ )
	node->accept( v );
    }
  };
Rule operator|( const Rule&, const Rule&);
Rule operator&( const Rule&, const Rule&);
}
#endif // __AST_HPP__
