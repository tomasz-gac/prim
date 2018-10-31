#ifndef __MEMORY_HPP__
#define __MEMORY_HPP__

#include <vector>
#include <cassert>

#include "../poly/value.hpp"
#include "../poly/vtable/vtable.hpp"
#include "../helpers.hpp"
#include "tracker.hpp"

namespace memory_test {

template< typename T >
struct begin : poly::Invoker< begin<T>, T* (poly::T&) >{  };
template< typename T >
struct cbegin : poly::Invoker<cbegin<T>, const T* (const poly::T&) >{  };

template< typename T >
struct end  : poly::Invoker< end<T>, T* (poly::T&) >{  };
template< typename T >
struct cend : poly::Invoker< cend<T>, const T* (const poly::T&) >{  };

template< typename T >
T* invoke( begin<T>, std::vector<T>& vec ){ return &*vec.begin(); }
template< typename T, typename U >
T* invoke( begin<T>, std::vector<U>& vec ){ throw std::runtime_error("ERROR"); }
template< typename T >
const T* invoke( cbegin<T>, const std::vector<T>& vec ){ return &*vec.cbegin(); }
template< typename T, typename U >
const T* invoke( cbegin<T>, const std::vector<U>& vec ){ throw std::runtime_error("ERROR");}

template< typename T >
T* invoke( end<T>, std::vector<T>& vec ){ return &*vec.end(); }
template< typename T, typename U >
T* invoke( end<T>, std::vector<U>& vec ){ throw std::runtime_error("ERROR"); }
template< typename T >
const T* invoke( cend<T>, const std::vector<T>& vec ){ return &*vec.cend(); }
template< typename T, typename U >
const T* invoke( cend<T>, const std::vector<U>& vec ){ throw std::runtime_error("ERROR"); }

template< typename T >
struct Iterable
  : decltype( poly::address_of() + begin<T>() + cbegin<T>() + end<T>() + cend<T>() + poly::copy() + poly::move_noexcept() + poly::destroy() + poly::storage() )
{  };

}

template< typename T, typename Value_t >
void test_values( const std::vector<T>& numbers, const Value_t& poly )
{
  using namespace memory_test;
  
  auto b = poly.template call<cbegin<T>>();
  auto e = poly.template call<cend<T>>();
  assert( numbers.size() == ( e - b ) );
  for( int i = 0; i < numbers.size(); ++i )
    assert( numbers[i].value == b[i].value );
}


template< template< typename... > class VT_t, typename Alloc >
void test_memory_vector( int n = 10000  ){
  using namespace memory_test;
  
  using Value_t = poly::Value< VT_t<Iterable<Guard<int>>>, Alloc >;
  static_assert( std::is_nothrow_move_constructible<Value_t>::value,
		 "Poly falsly assumed to be nothrow constructible" );

  Tracker tracker;
  {
    std::vector<Guard<int>> numbers;
    numbers.reserve(n);
    for( int i = 0; i < n; ++i )
      numbers.emplace_back( tracker, i );
    assert( tracker.objects.count() == n );
    
    auto p = Value_t{ in_place<decltype(numbers)>(), numbers };
    assert( tracker.objects.count() == 2*n );
    assert( tracker.copies.count() == n );
    assert( tracker.moves.count() == 0 );

    auto ap = p.template call<poly::address_of>();
    auto b = p.template call<cbegin<Guard<int>>>();
    auto e = p.template call<cend<Guard<int>>>();

    test_values( numbers, p );
  
    Value_t p2 = std::move(p);
    assert( tracker.objects.count() == 2*n );
    assert( tracker.copies.count() == n );
    assert( tracker.moves.count() == 0 );
    auto ap2 = p2.template call<poly::address_of>();
    auto b2 = p2.template call<cbegin<Guard<int>>>();
    auto e2 = p2.template call<cend<Guard<int>>>();
    assert( b == b2 && e == e2 );
    assert( ap != ap2 );
    test_values( numbers, p2 );

    auto nb2 = p2.template call<begin<Guard<int>>>();
    auto ne2 = p2.template call<end<Guard<int>>>();
    auto nb = numbers.begin();
    std::move( nb2, ne2, nb );
    assert( tracker.objects.count() == 2*n );
    assert( tracker.copies.count() == n );
    assert( tracker.moves.count() == n );
  }
  assert( tracker.objects.count() == 0 );
  assert( tracker.copies.count() == n );
  assert( tracker.moves.count() == n );
}

namespace memory_test {

template< typename... Ts >
struct convertible_to
  : poly::Interface< poly::copy, poly::move_noexcept, poly::destroy, poly::storage, as<Ts> ... >
{  };

}

template< template< typename > class VT_t, typename Alloc >
void test_memory_vector_poly( int n = 1000 ){
  using namespace memory_test;

  Tracker tracker;
  using convertible = convertible_to< int, float, double, bool >;
  using Value_t = poly::Value< VT_t< convertible>, Alloc >;
  static_assert( std::is_nothrow_move_constructible<Value_t>::value,
		 "Poly falsly assumed to be nothrow constructible" );
  {
    std::vector< Value_t > p, p_move;
    p.reserve(n);
    for( int i = 0; i < n; ++i )
      p.emplace_back( in_place<Guard<int>>(), tracker, 1 );
    assert( tracker.objects.count() == n );
    assert( tracker.copies.count() == 0 );
    assert( tracker.moves.count() == 0 );

    p_move.resize( p.size(), Value_t( in_place<Guard<int>>(), tracker, 0 ) );
    assert( tracker.objects.count() == (p_move.size()+p.size()) );
    assert( tracker.copies.count() == n );
    assert( tracker.moves.count() == 0 );
    tracker.copies.reset();
    
    std::move( p_move.begin(), p_move.end(), p.begin() );
    assert( tracker.objects.count() == (p_move.size()+p.size()) );
    assert( tracker.copies.count() == 0 );
    assert( tracker.moves.count() == n );

    tracker.copies.reset();
    tracker.moves.reset();
    p.clear();
    assert( tracker.objects.count() == p_move.size() );     
    assert( tracker.copies.count() == 0 );
    assert( tracker.moves.count() == 0 );
    p = p_move;
    assert( tracker.objects.count() == 2*n );     
    assert( tracker.copies.count() == n );
    assert( tracker.moves.count() == 0 );
  }
  assert( tracker.objects.count() == 0 );
}

namespace memory_test{

struct Storable
  : decltype( poly::copy() + poly::move() + poly::destroy() + poly::storage() )
{  };

struct A{
  A() = default;
  A( A&& ) { throw std::logic_error("A move constructor throw");  }
  A( const A& ){  }
};

}


template< template< typename > class VT_t, typename Alloc >
void test_memory_invalid()
{
  using namespace memory_test;

  Tracker tracker;
  using Value_t = poly::Value< VT_t<Storable>, Alloc >;
  {
    std::cout << std::boolalpha;
    Value_t t{ in_place<Guard<A>>(), tracker };
    Value_t t2{ in_place<Guard<A>>(), tracker};
    try{
      t = std::move(t2);
    } catch (std::logic_error e){    }
    assert( t.valueless_by_exception() );
    bool thrown = false;
    try{
      t.template call<poly::storage>();
    } catch ( const poly::invalid_vtable_call& e ){
      thrown = true;
    }
    assert( thrown );
    static_assert( !std::is_nothrow_move_constructible<decltype(t)>::value,
    		   "Poly falsly assumed to be not nothrow copy constructible" );
    static_assert( !std::is_nothrow_copy_constructible<decltype(t)>::value,
    		   "Poly falsly assumed to be not nothrow copy constructible" );

    Value_t t3{ t };
    assert( t3.valueless_by_exception() );
    Value_t t4{ std::move(t3) };
    assert( t4.valueless_by_exception() );
  }
  assert( tracker.objects.count() == 0 );
}

template< typename... Args >
using test_signature = void (*)( Args... );

template< typename T >
using stack_alloc = poly::StackAllocator< sizeof(T), alignof(T) >;

template< typename I >
using test1_JVT = poly::JumpVT< I, std::vector<Guard<float>>, std::vector<Guard<int>> >;

template< typename I >
using test2_JVT = poly::JumpVT< I, Guard<float>, Guard<int>, Guard<bool> >;

template< typename I >
using test3_JVT = poly::JumpVT< I, Guard<int>, Guard<float>, Guard<memory_test::A> >;

void test_memory()
{
  using namespace poly;
  
  using tested_t = std::vector<Guard<int>>;
  test_memory_vector< LocalVT, HeapAllocator >();
  test_memory_vector< RemoteVT, HeapAllocator >();
  test_memory_vector< test1_JVT, HeapAllocator >();
  test_memory_vector< LocalVT, stack_alloc<tested_t> > ();
  test_memory_vector< RemoteVT, stack_alloc<tested_t> >();
  test_memory_vector< test1_JVT, stack_alloc<tested_t> >();
  
  using tested_t2 = Guard<int>;
  using stack_alloc2 = StackAllocator< sizeof(tested_t2), alignof(tested_t2) >;
  test_memory_vector_poly< LocalVT, HeapAllocator >();
  test_memory_vector_poly< RemoteVT, HeapAllocator >();
  test_memory_vector_poly< test2_JVT, HeapAllocator >();
  test_memory_vector_poly< LocalVT, stack_alloc2 >();
  test_memory_vector_poly< RemoteVT, stack_alloc2 >();
  test_memory_vector_poly< test2_JVT, stack_alloc2 >();

  using tested_t3 = Guard<memory_test::A>;
  using stack_alloc3 = StackAllocator< sizeof(tested_t3), alignof(tested_t3) >;
  test_memory_invalid< LocalVT, HeapAllocator >();
  test_memory_invalid< RemoteVT, HeapAllocator >();
  test_memory_invalid< LocalVT, stack_alloc3 >();
  test_memory_invalid< RemoteVT, stack_alloc3 >();
}

#endif //__MEMEORY_HPP__
