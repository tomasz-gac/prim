#ifndef __AST_HPP__
#define __AST_HPP__


#include <memory>
#include <array>
#include <typeinfo>
#include <iostream>
#include "visitor.hpp"

namespace AST{
template< size_t N >
class Node;  

using NodeTerminal = Node<0>;
using NodeUnaryOp  = Node<1>;
using NodeBinaryOp = Node<2>;

class INode;

using node_ptr = std::shared_ptr< const INode >;

class NodeVisitor 
: public IVisitor< 
  NodeTerminal
, NodeUnaryOp
, NodeBinaryOp
, class Alternative
, class Sequence
, class Regex
> {  };

class INode 
: public IVisitable< NodeVisitor >{
public:
  virtual node_ptr clone() const = 0;
  virtual ~INode() = 0;
};
  
class Rule{
public:  
  Rule operator|( const Rule&   other ) const;
  Rule operator|(       Rule&&  other ) const;
  Rule operator&( const Rule&   other ) const;
  Rule operator&(       Rule&&  other ) const;
  
private:  
  node_ptr node_;
};

template< size_t N >
class Node
: public INode
{  
public:
  virtual node_ptr clone() const override{
    return std::make_shared<const Node>( *this );
  } 

  template< typename... Ts>
  Node( Ts&&... Vs)
  : rules_(std::forward<Ts>(Vs)...)
  {  }
  
  std::array< node_ptr, N > rules_;
};

class NodePrinter 
: public NodeVisitor{
public:
  virtual void visit( Regex& Node ) override;
  virtual void visit( const Regex& Node ) override;
  
  virtual void visit( Alternative& Node ) override;  
  virtual void visit( const Alternative& Node ) override;
  
  virtual void visit( Sequence& Node ) override;  
  virtual void visit( const Sequence& Node ) override;

  virtual void visit( NodeTerminal& Node ) override;
  virtual void visit( const NodeTerminal& Node ) override;
  
  virtual void visit( NodeUnaryOp& Node ) override;
  virtual void visit( const NodeUnaryOp& Node ) override;
  
  virtual void visit( NodeBinaryOp& Node ) override;
  virtual void visit( const NodeBinaryOp& Node ) override;

private:
  template< typename T >
  void visit_children( T&& node ){
    for( auto& node : node.rules_ )
      node->accept(*this);
  }
};

}
#endif // __AST_HPP__
