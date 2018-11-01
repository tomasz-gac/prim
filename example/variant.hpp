#ifndef __VARIANT_HPP__
#define __VARIANT_HPP__

#include "../poly/value.hpp"
#include "../poly/vtable/vtable.hpp"
#include "../poly/allocator.hpp"
#include "maybe.hpp"
#include <sstream>

template< typename T, T... v >
struct max;

template< typename T, T u, T u1, T... vs >
struct max< T, u, u1, vs... >
  : max< T, (u > u1 ? u : u1), vs... >
{  };

template< typename T, T u >
struct max< T, u >
  : std::integral_constant< T, u >
{  };

template< typename T, typename... Ts >
class Variant{
private:
  template< typename Type >
  struct visit : poly::Invoker< visit<Type>, void ( poly::T&, Type& ) >{  };

  template< typename T_, typename Type >
  friend void invoke( visit<Type>, T_& visitor, Type& visited ){
    visitor( visited );
  }

  struct IVisitor       :
    poly::Interface< visit<T>, visit<Ts>... >
  {  };
  struct IVisitor_const :
    poly::Interface< visit<std::add_const_t<T>>,
		     visit<std::add_const_t<Ts>>... >
  {  };

  using Visitor       = poly::View< poly::LocalVT<IVisitor> >;
  using Visitor_const = poly::View< poly::LocalVT<IVisitor_const> >;


  struct accept_
    : poly::Invoker< accept_,
		     void ( poly::T&, Visitor& ),
		     void ( const poly::T&, Visitor_const& )>
  {  };

  template< typename T_ >
  friend void invoke( accept_, T_& visited, Visitor& visitor ){ 
    visit<T_>::call( visitor, visited);
 }

  template< typename T_ >
  friend void invoke( accept_, const T_& visited, Visitor_const& visitor ){
    visit<const T_>::call( visitor, visited);
  }

private:
  struct IVariant
    : poly::Interface<
    poly::storage, poly::copy
  , poly::move, poly::destroy
  , accept_
  > // TODO : move and copy depending on type interfaces
  {  };
  
  static constexpr size_t size  = max< std::size_t, sizeof(T), sizeof(Ts)... >::value;
  static constexpr size_t align = max< std::size_t,alignof(T), alignof(Ts)... >::value;
  
  using variant_allocator_t = poly::StackAllocator< size, align >;
  using variant_vtable_t    = poly::JumpVT< IVariant, T, Ts... >;
  using variant_value_t     = poly::Value< variant_vtable_t, variant_allocator_t >;
  
public:
  template< typename U, typename... Args >
  Variant( in_place<U>, Args&&... args )
    : value_{ in_place<U>(), std::forward<Args>(args)... }
  {  }

  int index() const { return value_.vtable().index(); }

  template< typename T_, typename... Args >
  void emplace( Args&&... args ){
    value_.template emplace<T_>(std::forward<Args>(args)... );
  }

  template< typename F >
  void accept( F&& f ){
    Visitor v{ f };
    accept_::call( value_, v);
  }

  template< typename F >
  void accept( F&& f ) const {
    Visitor_const v{ f };
    accept_::call( value_, v);
  }

  template< typename Return, typename F >
  Return accept( F&& f ){
    Maybe< Return > result;
    auto v = [&result, &f]( auto&& visited  ){
      result.emplace( f( std::forward<decltype(visited)>(visited) ) );
    };
    Visitor visitor{ v };
    accept_::call( value_, visitor);
    return result.get();
  }

  template< typename Return, typename F >
  Return accept( F&& f ) const {
    Maybe< Return > result;
    auto v = [&result, &f]( auto&& visited  ){
      result.emplace( f( std::forward<decltype(visited)>(visited) ) );
    };
    Visitor_const visitor{ v };
    accept_::call( value_, visitor);
    return result.get();
  }

private:
  variant_value_t value_;
};


void test_variant()
{
  auto visitor = []( auto& v ){ };

  std::cout << std::boolalpha;

  Variant< float, bool, int, std::string > v{ in_place<float>(), 3 };
  const auto& cv = v;
  assert( v.index() == 0 );
  v.accept( visitor );
  cv.accept( visitor );
  v.emplace<bool>( true );
  assert( v.index() == 1 );
  v.accept( visitor );
  cv.accept( visitor );
  v = { in_place<int>(), 0 };
  v.emplace<int>( 0 );
  assert( v.index() == 2 );
  v.accept( visitor );
  cv.accept( visitor );
  v.emplace<std::string>( "test" );
  assert( v.index() == 3 );
  v.accept( visitor );
  cv.accept( visitor );

  int a0 = 0;
  int& aa = a0;
  auto vA = [&aa]( auto& v ) -> int&{ return aa; };

  int& a = v.accept<int&>( vA );
  assert( &a == &aa );
};

#endif //__VARIANT_HPP__
