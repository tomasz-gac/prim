#ifndef __HOLDER_HPP__
#define __HOLDER_HPP__

#include "signature.hpp"
#include "typelist.hpp"

// Template variable that implements invocation of an Invoker for type T
// Requires a function call operator ( T, args... ) 
template< typename Invoker, typename T >
static constexpr auto invoke = Invoker();

// Abstract base class for data holders of poly
template< typename Interface >
class IHolder;

// Concrete data holder of poly
template< typename T, typename Interface >
class Holder;


// IHolder and Holder implementations
namespace impl__{

  //Tag used for call disambiguation
  template< typename T >
  struct type;

  // Helper template for delayed evaluation
  template< typename T >
  struct t_{ using type = T; };

  //calls generate_overloads on Interface if it is non-empty.
  template< typename Interface >
  using interface_overloads_t = typename
    std::conditional_t< length<Interface>::value == 0,
		      t_< overloads<> >,
       		      generate_overloads< head_or_t<Interface,void> > >::type;

  // Defines an interface for call
  // First, implements all overloads for a single Invoker of a given interface
  // Then removes that Invoker from interface and continues for the next one
  // Terminates on empty Interface
  template< typename Interface, typename overloads = interface_overloads_t<Interface>>
  class IHolder;

  // non-const invoker specialization with non-const call metho
  // Consumes a single Signature<> from overloads<> typelist
  template<
      template< typename... > class typelist
    , typename Invoker, typename... Invokers
    , typename Return, typename... Args, typename... sigs >
  class IHolder<
    typelist< Invoker, Invokers...>
  , overloads< Signature< Return(Args...) >, sigs... >
    > : public IHolder< typelist< Invoker, Invokers... >, overloads< sigs... > >
  {
  public:
    // Forward the already defined call method from base class
    using IHolder< typelist< Invoker, Invokers...>, overloads< sigs... > >::call;
    
    virtual Return  call( type<Invoker>*, Args... args ) = 0;
    virtual ~IHolder() = default;
  };
  
  // Const invoker specialization with const call method
  // Consumes a single Signature<> from overloads<> typelist  
  template<
      template< typename... > class typelist
    , typename Invoker, typename... Invokers
    , typename Return, typename... Args, typename... sigs >
  class IHolder<
    typelist< const Invoker, Invokers...>
  , overloads< Signature< Return(Args...) >, sigs... >
    > : public IHolder< typelist< const Invoker, Invokers... >, overloads< sigs... > >
  {
  public:
    // Forward the already defined call method from base class    
    using IHolder< typelist< const Invoker, Invokers...>, overloads< sigs... > >::call;
    
    virtual Return  call( type<const Invoker>*, Args... args ) const = 0;
    virtual ~IHolder() = default;
  };

  // When no more overloads<> are available - consumes an Invoker
  // and continues with implementation
  template< template< typename... > class typelist, typename Invoker, typename... Invokers >
  class IHolder< typelist<Invoker, Invokers...>, overloads<  > >
    : public IHolder< typelist< Invokers... > >
  {
  public:
    // Forward the already defined call method from base class    
    using IHolder< typelist< Invokers... > >::call;
  };

  // Recursion termination. Defines a dummy call to keep consistency
  // with call forwarding
  template< template< typename... > class typelist >
  class IHolder< typelist<>, overloads<  > >
  {
  private:
    struct Invalid;
  protected:
    // Dummy call method for consistency in forwarding    
    virtual void call( type<Invalid>* ){};
  public:
    virtual ~IHolder() = default;
  };
  
  // Helper function that calls invoke on a given concrete holder
  // non-const version
  template< typename Invoker, typename T, typename Interface, typename... Args >
  return_t<Invoker>
  call( ::Holder< T, Interface >& holder, Args&&... args ){
    using Invoker_t = std::remove_const_t<Invoker>;
    using type = std::remove_const_t<T>;
    return invoke< Invoker_t, type >( holder.value, std::forward<Args>(args)... );
  }

  // const version
  template< typename Invoker, typename T, typename Interface, typename... Args >
  return_t<Invoker> 
  call( const ::Holder< T, Interface >& holder, Args&&... args ){
    using Invoker_t = std::add_const_t<Invoker>;
    using type = std::add_const_t<T>;
    return invoke< Invoker_t, type >( holder.value, std::forward<Args>(args)... );
  }

  //deduces appropriate IHolder for a given holder_t to inherit from
  template< typename holder_t >
  struct holder_interface;

  template< typename T, typename Interface >
  struct holder_interface< ::Holder< T, Interface > >{
    using type = ::IHolder< typename Interface::interface_type >;
  };

  template< typename holder_t >
  using holder_interface_t = typename holder_interface<holder_t>::type;
  
  // Concrete holder implementation
  // overloads argument is a set of overloaded Signatures for perfect forwarding of a given invoker
  template<
    typename Holder_t
  , typename Interface
  , typename overloads = interface_overloads_t<Interface>
  > class Holder;

  // non-const invoker call implementation
  // Consumes a single Signature<> from overloads<> typelist  
  template< typename Holder_t,
	    template< typename... > class interface,
	    typename Invoker, typename... Invokers,
	    typename Return, typename... Args, typename... other >
  class Holder< Holder_t,
		interface< Invoker, Invokers... >,
		overloads< Signature< Return(Args...) >, other... > >
    : public Holder< Holder_t, interface< Invoker, Invokers... >, overloads<other...> >
  {
  public:
    virtual Return call( type<Invoker>*, Args... args ) override {
      auto& holder = static_cast< Holder_t& >(*this); // CRTP
      return impl__::call<Invoker>( holder, std::forward<Args>(args)... );
    }
    virtual ~Holder() = default;
  };

  // const invoker call implementation
  // Consumes a single Signature<> from overloads<> typelist  
  template< typename Holder_t,
	    template< typename... > class interface,
	    typename Invoker, typename... Invokers,
	    typename Return, typename... Args, typename... other >
  class Holder< Holder_t,
		interface< const Invoker, Invokers... >,
		overloads< Signature< Return(Args...) >, other... > >
    : public Holder< Holder_t, interface< const Invoker, Invokers... >, overloads<other...> >
  {
  public:
    virtual Return call( type<const Invoker>*, Args... args ) const override {
      auto& holder = static_cast< const Holder_t& >(*this); // CRTP
      return impl__::call<const Invoker>( holder, std::forward<Args>(args)... );
    }
    virtual ~Holder() = default;
  };

    // overloads<> recursion termination, consumes Invoker
  template< typename Holder_t,
	    template< typename... > class interface,
	    typename Invoker, typename... Invokers >
  class Holder< Holder_t, interface< Invoker, Invokers... >, overloads<> >
    : public Holder< Holder_t, interface< Invokers... > >
  { };
    // interface<> recursion termination, inherits from appropriate ::IHolder
  template< typename Holder_t,
	    template< typename... > class interface >
  class Holder< Holder_t, interface<>, overloads<> >
    : public holder_interface_t< Holder_t >
  { };

}

// Base class of data holders of poly
// Defines interface of call methods
template< template< typename... > class typelist, typename... Invokers >
class IHolder< typelist< Invokers... > >
  : public impl__::IHolder< typelist<Invokers...> >
{
public:
  virtual std::unique_ptr< IHolder > copy() const = 0;
};


// Holder_CRTP is needed to hide the interface typelist in Holder
// to prevent excessive compilation error lengths
template< typename Holder_t, typename Interface >
class Holder_CRTP;

template< typename Holder_t, template< typename... > class typelist, typename... Invokers >
class Holder_CRTP< Holder_t, typelist< Invokers... > >
  : public impl__::Holder< Holder_t, typelist< Invokers...> >
{
private:
  using IHolder = IHolder< typelist< Invokers...> >;

public:
  virtual std::unique_ptr< IHolder > copy() const override {
    return
      std::unique_ptr< IHolder >(
        std::make_unique< Holder_t >( static_cast< const Holder_t&>(*this) )
      );
  }
};

// Concrete data holder of poly
// Implements the interface of IHolder< Interface >
template< typename T, typename Interface >
class Holder
  : public Holder_CRTP< Holder< T, Interface >, typename Interface::interface_type >
{
public:
  template< typename... Ts >
  Holder( Ts&&... vs )
    : value( std::forward<Ts>(vs)... )
  {  }

  T value;
};


#endif // __HOLDER_HPP__
