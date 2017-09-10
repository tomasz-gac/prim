#ifndef __NODE_IMPL_HPP__
#define __NODE_IMPL_HPP__

#include "disjunction.hpp"
#include <vector>
#include <array>

template< typename... Ts >
class Node;

template< typename  >
class IVisitor;

template< typename >
class INode;

template< typename >
class INode_interface;

template< typename Node_t, typename F >
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
    class Extend< Node< Ts... >, Derived, Base >
      : public Base
    {
    private:
      using INode          = ::INode< Node<Ts...> >;
      using IVisitor       = ::IVisitor<       Node<Ts...> >;
      using IVisitor_const = ::IVisitor< const Node<Ts...> >;
    public:
      static_assert( std::is_base_of< INode, Base >::value, "Base has to be derived from INode");
      static_assert( disjunction< std::is_same< Derived, Ts >... >::value, "Type not supported by Node." );

      using Base::Base;

      template< typename T >
      using extend = Extend< Node<Ts...>, T, Derived >;
      template< typename > struct print_type;
      
      virtual void accept( IVisitor& v ) override {
	static_cast< node_impl__::IVisitor<       Derived >& >(v)
	  .visit( static_cast<       Derived& >(*this) );
      }

      virtual void accept( IVisitor_const& v ) const override {
	static_cast< node_impl__::IVisitor< const Derived >& >(v)
	  .visit( static_cast< const Derived& >(*this) );
      }
    
      virtual Node<Ts...> clone() const override{
	return Node<Ts...>( new Derived( static_cast<const Derived&>(*this) ) );
      }
    };

    template< typename... Ts, size_t N, typename Derived >
    struct Static< Node< Ts... >, Derived, N >
      : public Extend< Node<Ts...>, Derived, INode< Node<Ts...>  > >
    {
      template< typename... Us >
      Static( Us&&... vs )
	: children{ std::forward<Us>(vs)... }
      {  }

      std::array< std::reference_wrapper< Node<Ts...> >, N > children;
    };

    template< typename... Ts, typename Derived >
    struct Terminal< Node< Ts...>, Derived >
      : public Extend< Node<Ts...>, Derived, INode< Node< Ts... > > >
    {  };

    template< typename... Ts, typename Derived >
    struct Dynamic< Node< Ts... >, Derived >
      : public Extend< Node<Ts...>, Derived, INode< Node<Ts...> > >
    {
      Dynamic() = default;

      std::vector< std::reference_wrapper< Node<Ts...> > > children;
    };

    template< typename Node_t, typename F, typename U, typename... Us >
    class Adapter
      : public node_impl__::CRTP::Adapter< Node_t, F, Us... >
    {
    public:
      virtual void visit( U& v ) override { static_cast<::Adapter<Node_t, F>*>(this)->f(v); }

      virtual ~Adapter() = default;
    };
    
    template< typename Node_t, typename F, typename U >
    class Adapter< Node_t, F, U >
      : public ::IVisitor< Node_t >
    {
    public:
      virtual void visit( U& v ) override { static_cast<::Adapter<Node_t, F>*>(this)->f(v); }

      virtual ~Adapter() = default;
    };

  } // namespace CRTP
}   // namespace node_impl__

template< typename... Ts >
class IVisitor< Node< Ts... > >
  : public node_impl__::IVisitor< helpers__::remove_const<Ts> >...
{  };

template< typename... Ts >
class IVisitor< const Node< Ts... > >
  : public node_impl__::IVisitor< helpers__::add_const<Ts> >...
{  };

template< typename... Ts >
class INode_interface< Node< Ts... > >
{  };
  
template< typename... Ts >
class INode< Node< Ts... > >
  : public INode_interface< Node< Ts... > >
{
public:
  template< typename Derived >
  using extend = node_impl__::CRTP::Extend< Node<Ts...>, Derived, INode >;

  virtual void accept( IVisitor<       Node<Ts...> >& )       = 0;
  virtual void accept( IVisitor< const Node<Ts...> >& ) const = 0;

  virtual Node< Ts... > clone() const = 0;

  virtual ~INode() = default;
};

template< typename... Ts, typename F >
struct Adapter< Node<Ts...>, F >
  : public node_impl__::CRTP::Adapter<       Node< Ts...>, F,  helpers__::remove_const<Ts>... >
{
  Adapter( F& f_ ) : f(f_) {};

  F& f;
};

template< typename... Ts, typename F >
struct Adapter< const Node<Ts...>, F >
  : public node_impl__::CRTP::Adapter< const Node< Ts...>, F, helpers__::add_const<Ts>... >
{
  Adapter( F& f_ ) : f(f_) {};

  F& f;
};

template< typename Derived >
struct visitor{
public:
  template< typename... Ts >
  void visit( INode< Node<Ts...> >& node ){
    auto adapter = Adapter< Node<Ts...>, Derived >{
      static_cast<Derived&>(*this)
    };
    node.accept( adapter );
  }

  template< typename... Ts >
  void visit( Node<Ts...>& node ){
    visit( *node );
  }

  template< typename... Ts >
  void visit( std::reference_wrapper< Node<Ts...> > node ){
    visit( node.get() );
  }
};

template< typename Derived >
struct const_visitor{
  template< typename... Ts >
  void visit( const INode< Node<Ts...> >& node ){
    auto adapter = Adapter< const Node<Ts...>, Derived >{
      static_cast<Derived&>(*this)
    };
    node.accept( adapter );
  }

  template< typename... Ts >
  void visit( const Node<Ts...>& node ){
    visit( *node );
  }

  template< typename... Ts >
  void visit( const std::reference_wrapper< Node<Ts...> > node ){
    visit( node.get() );
  }

};

#endif // __NODE_IMPL_HPP__
