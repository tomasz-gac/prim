#ifndef __AST_HPP__
#define __AST_HPP__


#include "tree.hpp"
#include <iostream>
 
namespace AST{
  using Rule = Tree< class Alternative, class Sequence, class Regex >;

  class Alternative
    : public Rule::NodeBinary
  {
   public:
    virtual void accept( Rule::NodeVisitor& v )       override { v.dispatch( *this ); }
    virtual void accept( Rule::NodeVisitor& v ) const override { v.dispatch( *this ); }
    
    Alternative(  const Rule::INode& lhs, const Rule::INode& rhs ){
      rules_[0] = lhs.clone();
      rules_[1] = rhs.clone();
    }  
  };
  class Sequence
    : public Rule::NodeBinary
  {
  public:
    virtual void accept( Rule::NodeVisitor& v )       override { v.dispatch( *this ); }
    virtual void accept( Rule::NodeVisitor& v ) const override { v.dispatch( *this ); }
    
    Sequence(  const Rule::INode& lhs, const Rule::INode& rhs ){
      rules_[0] = lhs.clone();
      rules_[1] = rhs.clone();
    }  
  };
  class Regex : public Rule::NodeBinary{
  public:
    virtual void accept( Rule::NodeVisitor& v )       override { v.dispatch( *this ); }
    virtual void accept( Rule::NodeVisitor& v ) const override { v.dispatch( *this ); }
  };

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
  
     void operator()( Rule::NodeBinary& Node );
     void operator()( const Rule::NodeBinary& Node );

  private:
    template< typename T >
    void visit_children( T& node ){
      auto v = Rule::adaptVisitor(*this);
      for( auto& node : node.rules_ )
	node->accept( v );
    }
  };
Rule operator|( const Rule&, const Rule&);
Rule operator&( const Rule&, const Rule&);
}
#endif // __AST_HPP__
