#ifndef __TREE_IMPL_HPP__
#define __TREE_IMPL_HPP__

#include "disjunction.hpp"

template< typename... Ts >
class Tree;

namespace tree_impl__{
  template< typename T >
  class IVisitor{
  public:
    virtual void visit(      T& ) = 0;
    virtual void visit(const T& ) = 0;

    virtual ~IVisitor() = default;
  };

  namespace CRTP {
    
    template< typename T, typename Derived, typename Base >
    class Extend;

    template< typename T, typename Derived, size_t N >
    struct Static;

    template< typename T, typename Derived >
    struct Dynamic;

    template< typename... Ts, typename Derived, typename Base >
    class Extend< Tree< Ts... >, Derived, Base >
      : public Base
    {
    private:
      using INode        = typename Tree<Ts...>::INode;
      using INodeVisitor = typename Tree<Ts...>::INodeVisitor;
    public:
      static_assert( std::is_base_of< INode, Base >::value, "Base has to be derived from INode");
      static_assert( disjunction< std::is_same< Derived, Ts >... >::value, "Type not supported by Tree." );

      using Base::Base;

      template< typename T >
      using extend = Extend< Tree<Ts...>, T, Derived >;
    
      virtual void accept( INodeVisitor& v ) override
      { v.dispatch( static_cast<       Derived& >( *this )); }

      virtual void accept( INodeVisitor& v ) const override
      { v.dispatch( static_cast< const Derived& >( *this )); }
    
      virtual Tree<Ts...> clone() const override{
	return Tree<Ts...>( new Derived( static_cast<const Derived&>(*this) ) );
      }
    };

    template< typename... Ts, size_t N, typename Derived >
    struct Static< Tree< Ts... >, Derived, N >
      : public Extend< Tree<Ts...>, Derived, typename Tree<Ts...>::INode >
    {
      template< typename... Us >
      Static( Us&&... vs )
	: children{ std::forward<Us>(vs)... }
      {  }

      std::array< Tree<Ts...>, N > children;
    };

    template< typename... Ts, typename Derived >
    struct Dynamic< Tree< Ts... >, Derived >
      : public Extend< Tree<Ts...>, Derived, typename Tree<Ts...>::INode >
    {
      Dynamic() = default;

      std::vector< Tree<Ts...> > children;
    };

  } // namespace CRTP
}   // namespace tree_impl__

#endif // __TREE_IMPL_HPP__
