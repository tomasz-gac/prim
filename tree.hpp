#ifndef __TREE_HPP__
#define __TREE_HPP__

#include <memory>
#include <array>
#include <vector>
#include <type_traits>

template< typename T >
class IVisitor{
public:
  virtual void visit(      T& ) = 0;
  virtual void visit(const T& ) = 0;

  virtual ~IVisitor() = default;
};


template<class...> struct disjunction : std::false_type { };
template<class B1> struct disjunction<B1> : B1 { };
template<class B1, class... Bn>
struct disjunction<B1, Bn...> 
  : std::conditional<bool(B1::value), B1, disjunction<Bn...>>::type  { };

template< typename... Ts >
class Tree{
public:
  class INode;
  using node_ptr = std::shared_ptr< INode >;

public:
  class INodeVisitor
    : public IVisitor<Ts>...
  {
  public:
    template< typename T >
    void dispatch( T& object ){
      static_cast<IVisitor<typename std::remove_const<T>::type>*>(this)->visit( object );
    }
  };

private:
  template< typename F >
  class VisitorAdapter;
  
  template< typename F, typename U, typename... Us >
  class VisitorAdapter_impl;

  template< typename Derived, typename Base >
  class extend_CRTP;

  template< size_t N, typename Derived >
  struct Static_CRTP;

  template< typename Derived >
  struct Dynamic_CRTP;

public:
  template< typename F >
  static VisitorAdapter<F> adaptVisitor( const F& f );
  
  class INode
    : public std::enable_shared_from_this<INode>
  {
  public:
    template< typename Derived >
    using extend = extend_CRTP< Derived, INode >;

    template< size_t N, typename Derived >
    using Static = Static_CRTP< N, Derived >;

    template< typename Derived >
    using Unary = Static_CRTP< 1, Derived >;

    template< typename Derived >
    using Binary = Static_CRTP< 2, Derived >;

    template< typename Derived >
    using Dynamic = Dynamic_CRTP< Derived >;

    virtual void accept( INodeVisitor& )       = 0;
    virtual void accept( INodeVisitor& ) const = 0;

    virtual node_ptr ref()         = 0;
    virtual node_ptr clone() const = 0;

    virtual ~INode() = default;
  };
  
private:
  template< typename Derived, typename Base >
  class extend_CRTP
    : public Base
  {
  public:
    static_assert( std::is_base_of< INode, Base >::value, "Base has to be derived from INode");
    static_assert( disjunction< std::is_same< Derived, Ts >... >::value, "Type not supported by Tree." );

    using Base::Base;

    template< typename T >
    using extend = extend_CRTP< T, Derived >;
    
    virtual void accept( INodeVisitor& v )       override { v.dispatch( static_cast<       Derived& >( *this )); }
    virtual void accept( INodeVisitor& v ) const override { v.dispatch( static_cast< const Derived& >( *this )); }
    
    virtual node_ptr ref() override{
      return static_cast<Derived*>(this)->shared_from_this();
    }

    virtual node_ptr clone() const override{
      return std::make_shared<Derived>( static_cast<const Derived&>(*this) );
    }
  };

  template< size_t N, typename Derived >
  struct Static_CRTP
    : public extend_CRTP< Derived, INode >
  {
    std::array< node_ptr, N > children;
  };
  
  template< typename Derived >
  struct Dynamic_CRTP
    : public extend_CRTP< Derived, INode >
  {
    std::vector< node_ptr > children;
  };
  
public:
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

  Tree( const Tree& other )
    : node_( other.node_->ref() )
  {  }

  Tree( Tree&& other )
    : node_( std::move( other.node_ ) )
  {  }
  
private:  
  Tree() = delete;
  Tree( INode*&& node )
  {
    node_.reset(node);
  }
  
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
  : public Tree<Ts...>::INodeVisitor
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

  VisitorAdapter( F f ) : f_(f) {};

  F f_;
};

template< typename... Ts >
template< typename F >
Tree<Ts...>::VisitorAdapter<F>Tree<Ts...>::adaptVisitor( const F& f ){
  return { f };
}

#endif // __TREE_HPP__

