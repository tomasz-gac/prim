#ifndef __TREE_HPP__
#define __TREE_HPP__

#include <memory>
#include <array>
#include <type_traits>
#include "tree.hpp"

template< typename T >
class IVisitor{
public:
  virtual void visit(      T& ) = 0;
  virtual void visit(const T& ) = 0;

  virtual ~IVisitor() = default;
};

template< typename... Ts >
class Tree{
public:
  class INode;
  using node_ptr = std::shared_ptr< const INode >;
public:
  template< size_t N >
  class Node;  

  using NodeTerminal = Node<0>;
  using NodeUnary  = Node<1>;
  using NodeBinary = Node<2>;

public:
  class NodeVisitor
    : public IVisitor<Ts>...
    , public IVisitor<NodeTerminal>
    , public IVisitor<NodeUnary>
    , public IVisitor<NodeBinary>
  {
  public:
    template< typename T >
    void dispatch( T& object ){
      static_cast<IVisitor<typename std::remove_const<T>::type>*>(this)->visit( object );
    }
  };

  template< typename F >
  class VisitorAdapter;
  
  template< typename F, typename U, typename... Us >
  class VisitorAdapter_impl;

  template< typename F >
  static VisitorAdapter<F> adaptVisitor( const F& f );
  
public:
  class INode{
  public:
    virtual void accept( NodeVisitor& )       = 0;
    virtual void accept( NodeVisitor& ) const = 0;

    virtual node_ptr clone() const = 0;

    virtual ~INode() = default;
  };

  template< size_t N >
  class Node
    : public INode, public std::enable_shared_from_this<Node<N>>
  {  
  public:
    virtual void accept( NodeVisitor& v )       override { v.dispatch( *this ); }
    virtual void accept( NodeVisitor& v ) const override { v.dispatch( *this ); }
    
    virtual node_ptr clone() const override{ return  this->shared_from_this(); }

    // template< typename... Us>
    // Node( Us&&... Vs)
    //   : rules_(std::forward<Us>(Vs)...)
    // {  }
  
    std::array< node_ptr, N > rules_;
  };

  template< typename T, typename... Us >
  static Tree make( Us&&... Vs ){
    auto rule = Tree();
    rule.node_.reset( new T( std::forward<Us>(Vs)... ) );
    return rule;
  }
  template< typename V >
  void accept( V& visitor ) const {
    auto deref = adaptVisitor( visitor );
    node_->accept( deref );
  }
  
  INode& node()       { return *node_; }
  const INode& node() const { return *node_; }

private:  
  Tree() = default;
  node_ptr node_;
};

template< typename... Ts >
template< typename F, typename U, typename... Us >
class Tree<Ts...>::VisitorAdapter_impl
  : public VisitorAdapter_impl< F, Us... >
{
  using VA = Tree<Ts...>::VisitorAdapter<F>;
public:
  virtual void visit(       U& v ) override { static_cast<VA*>(this)->visit(v); }
  virtual void visit( const U& v ) override { static_cast<VA*>(this)->visit(v); }

  ~VisitorAdapter_impl() = default;
};

template< typename... Ts >
template< typename F, typename U >
class Tree<Ts...>::VisitorAdapter_impl< F, U >
  : public Tree<Ts...>::NodeVisitor
{
  using VA = Tree<Ts...>::VisitorAdapter<F>;
public:
  virtual void visit(       U& v ) override { static_cast<VA*>(this)->visit(v); }
  virtual void visit( const U& v ) override { static_cast<VA*>(this)->visit(v); }

  ~VisitorAdapter_impl() = default;
};

template< typename... Ts >
template< typename F >
class Tree<Ts...>:: VisitorAdapter
  : public VisitorAdapter_impl< F, Ts... >
{
public:
  template< typename T >
  void visit( T&& v ){ f_( std::forward<T>(v) ); }

  virtual void visit(       NodeTerminal& n ) override { f_( n ); }
  virtual void visit( const NodeTerminal& n ) override { f_( n ); }

  virtual void visit(       NodeUnary& n ) override { f_( n ); }
  virtual void visit( const NodeUnary& n ) override { f_( n ); }

  virtual void visit(       NodeBinary& n ) override { f_( n ); }
  virtual void visit( const NodeBinary& n ) override { f_( n ); }

  VisitorAdapter( F f ) : f_(f) {};
private:
  F f_;
};

template< typename... Ts >
template< typename F >
Tree<Ts...>::VisitorAdapter<F> Tree<Ts...>::adaptVisitor( const F& f ){
  return {f};
}

#endif // __TREE_HPP__

