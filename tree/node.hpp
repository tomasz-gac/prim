#ifndef __NODE_HPP__
#define __NODE_HPP__

#include <memory>
#include <type_traits>
#include "node_impl.hpp"
#include "short_alloc.hpp"

template< size_t, size_t... >
class max;

template< size_t N, size_t M, size_t... Ns >
struct max< N, M, Ns... >
  : public std::conditional< (M > N), max< M, Ns... >, max< N, Ns... > >::type
{  };

template< size_t N >
struct max< N > : std::integral_constant< size_t, N >
{  };

template< typename... Ts >
class Node< INode< Ts... > >{
public:
  using INode = ::INode< Ts... >;

  using IVisitor       = ::IVisitor<       INode >;
  using IVisitor_const = ::IVisitor< const INode >;

  template< typename Derived >
  using Terminal = node_impl__::CRTP::Terminal< INode, Derived >;
  
  template< typename Derived >
  using Unary = node_impl__::CRTP::Static< INode, Derived, 1 >;

  template< typename Derived >
  using Binary = node_impl__::CRTP::Static< INode, Derived, 2 >;

  template< typename Derived, size_t N >
  using Static = node_impl__::CRTP::Static< INode, Derived, N >;

  template< typename Derived >
  using Dynamic = node_impl__::CRTP::Dynamic< INode, Derived >;

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
  static Adapter<       INode, F> adapt( F& f );

  template< typename F  >
  static Adapter< const INode, F> adapt_const( F& f );

  Node& operator=( Node other ){
    node_ = other.node_;
    return *this;
  }

  Node( Node&& other )
    : node_( std::move( other.node_ ) )
  {  }
  
protected:
  Node( INode*&& ptr )
  { node_.reset(ptr);  }
  Node() = default;

  std::unique_ptr< INode > node_;
};


template< typename... Ts >
template< typename F >
Adapter<INode<Ts...>, F> Node< INode<Ts...>>::adapt( F& f ){
  return { f };
}

template< typename... Ts >
template< typename F >
Adapter<const INode<Ts...>, F> Node<INode<Ts...>>::adapt_const( F& f ){
  return {f};
}

#include "children_iterator_impl.hpp"

#endif // __NODE_HPP__

