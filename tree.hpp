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

public:
  template< typename F >
  static VisitorAdapter<F> adaptVisitor( F& f );
  
  class INode
    : public std::enable_shared_from_this<INode>
  {
  public:
    template< typename Derived >
    using extend = extend_CRTP< Derived, INode >;

    virtual void accept( INodeVisitor& )       = 0;
    virtual void accept( INodeVisitor& ) const = 0;

    virtual Tree clone() const = 0;

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

    extend_CRTP( const extend_CRTP& ) = delete;
    extend_CRTP() = default;

    template< typename T >
    using extend = extend_CRTP< T, Derived >;
    
    virtual void accept( INodeVisitor& v ) override
    { v.dispatch( static_cast<       Derived& >( *this )); }

    virtual void accept( INodeVisitor& v ) const override
    { v.dispatch( static_cast< const Derived& >( *this )); }
    
    virtual Tree clone() const override{
      return Tree( new Derived( static_cast<const Derived&>(*this) ) );
    }
  };
public:
  template< size_t N, typename Derived >
  struct Static
    : public extend_CRTP< Derived, INode >
  {
    template< typename... Us >
    Static( Us&&... vs )
      : children{ std::forward<Us>(vs)... }
    {  }

    Static( const Static& other )
      : children( other.children ) // shallow copy
    {
      for( auto& child : children )
	child = child.node().clone();
    }

    std::array< Tree, N > children;
  };

  
  template< typename Derived >
  struct Dynamic
    : public extend_CRTP< Derived, INode >
  {
    Dynamic() = default;
    Dynamic( Dynamic&& ) = default;
    Dynamic( const Dynamic& other ){
      children.reserve( other.children.size() );
      for( auto& child : other.children )
	children.push_back( child.node().clone() );
    }

    std::vector< Tree > children;
  };

  template< typename Derived >
  using Unary = Static< 1, Derived >;

  template< typename Derived >
  using Binary = Static< 2, Derived >;

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

  Tree& operator=( Tree other  ){
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

#endif // __TREE_HPP__

