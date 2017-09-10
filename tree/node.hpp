#ifndef __NODE_HPP__
#define __NODE_HPP__

#include <memory>
#include <type_traits>
#include "node_impl.hpp"

template< typename... Ts >
class Node{
public:
  using IVisitor       = ::IVisitor<       Node >;
  using IVisitor_const = ::IVisitor< const Node >;

  using INode = ::INode< Node >;

  template< typename Derived >
  using Terminal = node_impl__::CRTP::Terminal< Node, Derived >;
  
  template< typename Derived >
  using Unary = node_impl__::CRTP::Static< Node, Derived, 1 >;

  template< typename Derived >
  using Binary = node_impl__::CRTP::Static< Node, Derived, 2 >;

  template< typename Derived, size_t N >
  using Static = node_impl__::CRTP::Static< Node, Derived, N >;

  template< typename Derived >
  using Dynamic = node_impl__::CRTP::Dynamic< Node, Derived >;

  template< typename T, typename Derived, typename Base >
  friend class node_impl__::CRTP::Extend; // For private constructors

  template< typename T, typename... Us >
  static
  Node make( Us&&... Vs ){
    static_assert( disjunction< std::is_same< T, Ts >... >::value, "Type not supported by Node." );
    return Node( new T( std::forward<Us>(Vs)... ) );
  }
  void accept( IVisitor& visitor )       {
    node_->accept( visitor );
  }
  void accept( IVisitor_const& visitor ) const {
    node_->accept( visitor );
  }
  
        INode& operator*()       { return *node_; }
  const INode& operator*() const { return *node_; }

        INode* operator->()       { return &*node_; }
  const INode* operator->() const { return &*node_; }

  template< typename F >
  static Adapter<       Node, F> adapt( F& f );

  template< typename F  >
  static Adapter< const Node, F> adapt_const( F& f );

  Node& operator=( Node other ){
    node_ = other.node_;
    return *this;
  }

  Node( const Node& other ) = delete;
  //   : node_( other.node_ )
  // {  }

  Node( Node&& other )
    : node_( std::move( other.node_ ) )
  {  }
  
protected:  
  Node() = delete;
  Node( INode*&& node ){
    node_.reset(node);
  }
private:  
  std::unique_ptr< INode > node_;
};


template< typename... Ts >
template< typename F >
Adapter<Node<Ts...>, F> Node<Ts...>::adapt( F& f ){
  return { f };
}

template< typename... Ts >
template< typename F >
Adapter<const Node<Ts...>, F> Node<Ts...>::adapt_const( F& f ){
  return {f};
}

#include "children_iterator_impl.hpp"

#endif // __NODE_HPP__

