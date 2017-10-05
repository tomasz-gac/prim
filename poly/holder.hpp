#ifndef __HOLDER_HPP__
#define __HOLDER_HPP__

#include "signature.hpp"
#include "typelist.hpp"

// Template variable that implements invocation of an Invoker for type T
// Requires a function call operator ( T, args... ) 
template< typename Invoker, typename T >
static constexpr auto invoke = Invoker();

template< typename... >
struct Interface;

template< typename Invoker, typename Interface >
struct implements
  :  disjunction<   in_typelist< ::Interface, Interface, std::remove_const_t<Invoker> >
		  , in_typelist< ::Interface, Interface, std::add_const_t<Invoker> > >
{  };

// Abstract base class for data holders of poly
template< typename Interface >
class IHolder;

// Concrete data holder of poly
template< typename T, typename Interface >
class Holder;


// IHolder and Holder implementations
namespace impl__{



  // Defines an interface for call
  template< typename Invoker, typename Signature = signature_t< Invoker > >
  class IHolder;

  // non-const invoker specialization with non-const call method
  template< typename Invoker, typename Return, typename... Args >
  class IHolder< Invoker, Signature< Return(Args...) > >
  {
  public:
    virtual Return  call( Args... args ) = 0;
    virtual ~IHolder() = default;
  };
  // Const invoker specialization with const call method
  template< typename Invoker, typename Return, typename... Args >
  class IHolder< const Invoker, Signature< Return(Args...) > >
  {
  public:
    virtual Return  call( Args... args ) const = 0;
    virtual ~IHolder() = default;
  };


  // Helper class that returns a base class of a concrete Holder
  template< typename >
  struct Holder_base;

  template< typename T, typename Interface >
  struct Holder_base< ::Holder< T, Interface > >
  { using type = ::IHolder< Interface >; };

  template< typename T >
  using Holder_base_t = typename Holder_base<T>::type;

  
  // Helper function that calls invoke on a given concrete holder
  // non-const version
  template< typename Invoker, typename T, typename Interface, typename... Args >
  return_t<Invoker>
  call_impl( ::Holder< T, Interface >& holder, Args&&... args ){
    return invoke< Invoker, std::remove_const_t<T> >( holder.value, std::forward<Args>(args)... );
  }

  // const version
  template< typename Invoker, typename T, typename Interface, typename... Args >
  return_t<Invoker>
  call_impl( const ::Holder< T, Interface >& holder, Args&&... args ){
    return invoke< Invoker, std::add_const_t<T> >( holder.value, std::forward<Args>(args)... );
  }

  // Concrete holder implementation
  // Sig argument is a signature of a given invoker, or a dummy value for empty interface
  template< typename Holder_t, typename Interface
	    , typename Sig = signature_t< head_t< ::Interface, Interface, Signature<void()> > >
  > class Holder;

  // Empty interface that terminates inheritance recursion
  // and inherits from IHolder to join the interface with its implementation
  template< typename Holder_t, typename Sig >
  class Holder< Holder_t, Interface< >, Sig >
    : public Holder_base_t< Holder_t >
  {  };
  

  // Uses ::Interface as typelist and implements call for its head
  // Derives from Holder without ::Interface's head
  // Recursiont terminates on empty ::Interface
  // non-const invoker call implementation
  template< typename Holder_t, typename Invoker, typename... Invokers,
	    typename Return, typename... Args >
  class Holder< Holder_t, Interface<       Invoker, Invokers... >, Signature< Return(Args...) > >
    : public Holder< Holder_t, Interface< /* consumes Invoker */ Invokers... > >
  {
  public:
    virtual Return call( Args... args ) override {
      auto& holder = static_cast< Holder_t& >(*this); // CRTP
      return call_impl<Invoker>( holder, args... );
    }
    virtual ~Holder() = default;
  };
  
  // const invoker call implementation
  template< typename Holder_t, typename Invoker, typename... Invokers,
	    typename Return, typename... Args >
  class Holder< Holder_t, Interface< const Invoker, Invokers... >, Signature< Return(Args...) > >
    : public Holder< Holder_t, Interface< Invokers... > >  
  {
  public:
    virtual Return call( Args... args ) const override {
      auto& holder = static_cast< const Holder_t& >(*this); // CRTP
      return call_impl<Invoker>( holder, args... );
    }
    virtual ~Holder() = default;
  };

}

// Function that calls the call method if IHolder of an appropriate base
// Base is selected depending on Invoker type
template< typename Invoker, typename... Invokers, typename... Args >
return_t<Invoker> call( IHolder< Interface< Invokers... > >& iholder, Args&&... args )
{
  using IHolder_t =       impl__::IHolder<        Invoker >;
  auto& impl = static_cast< IHolder_t& >(iholder); // disambiguation
  return impl.call( std::forward<Args>(args)... ); //call
};

template< typename Invoker, typename... Invokers, typename... Args >
return_t<Invoker> call( const IHolder< Interface< Invokers... > >& iholder, Args&&... args )
{
  using IHolder_t = const impl__::IHolder< const Invoker >;
  auto& impl = static_cast< IHolder_t& >(iholder); // disambiguation
  return impl.call( std::forward<Args>(args)... ); //call
};

// Base class of data holders of poly
// Defines interface of call methods and
// implements a call method that serves as disambiguation
template< typename... Invokers >
class IHolder< Interface< Invokers... > >
  : public impl__::IHolder< Invokers >...
{
 public:
  virtual ~IHolder() = default;
};

// Concrete data holder of poly
// Implements the interface of IHolder< Interface >
template< typename T, typename Interface >
class Holder
  : public impl__::Holder< Holder< T, Interface >, Interface >
{
public:
  template< typename... Ts >
  Holder( Ts&&... vs )
    : value( std::forward<Ts>(vs)... )
  {  }
  virtual ~Holder() = default;

  T value;
};

#endif // __HOLDER_HPP__
