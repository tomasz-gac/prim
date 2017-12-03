#include <iostream>
#include <typeinfo>
#include <cassert>
#include "poly/view.hpp"
#include "poly/vtable.hpp"
#include "helpers.hpp"
#include "poly/builtins.hpp"

struct printable :
  Interface< print, assign, type_id, as<int&>, as<float&>, as<bool> >
{
  ::print print;
  ::assign assign;
  ::type_id type_id;
};

struct convertible : Interface< as<float&>, as<int&>, as<bool> >{};

template< typename ... >
struct print_ts;

int main()
{
  printable p; 
  std::cout << std::boolalpha;
  
  float f = 1.11;
  struct printfl : decltype( print() + as<float>() + storage() + copy() ) {};
  struct pnoint : decltype(printable() - as<int&>()) {};
  using vtbl = decltype(joinVT( localVT( printfl() ) ,remoteVT( pnoint() ) ) );

  View< vtbl > fff = f;
  
  auto storage = fff[ ::storage() ]();
  assert( storage.size == sizeof(f) );
  assert( storage.alignment == alignof(f) );

  fff[print()]();
  void* ptr = std::malloc( sizeof(f) );
  fff[ copy() ]( ptr );
  std::cout << *reinterpret_cast<float*>(ptr) << std::endl;
  std::free(ptr);

  int s = 1;
  View< LocalVT< printable > > i = s;
  View< RemoteVT< printable > > ii = s;
  std::cout << sizeof( fff ) << " " << sizeof( i ) << " " << sizeof(ii) << std::endl;
  View< LocalVT< print > > k(i);
  View< LocalVT< convertible > > c(i);
  const auto& ci = i;
  k[ p.print ]();
  c[ as<int&>() ]() = 5;
  try{
    c[ as<float&>() ]() = 3.14;
  } catch ( std::bad_cast e ){    
    std::cout << "Exception : " << e.what() << std::endl;
  };
  std::cout << c[ as<bool>() ]() << std::endl;
  int s2 = 3;
  View< RemoteVT<printable> > i2 = s2;
  i[ p.assign ]( i2 );
  k[ p.print ](); 
  std::cout << i[ p.type_id ]().name() << std::endl;
  i[ p.print ]();
  ci[ p.print ]();
  
  return 0;
}
