#ifndef __TREE_HPP__
#define __TREE_HPP__

#include <memory>
#include <array>
#include <vector>
#include <type_traits>
#include "tree_impl.hpp"

template< typename... Ts >
class Tree{
public:
  class INode;
  
public:
  class INodeVisitor
    : public tree_impl__::IVisitor<Ts>...
  {
  public:
    template< typename T >
    void dispatch( T& object ){
      static_cast< tree_impl__::IVisitor<typename std::remove_const<T>::type>*>(this)->visit( object );
    }
  };

private:
  template< typename F >
  class VisitorAdapter;
  
  template< typename F, typename U, typename... Us >
  class VisitorAdapter_impl;

public:
  template< typename F >
  static VisitorAdapter<F> adaptVisitor( F& f );
  
public:
  class INode{
  public:
    template< typename Derived >
    using extend = tree_impl__::CRTP::Extend< Tree, Derived, INode >;

    virtual void accept( INodeVisitor& )       = 0;
    virtual void accept( INodeVisitor& ) const = 0;

    virtual Tree clone() const = 0;

    virtual ~INode() = default;
  };

  template< typename Derived >
  using Unary = tree_impl__::CRTP::Static< Tree, Derived, 1 >;

  template< typename Derived >
  using Binary = tree_impl__::CRTP::Static< Tree, Derived, 2 >;

  template< typename Derived, size_t N >
  using Static = tree_impl__::CRTP::Static< Tree, Derived, N >;

  template< typename Derived >
  using Dynamic = tree_impl__::CRTP::Dynamic< Tree, Derived >;

  template< typename T, typename Derived, typename Base >
  friend class tree_impl__::CRTP::Extend; // For private constructors

  template< typename T, typename... Us >
  static
  Tree make( Us&&... Vs ){
    static_assert( disjunction< std::is_same< T, Ts >... >::value, "Type not supported by Tree." );
    return Tree( new T( std::forward<Us>(Vs)... ) );
  }

  void accept( INodeVisitor& visitor )       { node_->accept( visitor ); }
  void accept( INodeVisitor& visitor ) const { node_->accept( visitor ); }
  
        INode& node()       { return *node_; }
  const INode& node() const { return *node_; }

        INode* operator->()       { return &*node_; }
  const INode* operator->() const { return &*node_; }

  Tree& operator=( Tree other ){
    node_ = other.node_;
    return *this;
  }

  Tree( const Tree& other )
    : node_( other.node_ )
  {  }

  Tree( Tree&& other )
    : node_( std::move( other.node_ ) )
  {  }
  
private:  
  Tree() = delete;
  Tree( INode*&& node ){
    node_.reset(node);
  }
  
  std::shared_ptr< INode > node_;
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
  : public Tree<Ts...>::INodeVisitor
{
  using VA = Tree<Ts...>::VisitorAdapter<F>;
public:
  virtual void visit(       U& v ) override { static_cast<VA*>(this)->visit(v); }
  virtual void visit( const U& v ) override { static_cast<VA*>(this)->visit(v); }

  virtual ~VisitorAdapter_impl() = default;
};

template< typename... Ts >
template< typename F >
class Tree<Ts...>:: VisitorAdapter
  : public VisitorAdapter_impl< F, Ts... >
{
public:
  template< typename T >
  void visit( T&& v ){ f_( std::forward<T>(v) ); }

  VisitorAdapter( F& f ) : f_(f) {};

  F& f_;
};

template< typename... Ts >
template< typename F >
Tree<Ts...>::VisitorAdapter<F>Tree<Ts...>::adaptVisitor( F& f ){
  return { f };
}

#include "children_iterator_impl.hpp"

#endif // __TREE_HPP__

