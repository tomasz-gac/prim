#include <iostream>
#include "graph_interface.hpp"

struct print : Signature< void( std::ostream& ) >{};
struct test : Signature< void( forward<int> ) >{};

struct printable : Interface< const test, const print >{};

template< typename T >
constexpr auto invoke< const print, T > =
  []( const T& v, std::ostream& str ){ print_impl(v, str); };

template< typename T >
constexpr auto invoke< const test, T > =
  []( const T& v, auto&& f ){
    std::cout << "test" << std::endl;
    std::cout << (std::is_rvalue_reference<decltype(f)>::value ? "rv" : "lv") << std::endl;
  };


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


int main()
{
  auto node = Graph< printable >::make<Terminal>( int(3) );
  node.call<print>( std::cout );
  node.call<test>( 3 );
  int i = 4;
  node.call<test>( i );
  
  return 0;
}


