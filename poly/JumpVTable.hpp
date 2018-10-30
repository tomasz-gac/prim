#ifndef __JUMP_VTABLE_HPP__
#define __JUMP_VTABLE_HPP__

#include "typelist.hpp"

namespace impl_{
  template< typename T >
  struct type_wrapper{
    using type = T;
  };
}

struct Invalid;

template< template< int > class Invoke, int N = 0, typename... Args >
decltype(auto) call_version( int index, Args&&... args ){
  switch (index)
  {
    case -1+N : return Invoke<-1+N>::apply( std::forward<Args>(args)... );
    case   +N : return Invoke<  +N>::apply( std::forward<Args>(args)... );
    case  1+N : return Invoke< 1+N>::apply( std::forward<Args>(args)... );
    case  2+N : return Invoke< 2+N>::apply( std::forward<Args>(args)... );
    case  3+N : return Invoke< 3+N>::apply( std::forward<Args>(args)... );
    case  4+N : return Invoke< 4+N>::apply( std::forward<Args>(args)... );
    case  5+N : return Invoke< 5+N>::apply( std::forward<Args>(args)... );
    case  6+N : return Invoke< 6+N>::apply( std::forward<Args>(args)... );
    case  7+N : return Invoke< 7+N>::apply( std::forward<Args>(args)... );
    case  8+N : return Invoke< 8+N>::apply( std::forward<Args>(args)... );
    case  9+N : return Invoke< 9+N>::apply( std::forward<Args>(args)... );
    default   : return call_version<Invoke, 10+N>( std::forward<Args>(args)... );
  }
}

template< int index, typename JumpVT, typename Invoker >
struct Jump_invoke;

// VTable that holds thunks locally
template< typename Interface, template< typename > class Transform, typename... Ts >
class JumpVTable
  : unique_typelist< Ts... >
{
public:
  using interface = interface_t<Interface>;

  template< typename T >
  using get_index = index_of<T, _<Ts...> >;
  

private:
 
  JumpVTable( int index )
    : index_(index)
  {  }

public:
  template< typename T >
  static JumpVTable make(){
    if( std::is_same<T,Invalid>::value ) return { -1 };
    return { get_index<T>::value };
  }

  // Get a thunk based on tag and signature
  template< typename Invoker >
  auto get() const {
    template< int index >
    using invoke_ = Jump_invoke< index, JumpVTable, Invoker>;

    return []( auto Args&&... args ){
      return call_version< index >( index_ );
    }
  }

  template< typename Invoker >
  auto operator[]( const Invoker& ) const {
    return get< Tag >();
  }

public:
  const int index_;
};

template< int index, template< typename > class Transform, typename Interface, typename... Ts, typename Invoker >
struct Jump_invoke< index, JumpVTable<Interface, Transform, Ts... >, Invoker >{
private:
  using type = typename std::tuple_element< index, std::tuple< impl_::type_wrapper<Ts>... > >::type::type;

public:
  template< typename... Args >
  static decltype(auto) apply( Args&&... args ) const {
    using Signature = unerase_signature< Invoker, Transform, Args&&... >;
    static_assert( !std::is_same< Signature, invalid_arguments >::value,
		   "Invoker cannot be called with supplied arguments" );
    return (*get_thunk<Signature, Transform, T>())( std::forward<Args>(args)... );
  }
};

template< int index, typename Interface, typename... Ts, typename Invoker >
struct Jump_invoke< -1, JumpVTable<Interface, Ts... >, Invoker >{
  
  template< typename... Args >
  static decltype(auto) apply( Args&&... args ) const {
    using Signature = unerase_signature< Invoker, Transform, Args&&... >;
    static_assert( !std::is_same< Signature, invalid_arguments >::value,
		   "Invoker cannot be called with supplied arguments" );
    return (*get_thunk<Signature, Transform, Invalid>())( std::forward<Args>(args)... );
  }
};

#endif // __JUMP_VTABLE_HPP__
