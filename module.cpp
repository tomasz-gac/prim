#include <iostream>
#include "poly/value.hpp"
#include "poly/vtable/vtable.hpp"

#include <chrono>
#include <vector>

template< typename T, typename Children >
struct node_holder{
  T value;
  Children children;
};

template< typename T, typename C >
struct poly::wrapper_traits< node_holder<T,C> > {
  template< size_t index, typename U >
  static decltype(auto) get( U&& u ){
    return std::get<index>( std::tie( u.value, u.children ) );
  }
};

struct print : poly::declare< print, void( poly::T& ) >{  };
struct DFW : poly::declare< DFW, void( poly::T&, poly::T1& ) >{  };

template< typename T >
void invoke( print, T& v ){
  std::cout << v << std::endl;
}

template< typename T, typename U >
void invoke( DFW, T& v, U& children ){
  for( auto& c : children )
    poly::call< DFW >( *c, *c );
  invoke( print(), v );
}

struct Node : poly::Interface< print, DFW >{  };

int main()
{
  auto start = std::chrono::high_resolution_clock::now();
  using ptr_t = poly::pointer< poly::RemoteVT<Node> >;
  using holder_t = node_holder< int, std::vector< ptr_t > >;
  holder_t holder{ 0, {} };
  holder_t holder2{ 2, {} };
  holder_t holder3{ 3, {} };
  
  ptr_t ptr = &holder;
  holder.children.emplace_back( &holder2 );
  holder.children.emplace_back( &holder3 );
  poly::call<print>( *ptr );
  poly::call<DFW>( *ptr, *ptr );


  
  auto finish = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = finish - start;

  std::cout << "Passed : " << elapsed.count() << "s" << std::endl;
									 
  return 0;
}
