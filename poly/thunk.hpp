#ifndef __THUNK_HPP__
#define __THUNK_HPP__

#include "signature.hpp"

// Template variable that implements invocation of an Invoker for type T
// Requires a function call operator ( T, args... ) 
template< typename Invoker, typename T, typename = void >
static constexpr auto invoke = Invoker();

template< typename Tag, typename T, typename Return, typename... Args >
static Return thunk( erased_t<Args>... args ) {
  using resolved_invoker = resolve_invoker< Tag, Args... >;
  return invoke< resolved_invoker, T >
    ( Eraser<Args>::template unerase<T>
      ( static_cast<erased_t<Args>&&>(args) )...
      );
}
namespace impl__{
  template< typename Signature >
  struct thunk_type;

  template< typename Return, typename... Args >
  struct thunk_type< Signature< Return(Args...) > >{
    using type = Return (*)(erased_t<Args>...);
    
    template< typename Tag, typename T >
    static type get_thunk(){
      return &::thunk< Tag, T, Return, Args... >;
    }
  };
}

template< typename S >
using thunk_type = typename impl__::thunk_type<S>::type;

template< typename Tag, typename T, typename Signature >
thunk_type<Signature> get_thunk(){
  return impl__::thunk_type<Signature>::template get_thunk<Tag, T>();
}

namespace impl__{
  template< typename >
  struct unerase_signature;

  template< typename Return, typename... Args, typename... rest >
  struct unerase_signature< overloads< Signature<Return(Args...)>, rest... > >
    : unerase_signature< overloads< rest... > >
  {
    using unerase_signature< overloads< rest... > >::get;
  
    static Signature<Return(Args...)> get( erased_t< Args >... );
  };

  template<>
  struct unerase_signature< overloads<> >
  {
    struct Invalid;
    static Invalid get( ... );
  };
}

template< typename Invoker, typename... Args >
auto unerase_signature( Args&&... args ) ->
  decltype( impl__::unerase_signature< overloads_t<Invoker> >::get( std::forward<Args>(args)... ) );

#endif // __THUNK_HPP__
