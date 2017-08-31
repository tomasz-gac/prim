ifndef __AST_HPP__
#define __AST_HPP__


#include <memory>
#include <array>
#include <typeeinfo>
#include <iostream>
#include "visitor.hpp"

namespace AST{
template< typename T, size_t N >
class Node;  

using NodeTerminal = Node<Terminal, 0>;

template<typename T> 
using NodeUnaryOp  = Node<T, 1>;

template<typename T> 
using NodeBinaryOp = Node<T, 2>;

class Alternative : Visitable<>::extends<  > {};
  class Sequence: isitable<>::extends<  > {};
class Terminal : Visitable<>::extends<  > {};

class INode;

using node_ptr = std::shared_ptr< const INode >;

class NodeVisitor 
: public IVisitor< 
  NodeTerminal
, NodeBinaryOp< Alternative >
, NodeBinaryOp< Sequence > 
> {  };

class INode 
: public IVisitable< NodeVisitor >{
public:
  virtual node_ptr clone() const = 0;
  virtual ~INode() =
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

template< typename T, size_t N >
class Rule::Node
: public Rule::INode
{  
public:
  virtual node_ptr clone() const override{
    return std::make_shared<const Node>( *this );
  } 

  template< typename... Ts>
  Node( const T& v, Ts&&... Vs)
  : value_(v)
  , rules_(std::forward<Ts>(Vs)...)
  {  }
  
  T value_; 
  std::array< std::shared_ptr< const INode >, N > rules_;
};


/*class NodePrinter 
: public NodeVisitor{
public:
  virtual void visit( Terminal& Node ) override;
  virtual void visit( const Terminal& Node ) override;
  
  virtual void visit( Alternative& Node ) override;  
  virtual void visit( const Alternative& Node ) override;
  
  virtual void visit( Sequence& Node ) override;  
  virtual void visit( const Sequence& Node ) override;
};*/

}
#endif // __AST_HPP__
