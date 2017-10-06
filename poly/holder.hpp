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
  template< typename Holder_t, typename Invoker
	    , typename Sig = signature_t< Invoker >
  > class Holder;

  // non-const invoker call implementation
  template< typename Holder_t, typename Invoker, typename Return, typename... Args >
  class Holder< Holder_t, Invoker, Signature< Return(Args...) > >
    : public ::IHolder< Invoker >
  {
  public:
    virtual Return call( Args... args ) override {
      auto& holder = static_cast< Holder_t& >(*this); // CRTP
      return call_impl<Invoker>( holder, args... );
    }
    virtual ~Holder() = default;
  };

  // const invoker call implementation
  template< typename Holder_t, typename Invoker, typename Return, typename... Args >
  class Holder< Holder_t, const Invoker, Signature< Return(Args...) > >
    : public ::IHolder< Invoker >
  {
  public:
    virtual Return call( Args... args ) override {
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
template< typename Invoker >
class IHolder
  : public impl__::IHolder< Invoker >
{
 public:
  virtual ~IHolder() = default;
};

template< typename Interface >
class Holder_common;

template< typename... Invokers >
class Holder_common< Interface< Invokers... > >
{
public:
  template< typename T >
  Holder< T, Interface< Invokers... > >& as_holder(){
    return static_cast< Holder< T, Interface< Invokers... > >& >(*this);
  }

  template< typename T >
  const Holder< T, Interface< Invokers... > >& as_holder() const {
    return static_cast< const Holder< T, Interface< Invokers... > >& >(*this);
  }

  virtual ~Holder_common() = default;
};

// Concrete data holder of poly
// Implements the interface of IHolder< Interface >
template< typename T, typename... Invokers >
class Holder< T, Interface< Invokers... > >
  : public impl__::Holder< Holder< T, Interface<Invokers...> >, Invokers >...
  , public Holder_common< Interface<Invokers...> >
{
public:
  template< typename... Ts >
  Holder( Ts&&... vs )
    : value( std::forward<Ts>(vs)... )
  {  }
  virtual ~Holder() = default;

  template< typename Invoker >
        IHolder< Invoker >& as_interface()
  {  return static_cast<       IHolder< Invoker >& >(*this); }

  template< typename Invoker >
  const IHolder< Invoker >& as_interface() const
  {  return static_cast< const IHolder< Invoker >& >(*this); }

  T value;
};



#endif // __HOLDER_HPP__
