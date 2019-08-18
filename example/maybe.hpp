#ifndef __MAYBE_HPP__
#define __MAYBE_HPP__

#include <exception>

#include "../prim/value.hpp"
#include "../prim/vtable/vtable.hpp"
#include "../prim/allocator.hpp"

namespace impl__{ struct Empty_maybe{  }; }

class empty_maybe_access :
  public std::exception
{
public:
  virtual const char* what() const noexcept override
  { return "Attempted access to empty Maybe"; }
};

template< typename T >
using Maybe_ref_ =
  std::conditional_t<
    std::is_reference<T>::value,
    std::reference_wrapper< std::decay_t<T> >,
    std::decay_t<T>
  >;

template< typename T__, typename Alloc = prim::StackAllocator< sizeof(Maybe_ref_<T__>), alignof(Maybe_ref_<T__>) > >
class Maybe{
private:
  using Empty = impl__::Empty_maybe;
  using T = Maybe_ref_< T__ >;

  struct Get :
    prim::Invoker< Get,
		   T& (prim::T& ),
		   const T& (const prim::T&) >
  {  };

  friend T& invoke( Get, T& value ){ return value; }
  friend T& invoke( Get, Empty& )  { throw empty_maybe_access(); }
  template< typename U >
  friend T& invoke( Get, U&& v ){ throw std::logic_error(); }
  
  struct Interface
    : prim::Interface< Get, prim::copy, prim::move, prim::destroy, prim::storage >
  {  };
  
  using value_t = prim::value< prim::JumpVT< Interface, T, Empty >, Alloc >;
public:
  Maybe()
    : value_{ in_place<Empty>() }{  }
  Maybe( const T& value )
    : value_( in_place<T>(), value ){  }
  Maybe( T&& value )
    : value_( in_place<T>(), std::move(value) )
  {  }

  const T& get() const { return prim::call<Get>( *value_ ); }
        T& get()       { return prim::call<Get>( *value_ ); }

  template< typename... Args >
  void emplace( Args&&... args ){ value_.template emplace<T>( std::forward<Args>(args)... ); }
  void reset(){ *this = {}; }
	    
  bool empty(){ return value_.vtable().index() == 1; }

public:
  value_t value_;
};

void test_maybe(){
  Maybe<int> mi = 3;
  Maybe<int> mn;
  mn = mi;
  assert( mn.get() == mi.get() );
  mn.get() = 2;
  assert( !mn.empty() );
  assert( mn.get() == 2 );
  mn = {};
  try{
    std::cout << mn.get() << std::endl;
  } catch ( const empty_maybe_access& a ) {
  }
  assert( mn.empty() );
}

#endif //__MAYBE_HPP__
