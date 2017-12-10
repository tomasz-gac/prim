#include <iostream>
#include <typeinfo>
#include <cassert>
#include <cstddef>
#include "poly/builtins.hpp"
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

struct HeapAllocator{
  void* allocate( storage_info storage ){
    auto buffer = std::malloc( storage.size );
    if( buffer == nullptr ) throw std::bad_alloc{};
    return buffer;
  }

  void deallocate( void* data ){ 
    std::free( data );
  }
};
template< size_t Size, size_t Alignment = alignof(std::max_align_t) >
class StackAllocator{
public:
  static constexpr size_t size = Size;
  static constexpr size_t alignment = Alignment;
private:
  std::aligned_storage_t< size, alignment > buffer_;
public:
  void* allocate( storage_info storage ){
    assert( storage.size <= size && storage.alignment <= alignment );
    return reinterpret_cast<void*>(&buffer_);
  }

  void deallocate( void* data ){
    assert( data == reinterpret_cast<void*>(&buffer_) );
    //failure means that data was not allocated using this allocator
  };
};

template< typename T >
struct in_place{  };

template< typename Implementation, typename Allocator = HeapAllocator >
class Poly
  : private Allocator
  , private View<Implementation>
{
public:
  using implementation = impl_t<Implementation>;
  using interface = interface_t<implementation>;
private:
  using View_t = View<Implementation>;
  
  template< typename Invoker >
  using enable_if_supports = std::enable_if_t< supports<interface, Invoker>() >;

  template< typename F >
  decltype(auto) allocator_initialize( storage_info info, F f )
  {
    auto ptr = this->allocate( info );
    try{
      return f(ptr);
    } catch(...){
      this->deallocate( ptr );
      throw;
    }
  }
    
public:
  using View<Implementation>::operator[];
  using View<Implementation>::get;
  using View<Implementation>::call;
  
  template< typename T, typename... Args >
  Poly( in_place<T>, Args&&... args )
    : Allocator()
    , View_t( allocator_initialize( storage_info::get<T>(),
				    [&args...]( void* ptr ){
				      new (ptr) T(std::forward<Args>(args)...);
				      return View_t{ *reinterpret_cast<T*>(ptr) };
				    } ))
  {  }
    

  Poly( const Poly& other )
    : Allocator()
    , View_t( allocator_initialize( other.template call< storage >(),
				    [&other]( void* ptr ){
				      other.template call< copy >( ptr );
				      return ptr;
				    } ), other.vtable_ )
  {  }

  Poly( Poly&& other )
    : Allocator()
    , View_t( allocator_initialize( other.template call< storage >(),
				    [&other]( void* ptr ){
				      other.template call< move >( ptr );
				      return ptr;
				    } ), other.vtable_ )
  {  }
  
  ~Poly(){
    this->template call< destroy >();
    static_cast<Allocator&>(*this).deallocate( this->data_.data );
  }
};

struct A{ ~A(){ std::cout << "~A():" << this << std::endl; } };

void invoke( print, const A& a ){
  std::cout << "A:" << &a  << std::endl;
};


int main()
{
  printable p; 
  std::cout << std::boolalpha;

  float f = 1.11;
  float f2 = 2.22;
  struct printfl : decltype( print() + as<float>() + storage() + copy() ) {};
  struct pnoint : decltype(printable() - as<int&>()) {};
  using vtbl = decltype(joinVT( localVT( printfl() ) ,remoteVT( pnoint() ) ) );
  
  View< vtbl > fff = f;
  fff = f2;
  
  auto storage = fff[ ::storage() ]();
  assert( storage.size == sizeof(f) );
  assert( storage.alignment == alignof(f) );
  

  fff[print()]();
  {
    using p = decltype( print() + destroy() + move() + copy() + ::storage() );
    Poly< RemoteVT< p > > Int{ in_place<int>(), 33};
    Int.call<print>();
    Poly< RemoteVT< p >, StackAllocator<1> > Ant{in_place<A>()};
    Ant.call<print>();
    Poly< RemoteVT< p >, StackAllocator<1> > Ant2 = Ant;
    Ant2.call<print>();
    Poly< RemoteVT< p >, StackAllocator<1> > Ant3 = std::move(Ant);
    Ant3.call<print>();    
    
    std::cout << "end" << std::endl;
  }
  
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
