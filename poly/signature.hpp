#ifndef __SIGNATURE_HPP__
#define __SIGNATURE_HPP__

#include <type_traits>

// Type that encodes a signature for a given invoker
// Invokers are meant to derive from this type
template< typename >
class Signature;

template< typename Return, typename... Args >
struct Signature< Return(Args...)>{
  using return_type = Return;
  using signature_type = Signature;
};

template< typename T >
using signature_t = typename T::signature_type;

template< typename T >
using return_t = typename T::return_type;

template< typename T >
struct is_signature
  : std::false_type
{  };

template< typename Return, typename... Args >
struct is_signature< Signature< Return(Args...) > >
  : std::true_type
{  };

namespace impl__{

template< typename T, typename = void >
struct is_invoker
  : std::false_type
{  };

template< typename T >
struct is_invoker< T, std::conditional_t< false, signature_t<T>, void > >
  : is_signature< signature_t< T > >
{  };

}

template< typename T >
struct is_invoker
  : impl__::is_invoker< T >
{  };

#endif // __SIGNATURE_HPP__
