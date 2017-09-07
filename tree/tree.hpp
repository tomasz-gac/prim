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
  using IVisitor       = ::IVisitor<       Tree >;
  using IVisitor_const = ::IVisitor< const Tree >;
  
  using INode = ::INode< Tree >;

  template< typename Derived >
  using Terminal = tree_impl__::CRTP::Terminal< Tree, Derived >;
  
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
    node_->accept( visitor );
  }
  void accept( IVisitor_const& visitor ) const {
    node_->accept( visitor );
  }
  
        INode& node()       { return *node_; }
  const INode& node() const { return *node_; }

        INode* operator->()       { return &*node_; }
  const INode* operator->() const { return &*node_; }

  template< typename F >
  static Adapter<       Tree, F> adapt( F& f );

  template< typename F  >
  static Adapter< const Tree, F> adapt_const( F& f );

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
template< typename F >
Adapter<Tree<Ts...>, F> Tree<Ts...>::adapt( F& f ){
  return { f };
}

template< typename... Ts >
template< typename F >
Adapter<const Tree<Ts...>, F> Tree<Ts...>::adapt_const( F& f ){
  return {f};
}

#include "children_iterator_impl.hpp"

#endif // __TREE_HPP__

