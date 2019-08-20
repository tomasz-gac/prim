#ifndef __VARIANT_HPP__
#define __VARIANT_HPP__

#include "../prim/value.hpp"
#include "../prim/vtable/vtable.hpp"
#include "../prim/allocator.hpp"
#include "maybe.hpp"
#include <sstream>

template <class T>
constexpr T &constexpr_max(T &a, T &b) {
    return a < b ? b : a;
}

template <class T>
constexpr T &arrayMax_impl(T *begin, T *end) {
    return begin + 1 == end
        ? *begin
        : constexpr_max(*begin, arrayMax_impl(begin + 1, end));
}

template <class T, std::size_t N>
constexpr T &arrayMax(T(&arr)[N]) {
    return arrayMax_impl(arr, arr + N);
}

template< typename T, typename... Ts >
class Variant{
private:
  template< typename Type >
  struct visit : prim::Invoker< visit<Type>, void ( prim::T&, Type& ),
				             void ( prim::T&, const Type& ) >
  {  };

  template< typename T_, typename Type >
  friend void invoke( visit<Type>, T_& visitor, Type& visited ){
    visitor( visited );
  }

  template< typename T_, typename Type >
  friend void invoke( visit<Type>, T_& visitor, const Type& visited ){
    visitor( visited );
  }

  struct IVisitor       :
    prim::Interface< visit<T>, visit<Ts>... >
  {  };

  using Visitor       = prim::reference< prim::LocalVT<IVisitor> >;

  struct accept_
    : prim::Invoker< accept_,
		     void ( prim::T&, Visitor& ),
		     void ( const prim::T&, Visitor& )>
  {  };

  template< typename T_ >
  friend void invoke( accept_, T_& visited, Visitor& visitor ){
    using visited_t = std::remove_const_t<T_>; // const T_ causes duplicate member function signatures
    prim::call<visit<visited_t>>( *visitor, visited);
 }

public:
  static constexpr bool all_copyable = 
    prim::tl::conjunction< std::is_copy_constructible<T>, std::is_copy_constructible<Ts>... >::value;
  static constexpr bool all_movable =
    prim::tl::conjunction< std::is_move_constructible<T>, std::is_move_constructible<Ts>... >::value;
  static constexpr bool all_movable_noexcept =
    prim::tl::conjunction< std::is_nothrow_move_constructible<T>, std::is_nothrow_move_constructible<Ts>... >::value;

  using IVariant_interface =
    std::conditional_t< all_copyable,
      std::conditional_t< all_movable_noexcept,
        prim::Interface< accept_, prim::copy, prim::move_noexcept, prim::storage, prim::destroy >,					    
        std::conditional_t< all_movable,
			    prim::Interface< accept_, prim::copy, prim::move, prim::storage, prim::destroy >,
			    prim::Interface< accept_, prim::copy, prim::storage, prim::destroy > > >,
			prim::Interface< accept_, prim::storage, prim::destroy > >;
  
  struct IVariant :
    IVariant_interface
  {  };

  static constexpr size_t sizes[] = { sizeof(T), sizeof(Ts)... };
  static constexpr size_t alignments[] = { alignof(T), alignof(Ts)... };
  
  static constexpr size_t size  = arrayMax(sizes);
  static constexpr size_t align = arrayMax(alignments);
  
  using variant_allocator_t = prim::StackAllocator< size, align >;
  using variant_vtable_t    = prim::JumpVT< IVariant, T, Ts... >;
  using variant_value_t     = prim::value< variant_vtable_t, variant_allocator_t >;
  
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
    prim::call<accept_>( *value_, v );
  }

  template< typename F >
  void accept( F&& f ) const {
    Visitor v{ f };
    prim::call<accept_>( *value_, v);
  }

  template< typename Return, typename F >
  Return accept( F&& f ){
    Maybe< Return > result;
    auto v = [&result, &f]( auto&& visited  ){
      result.emplace( f( std::forward<decltype(visited)>(visited) ) );
    };
    Visitor visitor{ v };
    prim::call<accept_>( *value_, visitor);
    return result.get();
  }

  template< typename Return, typename F >
  Return accept( F&& f ) const {
    Maybe< Return > result;
    auto v = [&result, &f]( auto&& visited  ){
      result.emplace( f( std::forward<decltype(visited)>(visited) ) );
    };
    Visitor visitor{ v };
    prim::call<accept_>( *value_, visitor);
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
  static_assert( std::is_nothrow_move_constructible<decltype(v)>::value );		 
  static_assert( std::is_copy_constructible<decltype(v)>::value );		 
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
