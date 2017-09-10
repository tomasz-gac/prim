#ifndef __TREE_IMPL_HPP__
#define __TREE_IMPL_HPP__

#include "disjunction.hpp"
#include <vector>
#include <array>

template< typename... Ts >
class Tree;

template< typename  >
class IVisitor;

template< typename >
class INode;

template< typename >
class INode_interface;

template< typename Tree_t, typename F >
class Adapter;

namespace helpers__{
  template< typename T >
  using remove_const = typename std::remove_const< T >::type;

  template< typename T >
  using add_const = typename std::add_const< T >::type;

}

namespace tree_impl__{
  
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
    class Extend< Tree< Ts... >, Derived, Base >
      : public Base
    {
    private:
      using INode          = ::INode< Tree<Ts...> >;
      using IVisitor       = ::IVisitor<       Tree<Ts...> >;
      using IVisitor_const = ::IVisitor< const Tree<Ts...> >;
    public:
      static_assert( std::is_base_of< INode, Base >::value, "Base has to be derived from INode");
      static_assert( disjunction< std::is_same< Derived, Ts >... >::value, "Type not supported by Tree." );

      using Base::Base;

      template< typename T >
      using extend = Extend< Tree<Ts...>, T, Derived >;
      template< typename > struct print_type;
      
      virtual void accept( IVisitor& v ) override {
	static_cast< tree_impl__::IVisitor<       Derived >& >(v)
	  .visit( static_cast<       Derived& >(*this) );
      }

      virtual void accept( IVisitor_const& v ) const override {
	static_cast< tree_impl__::IVisitor< const Derived >& >(v)
	  .visit( static_cast< const Derived& >(*this) );
      }
    
      virtual Tree<Ts...> clone() const override{
	return Tree<Ts...>( new Derived( static_cast<const Derived&>(*this) ) );
      }
    };

    template< typename... Ts, size_t N, typename Derived >
    struct Static< Tree< Ts... >, Derived, N >
      : public Extend< Tree<Ts...>, Derived, INode< Tree<Ts...>  > >
    {
      template< typename... Us >
      Static( Us&&... vs )
	: children{ std::forward<Us>(vs)... }
      {  }

      std::array< Tree<Ts...>, N > children;
    };

    template< typename... Ts, typename Derived >
    struct Terminal< Tree< Ts...>, Derived >
      : public Extend< Tree<Ts...>, Derived, INode< Tree< Ts... > > >
    {  };

    template< typename... Ts, typename Derived >
    struct Dynamic< Tree< Ts... >, Derived >
      : public Extend< Tree<Ts...>, Derived, INode< Tree<Ts...> > >
    {
      Dynamic() = default;

      std::vector< Tree<Ts...> > children;
    };

    template< typename Tree_t, typename F, typename U, typename... Us >
    class Adapter
      : public tree_impl__::CRTP::Adapter< Tree_t, F, Us... >
    {
    public:
      virtual void visit( U& v ) override { static_cast<::Adapter<Tree_t, F>*>(this)->f(v); }

      virtual ~Adapter() = default;
    };
    
    template< typename Tree_t, typename F, typename U >
    class Adapter< Tree_t, F, U >
      : public ::IVisitor< Tree_t >
    {
    public:
      virtual void visit( U& v ) override { static_cast<::Adapter<Tree_t, F>*>(this)->f(v); }

      virtual ~Adapter() = default;
    };

  } // namespace CRTP
}   // namespace tree_impl__

template< typename... Ts >
class IVisitor< Tree< Ts... > >
  : public tree_impl__::IVisitor< helpers__::remove_const<Ts> >...
{  };

template< typename... Ts >
class IVisitor< const Tree< Ts... > >
  : public tree_impl__::IVisitor< helpers__::add_const<Ts> >...
{  };

template< typename... Ts >
class INode_interface< Tree< Ts... > >
{  };
  
template< typename... Ts >
class INode< Tree< Ts... > >
  : public INode_interface< Tree< Ts... > >
{
public:
  template< typename Derived >
  using extend = tree_impl__::CRTP::Extend< Tree<Ts...>, Derived, INode >;

  virtual void accept( IVisitor<       Tree<Ts...> >& )       = 0;
  virtual void accept( IVisitor< const Tree<Ts...> >& ) const = 0;

  virtual Tree< Ts... > clone() const = 0;

  virtual ~INode() = default;
};

template< typename... Ts, typename F >
struct Adapter< Tree<Ts...>, F >
  : public tree_impl__::CRTP::Adapter<       Tree< Ts...>, F,  helpers__::remove_const<Ts>... >
{
  Adapter( F& f_ ) : f(f_) {};

  F& f;
};

template< typename... Ts, typename F >
struct Adapter< const Tree<Ts...>, F >
  : public tree_impl__::CRTP::Adapter< const Tree< Ts...>, F, helpers__::add_const<Ts>... >
{
  Adapter( F& f_ ) : f(f_) {};

  F& f;
};

template< typename Derived >
struct visitor{
public:
  template< typename... Ts >
  void visit( INode< Tree<Ts...> >& node ){
    auto adapter = Adapter< Tree<Ts...>, Derived >{
      static_cast<Derived&>(*this)
    };
    node.accept( adapter );
  }

  template< typename... Ts >
  void visit( Tree<Ts...>& tree ){
    visit( tree.node() );
  }

};

template< typename Derived >
struct const_visitor{
  template< typename... Ts >
  void visit( const INode< Tree<Ts...> >& node ){
    auto adapter = Adapter< const Tree<Ts...>, Derived >{
      static_cast<Derived&>(*this)
    };
    node.accept( adapter );
  }

  template< typename... Ts >
  void visit( const Tree<Ts...>& tree ){
    visit( tree.node() );
  }
};

#endif // __TREE_IMPL_HPP__
