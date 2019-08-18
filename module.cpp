#include <iostream>
#include "prim/value.hpp"
#include "prim/allocator.hpp"
#include "prim/vtable/vtable.hpp"
#include "helpers.hpp"

#include <chrono>
#include <vector>

template< typename T, typename Children >
struct node_holder{
  T value;
  Children children;

  node_holder( T v, Children c = Children() )
    : value(v), children( std::move(c) )
  {  }

  ~node_holder(){ std::cout << "~node_holder : " << this << std::endl; }
};

template< typename T, typename Children >
std::ostream& operator<<( std::ostream& str, const node_holder<T,Children>& h ){
  str << h.value << ", children: " << h.children.size();
  return str;
}

template< typename T, typename C >
struct prim::wrapper_traits< node_holder<T,C> > {
  template< size_t index, typename U >
  static decltype(auto) get( U&& u ){
    return std::get<index>( std::tie( u, u.value, u.children ) );
  }
};

//struct print : prim::declare< print, void( prim::T& ) >{  };
struct DFW : prim::declare< DFW, void( prim::T1&, prim::T2& ) >{  };

template< typename... Ts >
struct print_ts;

template< typename T >
void invoke( print, T* v ){
  if( v == nullptr ){    
    std::cout << "nullptr" << std::endl;
  } else {
    print_type(*v);
  }
}

template< typename T, typename U >
void invoke( DFW, T& v, U& children ){
  for( auto& c : children )
    prim::call< DFW >( *c, *c );
  invoke( print(), v );
}

struct Node : prim::Interface< print, DFW, prim::copy, prim::move, prim::destroy, prim::storage >{  };

int main()
{
  auto start = std::chrono::high_resolution_clock::now();
  using ptr_t = prim::pointer< prim::RemoteVT<Node> >;
  using val_t = prim::value< prim::RemoteVT<Node>, prim::HeapAllocator >;
  using holder_t = node_holder< int, std::vector< val_t > >;
  {
    val_t holder{ in_place<holder_t>(), 0 };
    val_t holder2{ in_place<holder_t>(), 2 };
    std::vector<val_t> children;
    children.push_back(holder);
    children.push_back(holder2);  
    val_t holder3{ in_place<holder_t>(), 3, std::move(children) };

    prim::call<print>( *holder3 );
    prim::call<DFW>( *holder3, *holder3 );
  }
  
  std::cout << "ptr test" << std::endl;
  using ptr2_t = prim::pointer< prim::RemoteVT< print > >;
  int i = 0;
  int* pi = nullptr;
  ptr2_t p = { &pi };
  prim::call<print>( *p );
  pi = &i;
  prim::call<print>( *p );
  i = 2;
  prim::call<print>( *p );
  
  auto finish = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = finish - start;

  std::cout << "Passed : " << elapsed.count() << "s" << std::endl;
  return 0;
}
