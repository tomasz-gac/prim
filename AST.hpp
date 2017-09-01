#ifndef __AST_HPP__
#define __AST_HPP__


#include <memory>
#include <array>
#include <typeinfo>
#include <iostream>
#include "visitor.hpp"

namespace AST{

  template< typename... Ts >
  class Tree{
  public:
    class INode;
    using node_ptr    = std::shared_ptr< const INode >;
  public:

    template< size_t N >
    class Node;  

    using NodeTerminal = Node<0>;
    using NodeUnary  = Node<1>;
    using NodeBinary = Node<2>;

  private:
    class NodeVisitor 
    : public IVisitor< 
        NodeTerminal
      , NodeUnary
      , NodeBinary
      , Ts...
    > {  };

  public:
    class INode 
      : public IVisitable< NodeVisitor >{
    public:
      virtual node_ptr clone()    const = 0;
      virtual ~INode() = default;
    };

    template< size_t N >
    class Node
      : public Visitable<Node<N>>::template extends< INode >
      , public std::enable_shared_from_this<Node<N>>
    {  
    public:
      virtual node_ptr clone() const override{
	return this->shared_from_this();
      } 

      template< typename... Us>
      Node( Us&&... Vs)
	: rules_(std::forward<Us>(Vs)...)
      {  }
  
      std::array< node_ptr, N > rules_;
    };

    template< typename T, typename... Us >
    static Tree make( Us&&... Vs ){
      auto rule = Tree();
      rule.node_.reset( new T( std::forward<Us>(Vs)... ) );
      return rule;
    }

    // template< typename V >
    // void accept( V& visitor ){
    //   auto deref = INode::adaptVisitor([&visitor]( auto& node ){ 
    // 	  return visitor.visit(*node);
    // 	});
    //   node_->accept( deref );
    // }

    template< typename V >
    void accept( V& visitor ) const {
      auto deref = INode::adaptVisitor( visitor );
	// [&visitor]( const auto& node ){ 
	//   return visitor.visit(*node);
	// });
      node_->accept( deref );
    }
          INode& node()       { return *node_; }
    const INode& node() const { return *node_; }

  private:  
    Tree() = default;
    node_ptr node_;
  };

  using Rule = Tree< class Alternative, class Sequence, class Regex >;

  class Alternative
    : public Visitable< Alternative >::extends< Rule::NodeBinary >
  {
  public:
    Alternative(  const Rule::INode& lhs, const Rule::INode& rhs ){
      rules_[0] = lhs.clone();
      rules_[1] = rhs.clone();
    }  
  };
  class Sequence
    : public Visitable< Sequence >::extends< Rule::NodeBinary >
  {
  public:
    Sequence(  const Rule::INode& lhs, const Rule::INode& rhs ){
      rules_[0] = lhs.clone();
      rules_[1] = rhs.clone();
    }  
  };
  class Regex    : public Visitable< Regex >::extends< Rule::NodeTerminal > {};

  class NodePrinter 
  {
  public:
     void operator()( Regex& Node );
     void operator()( const Regex& Node );
  
     void operator()( Alternative& Node );  
     void operator()( const Alternative& Node );
  
     void operator()( Sequence& Node );  
     void operator()( const Sequence& Node );

     void operator()( Rule::NodeTerminal& Node );
     void operator()( const Rule::NodeTerminal& Node );
  
     void operator()( Rule::NodeUnary& Node );
     void operator()( const Rule::NodeUnary& Node );
  
     void operator()( Rule::NodeBinary& Node );
     void operator()( const Rule::NodeBinary& Node );

  private:
    template< typename T >
    void visit_children( T& node ){
      auto v = Rule::INode::adaptVisitor(*this);
      for( auto& node : node.rules_ )
	node->accept( v );
    }
  };
Rule operator|( const Rule&, const Rule&);
Rule operator&( const Rule&, const Rule&);
}
#endif // __AST_HPP__
