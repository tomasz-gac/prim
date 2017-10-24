#ifndef __GRAPH_INTERFACE_HPP__
#define __GRAPH_INTERFACE_HPP__

#include "../poly/poly.hpp"

#include <array>
#include <vector>

template< typename Interface >
class Graph;

template< typename T, typename StoragePolicy >
struct Node : StoragePolicy
{
  template< typename U, typename... Ts >
  Node( U&& v, Ts&&... vs )
    : StoragePolicy( std::forward<Ts>(vs)... )
    , value( std::forward<U>(v) )
  {  }

  T value;  
};

template< typename Interface >
class Graph : public Poly< Interface >
{
public:
  template< template< typename > class StoragePolicy, typename T, typename... Ts >
  static
  Graph make( T&& v, Ts&&... vs )
  {
    return { Node< T, StoragePolicy<Graph>>{ std::forward<T>(v), std::forward<Ts>(vs)...  } };
  }
private:
  using Poly<Interface>::Poly;
};

template< size_t N, typename Graph_t >
struct Static;

template< size_t N, typename Interface >
struct Static< N, Graph<Interface> >{
  template< typename... Ts >
  Static( Ts&&... vs)
    : children{ std::forward<Ts>(vs)... }
  {  };
  
  std::array< Graph<Interface>, N > children;
};

template< typename Graph_t >
using Terminal = Static<0, Graph_t>;

template< typename Graph_t >
using Unary = Static<1, Graph_t>;

template< typename Graph_t >
using Binary = Static<2, Graph_t>;

template< typename Graph_t >
struct Dynamic;

template< typename Interface >
struct Dynamic< Graph<Interface> >{
  template< typename... Ts >
  Dynamic( Ts&&... vs)
    : children( std::forward<Ts>(vs)... )
  {  };
  
  std::vector< Graph<Interface> > children;
};

struct Recursion{};

#endif // __GRAPH_INTERFACE_HPP__
