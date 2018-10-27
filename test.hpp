#ifndef __TEST_VIEW_HPP__
#define __TEST_VIEW_HPP__

#include "poly/view.hpp"
#include "helpers.hpp"
#include "poly/builtins.hpp"
#include <cassert>

struct printable :
  Interface< print, assign, type_id, as<int&>, as<float&>, as<bool> >
{
  ::print print;
  ::assign assign;
  ::type_id type_id;
};

struct convertible : Interface< as<float&>, as<int&>, as<bool> >{};

void test_view(){
  printable p; 
  std::cout << std::boolalpha;

  std::cout << "test_view()" << std::endl;

  float f = 1.11;
  float f2 = 2.22;
  struct printfl : decltype( print() + as<float>() + storage() ) {};
  struct pnoint : decltype(printable() - as<int&>()) {};
  using vtbl = decltype(joinVT( localVT( printfl() ) ,remoteVT( pnoint() ) ) );
  
  View< vtbl > fff = f;
  fff = f2;
  
  auto storage = fff[ ::storage() ]();
  assert( storage.size == sizeof(f) );
  assert( storage.alignment == alignof(f) );
  

  fff[print()]();
  
  int s = 1;
  View< LocalVT< printable > > i = s;
  View< RemoteVT< printable > > ii = s;
  std::cout << sizeof(int*) << " " << sizeof( fff ) << " " << sizeof( i ) << " " << sizeof(ii) << std::endl;
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
}


#endif // __TEST_VIEW_HPP__

