#ifndef __MAKE_THUNK_HPP__
#define __MAKE_THUNK_HPP__

#include "../invoker.hpp"
#include "invalid.hpp"

namespace poly{

namespace impl__{
  template< typename Invoker, typename pointer_type >
  struct thunk_impl_;

  template< typename Tag, typename Return, typename... Args,
	    typename T__>
  struct thunk_impl_< Invoker< Tag, Return(Args...) >, T__* >{
    using pointer_type = T__*;
    template< typename SignatureT >
    using Transform = Erase< SignatureT, pointer_type >;

    template< typename T >
    static typename std::enable_if< !std::is_same<T, Invalid>::value,
				    Return >::type
    thunk( typename Transform<Args>::type... args ) {
      static_assert( std::is_empty< Tag >::value, "Tags may have no data members" );
      return invoke( Tag(), Transform<Args>::template reverse<T>
		     ( static_cast<typename Transform<Args>::type&&>(args) )... );
    }

    template< typename T >
    static typename std::enable_if< std::is_same<T, Invalid>::value,
				    Return >::type
    thunk( typename Transform<Args>::type... args ) {
      throw invalid_vtable_call();
    }
  };
  
  template< typename Invoker, typename pointer_type >
  struct thunk_type;

  template< typename Tag, typename Return, typename... Args, typename T__ >
  struct thunk_type< Invoker< Tag, Return(Args...) >, T__* >{
    using pointer_type = T__*; 
    template< typename SignatureT >
    using Transform = Erase< SignatureT, pointer_type >;
    using type = Return (*)( typename Transform<Args>::type... );
   
    template< typename T >
    static type get_thunk(){
      return &thunk_impl_< Invoker<Tag, Return(Args...)>, pointer_type >::template thunk<T>;
    }
  };
}

// returns a type of thunk given Invoker and Transform
template< typename S, typename pointer_type >
using thunk_type = typename impl__::thunk_type<S, pointer_type>::type;

// returns a Thunk given Invoker, Transform and a seed type T
template< typename Invoker, typename pointer_type, typename T >
constexpr thunk_type<Invoker, pointer_type > get_thunk() {
  return impl__::thunk_type<Invoker, pointer_type >::template get_thunk<T>();
}

// Helper struct signifying that an invoker cannot be called with supplied arguments
struct invalid_arguments;

namespace impl__{
  template< typename pointer_type, typename overloads >
  struct unerase_signature;

  template< typename pointer_type,
	    typename Tag, typename Return, typename... Args, typename... rest
	    >
  struct unerase_signature< pointer_type, overloads< Invoker< Tag, Return(Args...)>, rest... > >
    : unerase_signature< pointer_type, overloads< rest... > >
  {
    using unerase_signature< pointer_type, overloads< rest... > >::get;
    template< typename SignatureT >
    using Transform = Erase< SignatureT, pointer_type >;
    
    static Invoker< Tag, Return(Args...)> get( typename Transform<Args>::type... );
  };

  template< typename pointer_type >
  struct unerase_signature< pointer_type, overloads<> >
  {
    static invalid_arguments get( ... );
  };
}

// Selects Invoker unerased signature that matches erased Args given supplied Transform
template< typename Invoker, typename pointer_type, typename... Args >
using unerase_signature =
  decltype( impl__::unerase_signature< pointer_type, overloads_t<Invoker> >::get( std::declval<Args>()... ) );

}
#endif // __MAKE_THUNK_HPP__
