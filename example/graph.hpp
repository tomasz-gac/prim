#ifndef __GRAPH_HPP__
#define __GRAPH_HPP__

#include "poly/poly.hpp"
#include <array>
#include <vector>

template< typename T, typename StoragePolicy >
struct Node : StoragePolicy
{
  Node( T v )
    : value( std::move(v) )
  {  }
  
  T value;
};

template< typename Graph_type >
class Visitor;

template< typename Return, typename... Ts >
class Graph;

template< typename Visitor_type >
struct accept;

template< typename... Ts, typename Return >
struct accept< Visitor< Graph< Return, Ts... > > >
  : Signature< Return( Visitor< Graph< Return, Ts... > >& ) >
{  };

template< typename... Ts, typename Return >
struct accept< Visitor< const Graph< Return, Ts... > > >
  : Signature< Return( Visitor< const Graph< Return, Ts... > >& ) >
{  };


template< typename... Ts, typename Return, typename T, typename StoragePolicy >
constexpr auto invoke<
  accept< Visitor< Graph< Return, Ts... > > >
, Node< T, StoragePolicy >
  > =
  []( Node< T, StoragePolicy >& node
    , Visitor< Graph< Return, Ts... > >& visitor ) -> Return
  {
    return visitor.visit( node );
  };

template< typename... Ts, typename Return, typename T, typename StoragePolicy >
constexpr auto invoke<
  const accept< Visitor< const Graph< Return, Ts... > > >
, const Node< T, StoragePolicy >
  > =
  []( const Node< T, StoragePolicy >& node
    , Visitor< const Graph< Return, Ts... > >& visitor) -> Return
  {
    return visitor.visit( node );
  };

template< typename Return, typename... Ts >
class Graph{
private:
  struct graph_interface
    : Interface< accept< Visitor< Graph > >, const accept< Visitor< const Graph > > >
  {  };

public:
  Return accept( Visitor< Graph >& visitor ){
    return node_.template call< ::accept< Visitor< Graph > > >(  visitor );
  };

  Return accept( Visitor< const Graph >& visitor ) const {
    return node_.template call< const ::accept< Visitor< const Graph > > >( visitor );
  };

  template< template< typename > class StoragePolicy, typename T >
  static
  std::enable_if_t< in_typelist< Graph, std::remove_reference_t<T>>::value, Graph >  
  make( T&& v ){
    return { Node< std::remove_reference_t<T>, StoragePolicy< Graph > >( std::forward<T>(v) ) };
  }
  
private:
  template< typename T, typename StoragePolicy >
  Graph( Node< T, StoragePolicy >&& node )
    : node_( std::move(node) )
  {  };

  template< typename T, typename StoragePolicy >
  Graph( const Node< T, StoragePolicy >& node )
    : node_( node )
  {  };
  
  Poly< graph_interface > node_;
};

template< size_t N, typename Graph_type >
struct Static;

template< size_t N, typename Return, typename... Ts >
struct Static<N, Graph< Return, Ts... > >{
  std::array< Graph< Return, Ts... >, N > children;
};

template< typename Graph_t >
using Terminal = Static< 0, Graph_t >;

template< typename Graph_t >
using Unary = Static< 1, Graph_t >;

template< typename Graph_t >
using Binary = Static< 2, Graph_t >;

template< typename Graph_type >
struct Dynamic;

template< typename Return, typename... Ts >
struct Dynamic<Graph< Return, Ts... > >{
  std::vector< Graph< Return, Ts... > > children;
};

template< typename Node_t, typename Return = void >
struct visit;

template< typename T, typename StoragePolicy, typename Return >
struct visit< Node< T, StoragePolicy >, Return >
  : Signature< Return ( Node< T, StoragePolicy >& ) >
{  };

template< typename T, typename StoragePolicy, typename Return >
struct visit< const Node< T, StoragePolicy >, Return >
  : Signature< Return ( const Node< T, StoragePolicy >& ) >
{  };

template< typename T, typename StoragePolicy, typename Return, typename F >
constexpr auto invoke< visit< Node< T, StoragePolicy >, Return >, F > =
  []( F& visitor, Node< T, StoragePolicy >& node  ) -> Return
  {
    return visitor( node );
  };

template< typename T, typename StoragePolicy, typename Return, typename F >
constexpr auto invoke< const visit< const Node< T, StoragePolicy >, Return >, F > =
  []( F& visitor, const Node< T, StoragePolicy >& node  ) -> Return
  {
    return visitor( node );
  };


template< typename Return, typename... Ts >
class Visitor< Graph< Return, Ts... > >{
private:
  using graph_t = Graph< Return, Ts... >;

  struct visitor_interface
    : Interface<
        visit< Node< Ts, Terminal<graph_t>>, Return >...
      , visit< Node< Ts, Unary<graph_t>>, Return >...
      , visit< Node< Ts, Binary<graph_t>>, Return >...  
      , visit< Node< Ts, Dynamic<graph_t>>, Return >...
      >
  {  };

public:
  template< typename T, typename StoragePolicy >
  Return visit( Node< T, StoragePolicy >& node ){
    return visitor_.template call< ::visit< Node< T, StoragePolicy>, Return > >( node );
  };

  template< typename F >
  Visitor( F&& f )
    : visitor_( std::forward<F>(f) )
  {  }

private:
  Poly< visitor_interface > visitor_;
};

template< typename Return, typename... Ts >
class Visitor< const Graph< Return, Ts... > >{
private:
  using graph_t = Graph< Return, Ts... >;
  
  struct visitor_interface {
    using interface =
      Interface<
        visit< const Node< Ts, Terminal<graph_t>>, Return >...
      , visit< const Node< Ts, Unary<graph_t>>, Return >...
      , visit< const Node< Ts, Binary<graph_t>>, Return >...  
      , visit< const Node< Ts, Dynamic<graph_t>>, Return >...
      >;
  };

public:
  template< typename T, typename StoragePolicy >
  Return visit( const Node< T, StoragePolicy >& node ) {
    return visitor_.template call< ::visit< const Node< T, StoragePolicy>, Return >>( node );
  };

  template< typename F >
  Visitor( F&& f )
    : visitor_( std::forward<F>(f) )
  {  }

private:
  Poly< visitor_interface > visitor_;
};


#endif // __GRAPH_HPP__
