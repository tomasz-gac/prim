#ifndef __JUMP_VTABLE_HPP__
#define __JUMP_VTABLE_HPP__

#include <cassert>

#include "thunk.hpp"

namespace poly{

namespace impl_{
  template< typename T >
  struct type_{
    using type = T;
  };
}

template< typename Interface, typename erased_t, typename... Ts >
class JumpVTable
{
public:
  using interface = interface_t<Interface>;
  using erased_type = erased_t;

  template< typename T >
  static constexpr int get_index = static_cast<int>(tl::index_of<T, tl::_<Invalid, Ts...> >::value)-1;

private:
  
  static tl::unique_typelist< Ts... > assert_unique_typelist__;
  
  template< typename T >
  JumpVTable( impl_::type_<T> )
    : index_( get_index<T> )
  {  }

public:
  template< typename T >
  static JumpVTable make(){
    static_assert( tl::unique_typelist< Ts... >::template contains<T>::value || std::is_same<T, Invalid>::value,
		   "Requested type is not supported by this JumpVTable.");
    
    return { impl_::type_<T>() };
  }

  // Get a thunk based on tag and signature
  template< typename Invoker >
  auto get() const {
    return [this]( auto&&... args ) -> decltype(auto){
      return this->jumptable<
	Invoker, tl::_<Invalid, Ts...>
	>( std::true_type(), std::forward<decltype(args)>(args)... );
    };
  }

  template< typename Invoker >
  auto operator[]( const Invoker& ) const {
    return get< Invoker >();
  }

  JumpVTable( const JumpVTable& )           = default;
  JumpVTable(       JumpVTable&& ) noexcept = default;

  JumpVTable& operator=( const JumpVTable& ) = default;
  JumpVTable& operator=(       JumpVTable&& ) noexcept = default;

  int index() const { return index_; }

  bool operator==( const JumpVTable& other ) const {
    return index() == other.index();
  }

  bool operator!=( const JumpVTable& other ) const {
    return index() != other.index();
  }
private:
  int index_;

  template< typename Invoker, typename typelist, int N = 0, typename... Args >
  decltype(auto) jumptable( std::true_type, Args&&... args ) const {
    //Matches a dynamic value of index_ to compile-time indices of Ts...
    //When indices match - calls Invoker with a specific type
    switch (index_)
      {
	using T__1 =     typename tl::head_or_type<typelist, Invalid>::type;
	using list__1 =  typename tl::head_or_type<typelist, Invalid>::tail;
      case -1+N : return call<Invoker, T__1>( std::forward<Args>(args)... );
	using T_0 =     typename tl::head_or_type<list__1, Invalid>::type;
	using list_0 =  typename tl::head_or_type<list__1, Invalid>::tail;
      case  0+N : return call<Invoker, T_0>( std::forward<Args>(args)... );
	using T_1 =     typename tl::head_or_type<list_0, Invalid>::type;
	using list_1 =  typename tl::head_or_type<list_0, Invalid>::tail;
      case  1+N : return call<Invoker, T_1>( std::forward<Args>(args)... );
	using T_2 =     typename tl::head_or_type<list_1, Invalid>::type;
	using list_2 =  typename tl::head_or_type<list_1, Invalid>::tail;
      case  2+N : return call<Invoker, T_2>( std::forward<Args>(args)... );
	using T_3 =     typename tl::head_or_type<list_2, Invalid>::type;
	using list_3 =  typename tl::head_or_type<list_2, Invalid>::tail;
      case  3+N : return call<Invoker, T_3>( std::forward<Args>(args)... );
	using T_4 =     typename tl::head_or_type<list_3, Invalid>::type;
	using list_4 =  typename tl::head_or_type<list_3, Invalid>::tail;
      case  4+N : return call<Invoker, T_4>( std::forward<Args>(args)... );
	using T_5 =     typename tl::head_or_type<list_4, Invalid>::type;
	using list_5 =  typename tl::head_or_type<list_4, Invalid>::tail;
      case  5+N : return call<Invoker, T_5>( std::forward<Args>(args)... );
	using T_6 =     typename tl::head_or_type<list_5, Invalid>::type;
	using list_6 =  typename tl::head_or_type<list_5, Invalid>::tail;
      case  6+N : return call<Invoker, T_6>( std::forward<Args>(args)... );
	using T_7 =     typename tl::head_or_type<list_6, Invalid>::type;
	using list_7 =  typename tl::head_or_type<list_6, Invalid>::tail;
      case  7+N : return call<Invoker, T_7>( std::forward<Args>(args)... );
	using T_8 =     typename tl::head_or_type<list_7, Invalid>::type;
	using list_8 =  typename tl::head_or_type<list_7, Invalid>::tail;
      case  8+N : return call<Invoker, T_8>( std::forward<Args>(args)... );
	using T_9 =     typename tl::head_or_type<list_8, Invalid>::type;
	using list_9 =  typename tl::head_or_type<list_8, Invalid>::tail;
      case  9+N : return call<Invoker, T_9>( std::forward<Args>(args)... );
      default :
	return dispatch<Invoker, list_9, 10+N>( std::integral_constant< bool, (sizeof...(Ts) > 9+N)>(),
						std::forward<Args>(args)... );
      }
  }

  template< typename Invoker, typename typelist, int N = 0, typename... Args >
  decltype(auto) dispatch ( std::false_type, Args&&... args ) const {
    assert(false);
    return call<Invoker, Invalid>( std::forward<Args>(args)... );
  }
  
  template< typename Invoker, typename T, typename... Args >
  static decltype(auto) call( Args&&... args )
  {
    using Signature = unerase_signature< Invoker, erased_type, Args&&... >;
    static_assert( !std::is_same< Signature, invalid_arguments >::value,
    		   "Invoker cannot be called with supplied arguments" );
    return (*get_thunk<Signature, erased_type, T>())( std::forward<Args>(args)... );
  }



};

}

#endif // __JUMP_VTABLE_HPP__
