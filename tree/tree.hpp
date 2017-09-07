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
  class IVisitor
    : public tree_impl__::IVisitor_impl<typename std::remove_const<Ts>::type >...
  {  };

  class IVisitor_const
    : public tree_impl__::IVisitor_impl< typename std::add_const<Ts>::type >...
  {  };

private:
  template< typename F >
  class Adapter;
  
  template< typename F >
  class Adapter_const;
  
  template< typename F, typename U, typename... Us >
  class Adapter_impl;

public:
  template< typename F >
  static Adapter<F> adapt( F& f );
  
  template< typename F >
  static Adapter_const<F> adapt_const( F& f );
  
public:
  class INode{
  public:
    template< typename Derived >
    using extend = tree_impl__::CRTP::Extend< Tree, Derived, INode >;

    virtual void accept( IVisitor& )       = 0;
    virtual void accept( IVisitor_const& ) const = 0;

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
  void accept( IVisitor& visitor )       {
    // using non_const_accept = void (INode::*)( IVisitor& ); // accept
    // ((*node_).*non_const_accept(&(*node_).accept))( visitor );
    node_->accept( visitor );
  }
  void accept( IVisitor_const& visitor ) const {
    // using const_accept = void (INode::*)( IVisitor& ) const; // const accept
    // ((*node_).*const_accept(&(*node_).accept))( visitor );
    node_->accept( visitor );
  }
  
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
class Tree<Ts...>::Adapter_impl
  : public Adapter_impl< F, Us... >
{
public:
  virtual void visit( U& v ) override { static_cast<F*>(this)->visit(v); }

  virtual ~Adapter_impl() = default;
};

template< typename... Ts >
template< typename F, typename U >
class Tree<Ts...>::Adapter_impl< F, U >
  : public std::conditional<
  std::is_const<U>::value
  , Tree<Ts...>::IVisitor_const
  , Tree<Ts...>::IVisitor
  >::type
{
public:
  virtual void visit( U& v ) override { static_cast<F*>(this)->visit(v); }

  virtual ~Adapter_impl() = default;
};

template< typename... Ts >
template< typename F >
class Tree<Ts...>::Adapter
  : public Adapter_impl< Adapter<F>, typename std::remove_const<Ts>::type... >
{
public:
  template< typename T >
  void visit( T& v ){
    f_( v );
  }

  Adapter( F& f ) : f_(f) {};

  F& f_;
};

template< typename... Ts >
template< typename F >
class Tree<Ts...>::Adapter_const
  : public Adapter_impl< Adapter_const<F>, typename std::add_const<Ts>::type... >
{
public:
  template< typename T >
  void visit( T& v ){
    f_( v );
  }

  Adapter_const( F& f ) : f_(f) {};

  F& f_;
};

template< typename... Ts >
template< typename F >
Tree<Ts...>::Adapter<F> Tree<Ts...>::adapt( F& f ){
  return { f };
}

template< typename... Ts >
template< typename F >
Tree<Ts...>::Adapter_const<F> Tree<Ts...>::adapt_const( F& f ){
  return { f };
}

#include "children_iterator_impl.hpp"

#endif // __TREE_HPP__

