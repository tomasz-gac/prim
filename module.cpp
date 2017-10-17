#include <iostream>
#include "poly/poly.hpp"
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

  // operator       T&()       { return value; }
  // operator const T&() const { return value; }
  
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

struct print : Signature< void( std::ostream& ) >{};
struct test : Signature< void( std::ostream& ) >{};

struct printable : Interface< const print, const test >{};

template< typename T >
constexpr auto invoke< const print, T > =
  []( const T& v, std::ostream& str ){ print_impl(v, str); };

template< typename T >
constexpr auto invoke< const test, T > =
  []( const T& v, std::ostream& str ){ str << "test" << std::endl; };


template< typename T >
void print_impl( const T& v, std::ostream& str ){ str << v << std::endl; }

template< typename T, typename StoragePolicy >
void print_impl( const Node<T, StoragePolicy>& node, std::ostream& str )
{
  print_impl( node.value, str );
  for( const auto& child : node.children )
    print_impl( child, str );
};

template< typename StoragePolicy >
void print_impl( const Node<Recursion, StoragePolicy>& node, std::ostream& str )
{
  for( const auto& child : node.children )
    print_impl( child, str );
};


template< typename Interface >
void print_impl( const Graph< Interface >& graph, std::ostream& str ){
  graph.template call<print>(str);
}

template< typename T, T v >
struct print_v;

int main()
{
  // using graph = Graph< void, int, float, bool >;
  // using visitor = Visitor< graph >;
  // auto g = graph::make<Terminal>(int(3));
  // visitor v = []( auto& v){ std::cout <<  v.value << std::endl;  };
  // g.accept(v);

  // print_v< bool,
  // 	   std::is_base_of< impl__::IHolder< const print, overloads<> >, Holder< int, printable::interface > >::value > s;
\
  auto node = Graph< printable >::make<Terminal>( int(3) );
  auto rec = Graph< printable >::make<Unary>( Recursion(), std::move(node) );
  node.call<print>( std::cout );
  node.call<test>( std::cout );
  
  return 0;
}


