#ifndef __NODE_IMPL_HPP__
#define __NODE_IMPL_HPP__

#include "disjunction.hpp"
#include <vector>
#include <array>

template< typename >
class Node;

template< typename  >
class IVisitor;

template< typename... >
class INode;

template< typename >
class INode_interface;

template< typename INode_t, typename F >
class Adapter;

namespace helpers__{
  template< typename T >
  using remove_const = typename std::remove_const< T >::type;

  template< typename T >
  using add_const = typename std::add_const< T >::type;

}

namespace node_impl__{
  
  template< typename T >
  class IVisitor{
  public:
    virtual void visit( T& ) = 0;

    virtual ~IVisitor() = default;
  };

  namespace CRTP {
    
    template< typename T, typename Derived, typename Base >
    class Extend;

    template< typename T, typename Derived >
    class Terminal;

    template< typename T, typename Derived, size_t N >
    struct Static;

    template< typename T, typename Derived >
    struct Dynamic;

    template< typename... Ts, typename Derived, typename Base >
    class Extend< INode< Ts... >, Derived, Base >
      : public Base
    {
    private:
      using INode          = ::INode< Ts... >;
      using IVisitor       = ::IVisitor<       INode >;
      using IVisitor_const = ::IVisitor< const INode >;
    public:
      static_assert( std::is_base_of< INode, Base >::value, "Base has to be derived from INode");
      static_assert( disjunction< std::is_same< Derived, Ts >... >::value, "Type not supported by Node." );

      using Base::Base;

      template< typename T >
      using extend = Extend< INode, T, Derived >;
      
      virtual void accept( IVisitor& v ) override {
	static_cast< node_impl__::IVisitor<       Derived >& >(v)
	  .visit( static_cast<       Derived& >(*this) );
      }

      virtual void accept( IVisitor_const& v ) const override {
	static_cast< node_impl__::IVisitor< const Derived >& >(v)
	  .visit( static_cast< const Derived& >(*this) );
      }
    
      virtual Node< INode > clone() const override{
	return Node< INode >( new Derived( static_cast<const Derived&>(*this) ) );
      }
    };

    template< typename... Ts, size_t N, typename Derived >
    struct Static< INode< Ts... >, Derived, N >
      : public Extend< INode<Ts...>, Derived, INode<Ts...> >
    {
      template< typename... Us >
      Static( Us&&... vs )
	: children{ std::forward<Us>(vs)... }
      {  }

      std::array< std::reference_wrapper< INode< Ts... > >, N > children;
    };

    template< typename... Ts, typename Derived >
    struct Terminal< INode< Ts...>, Derived >
      : public Extend< INode<Ts...>, Derived, INode< Ts... > >
    {  };

    template< typename... Ts, typename Derived >
    struct Dynamic< INode< Ts... >, Derived >
      : public Extend< INode<Ts...>, Derived, INode<Ts...> >
    {
      Dynamic() = default;

      std::vector< std::reference_wrapper< INode< Ts... > > > children;
    };

    template< typename INode_t, typename F, typename U, typename... Us >
    class Adapter
      : public node_impl__::CRTP::Adapter< INode_t, F, Us... >
    {
    public:
      virtual void visit( U& v ) override { static_cast<::Adapter<INode_t, F>*>(this)->f(v); }

      virtual ~Adapter() = default;
    };
    
    template< typename INode_t, typename F, typename U >
    class Adapter< INode_t, F, U >
      : public ::IVisitor< INode_t >
    {
    public:
      virtual void visit( U& v ) override { static_cast<::Adapter<INode_t, F>*>(this)->f(v); }

      virtual ~Adapter() = default;
    };

  } // namespace CRTP
}   // namespace node_impl__

template< typename... Ts >
class IVisitor< INode< Ts... > >
  : public node_impl__::IVisitor< helpers__::remove_const<Ts> >...
{  };

template< typename... Ts >
class IVisitor< const INode< Ts... > >
  : public node_impl__::IVisitor< helpers__::add_const<Ts> >...
{  };

template< typename... Ts >
class INode_interface< INode< Ts... > >
{  };
  
template< typename... Ts >
class INode
  : public INode_interface< INode< Ts... > >
{
public:
  template< typename Derived >
  using extend = node_impl__::CRTP::Extend< INode, Derived, INode >;

  virtual void accept( IVisitor<       INode<Ts...> >& )       = 0;
  virtual void accept( IVisitor< const INode<Ts...> >& ) const = 0;

  virtual Node< INode > clone() const = 0;

  virtual ~INode() = default;
};

template< typename... Ts, typename F >
struct Adapter< INode<Ts...>, F >
  : public node_impl__::CRTP::Adapter<       INode< Ts...>, F,  helpers__::remove_const<Ts>... >
{
  Adapter( F& f_ ) : f(f_) {};

  F& f;
};

template< typename... Ts, typename F >
struct Adapter< const INode<Ts...>, F >
  : public node_impl__::CRTP::Adapter< const INode< Ts...>, F, helpers__::add_const<Ts>... >
{
  Adapter( F& f_ ) : f(f_) {};

  F& f;
};

template< typename Derived >
struct visitor{
public:
  template< typename... Ts >
  void visit( INode< Ts... >& node ){
    auto adapter = Adapter< INode<Ts...>, Derived >{
      static_cast<Derived&>(*this)
    };
    node.accept( adapter );
  }

  template< typename... Ts >
  void visit( Node<Ts...>& node ){
    visit( *node );
  }

  template< typename... Ts >
  void visit( std::reference_wrapper< INode< Ts... > > node ){
    visit( node.get() );
  }
};

template< typename Derived >
struct const_visitor{
  template< typename... Ts >
  void visit( const INode< Ts... >& node ){
    auto adapter = Adapter< const INode<Ts...>, Derived >{
      static_cast<Derived&>(*this)
    };
    node.accept( adapter );
  }

  template< typename... Ts >
  void visit( const Node< INode<Ts...> >& node ){
    visit( *node );
  }

  template< typename... Ts >
  void visit( const std::reference_wrapper< INode< Ts... > > node ){
    visit( node.get() );
  }

};

#endif // __NODE_IMPL_HPP__
