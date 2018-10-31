#include <iostream>
#include "poly/value.hpp"
#include "poly/vtable/vtable.hpp"
#include "poly/allocator.hpp"

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

  struct IVisitor
    : poly::Interface< visit<T>, visit<Ts>... >
  {  };

  using Visitor       = int; //poly::View< poly::LocalVT<IVisitor> >;

  struct accept
    : poly::Invoker< accept, void ( poly::T&, Visitor& ) >
		     
  {  };

  template< typename T_ >
  friend void invoke( accept, T_& visited, Visitor& visitor ){
    std::cout << visitor << std::endl;
    // visitor.template call<visit<T_>>();
  }

private:
  struct IVariant
    : poly::Interface<
    poly::storage, poly::copy
  , poly::move, poly::destroy
  , accept
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

  Variant( const Variant& )  = default;
  Variant(       Variant&& )
    noexcept(std::is_nothrow_move_constructible<variant_value_t>::value) = default;

  Variant& operator=( const Variant&  ) = default;
  Variant& operator=(       Variant&& )
    noexcept(std::is_nothrow_move_assignable<variant_value_t>::value) = default;

  int index() const { return value_.vtable().index(); }

  template< typename F >
  void operator()( F&& f ){
    //    Visitor v{ f };
    value_.template call<accept>(f);
  }

  template< typename F >
  void operator()( F&& f ) const {
    //    Visitor v{ f };
    value_.template call<accept>(f);
  }
private:
  variant_value_t value_;
};



int main()
{
  //  auto visitor = []( auto& v ){ std::cout << v << std::endl; };
  int visitor = 1;
  
  Variant< float, bool, int, std::string > v{ in_place<float>(), 3 };
  assert( v.index() == 0 );
  v( visitor );
  v = Variant< float, bool, int, std::string >{ in_place<bool>(), true };
  assert( v.index() == 1 );
  v( visitor );
  v = Variant< float, bool, int, std::string >{ in_place<int>(), 0 };
  assert( v.index() == 2 );
  v( visitor );
  v = Variant< float, bool, int, std::string >{ in_place<std::string>(), "test" };
  assert( v.index() == 3 );
  v( visitor );
  
  // test_memory();
  // test_JumpVTable();
  std::cout << "passed" << std::endl;
									 
  return 0;
}
