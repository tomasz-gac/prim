#ifndef __THUNK_HPP__
#define __THUNK_HPP__

#include "invoker.hpp"

namespace impl__{
  template< typename Invoker, template< typename > class Transform >
  struct thunk_impl_;

  template< typename Tag, typename Return, typename... Args,
	    template< typename > class Transform >
  struct thunk_impl_< Invoker< Tag, Return(Args...) >, Transform >{

    template< typename T >
    static Return thunk( typename Transform<Args>::type... args ) {
      static_assert( std::is_empty< Tag >::value, "Tags may have no data members" );
      return invoke( Tag(), Transform<Args>::template Reverse<T>::apply
		     ( static_cast<typename Transform<Args>::type&&>(args) )... );
    }
  };
  
  template< typename Invoker, template< typename > class Transform >
  struct thunk_type;

  template< typename Tag, typename Return, typename... Args, template< typename > class Transform >
  struct thunk_type< Invoker< Tag, Return(Args...) >, Transform >{
    using type = Return (*)( typename Transform<Args>::type... );
    
    template< typename T >
    static type get_thunk(){
      return &thunk_impl_< Invoker<Tag, Return(Args...)>, Transform >::template thunk<T>;
    }
  };
}

// returns a type of thunk given Invoker and Transform
template< typename S, template< typename > class Transform >
using thunk_type = typename impl__::thunk_type<S, Transform>::type;

// returns a Thunk given Invoker, Transform and a seed type T
template< typename Invoker, template< typename > class Transform, typename T >
thunk_type<Invoker, Transform > get_thunk(){
  return impl__::thunk_type<Invoker, Transform >::template get_thunk<T>();
}

// Helper struct signifying that an invoker cannot be called with supplied arguments
struct invalid_arguments;

namespace impl__{
  template< template< typename > class Transform, typename overloads >
  struct unerase_signature;

  template< template< typename > class Transform,
	    typename Tag, typename Return, typename... Args, typename... rest
	    >
  struct unerase_signature< Transform, overloads< Invoker< Tag, Return(Args...)>, rest... > >
    : unerase_signature< Transform, overloads< rest... > >
  {
    using unerase_signature< Transform, overloads< rest... > >::get;
  
    static Invoker< Tag, Return(Args...)> get( typename Transform<Args>::type... );
  };

  template< template< typename > class Transform >
  struct unerase_signature< Transform, overloads<> >
  {
    static invalid_arguments get( ... );
  };
}

// Selects Invoker unerased signature that matches erased Args given supplied Transform
template< typename Invoker, template< typename > class Transform, typename... Args >
using unerase_signature =
  decltype( impl__::unerase_signature< Transform, overloads_t<Invoker> >::get( std::declval<Args>()... ) );

#endif // __THUNK_HPP__
