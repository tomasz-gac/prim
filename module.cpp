#include <iostream>
#include "example/graph_interface.hpp"
#include <cstddef>

struct print : Signature< void( std::ostream& ) >{};

template< typename T >
struct test_o : Signature< void( forward<T> ) >{};

template< typename... Ts >
struct overloaded_test : Overloaded< test_o<Ts>... >{};

using otest = overloaded_test< int, float, bool, std::string >;

struct printable : Interface< const otest, const print >{};

template< typename T >
constexpr auto invoke< const print, T > =
  []( const T& v, std::ostream& str ){ print_impl(v, str); };

template< typename O, typename T >
constexpr auto invoke< const test_o<O>, T > =
  []( const T& v, auto&& f ){
    std::cout << "test" << std::endl;
    std::cout << (std::is_const<std::remove_reference_t<decltype(f)>>::value ? "const " : "")
              << typeid(f).name() 
              << (std::is_rvalue_reference<decltype(f)>::value ? "&&" : "&") << std::endl;
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

template< typename >
struct IData_traits;

template< typename IData, typename T >
using Holder_t = typename IData_traits< IData >::template type<T>;

template< typename Allocator_t = void >
class polymorphic_eraser{
    
  template< typename T >
  void erase( T&& value ){
    data_ = new Holder_type<T>( std::forward<T>(value) );
  };

  template< typename T >
  T& retrieve(){
    return static_cast< Holder_type<T>& >( *data_ ).held;
  };

  template< typename T >
  T& retrieve() const {
    return static_cast< const Holder_type<T>& >( *data_ ).held;
  };

  

};



int main()
{
  auto node = Graph< printable >::make<Terminal>( int(3) );
  node.call<print>( std::cout );
  auto t = std::string("test");
  node.call<otest>( t );
  node.call<otest>( int() );  
  int i = 4;
  node.call<otest>( i );

  return 0;
}


