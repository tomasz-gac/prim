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

  // Defines an interface for call
  // Terminates on empty Interface
  template< typename Interface >
  class IHolder;

  // Consume a single Tag and delegate implementation to an Invoker
  template< template< typename... > class typelist, typename Tag, typename... Tags >
  class IHolder< typelist< Tag, Tags...>>
    : public IHolder< typelist< Tags... > >
    , public ::Invoker< Tag >
  {
  public:
    // Forward the already defined call method from base class
    using ::Invoker< Tag >::call;
    using IHolder< typelist< Tags... > >::call;
  };

  // Recursion termination. Defines a dummy call to keep consistency
  // with call forwarding
  template< template< typename... > class typelist >
  class IHolder< typelist<> >  
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
  auto call( ::Holder< T, Interface >& holder, Args&&... args )
  {
    using resolved_invoker = resolve_invoker< Invoker, Args... >;
    using Invoker_t = std::remove_const_t<resolved_invoker>;
    using type = std::remove_const_t<T>;
    return invoke< Invoker_t, type >( holder.value, std::forward<Args>(args)... );
  }

  // const version
  template< typename Invoker, typename T, typename Interface, typename... Args >
  auto call( const ::Holder< T, Interface >& holder, Args&&... args ){
    using resolved_invoker = resolve_invoker< Invoker, Args... >;
    using Invoker_t = std::add_const_t<resolved_invoker>;
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

  //calls generate_overloads on Interface if it is non-empty.
  template< typename Interface >
  using interface_overloads_t = typename
    std::conditional_t< length<Interface>::value == 0,
  		        overloads<>,
		        overloads_t<head_or_t<Interface,Signature< void()>>> >;
  
  // Concrete holder implementation
  // Implements call methods by using invoke template variable
  template<
    typename Holder_t
  , typename Interface
  , typename overloads = interface_overloads_t<Interface>
  > class Holder;

  // non-const invoker call implementation
  // Consumes a single Signature<> from overloads<> typelist  
  template< typename Holder_t,
	    template< typename... > class interface,
	    typename Tag, typename... Tags,
	    typename Return, typename... Args, typename... other >
  class Holder< Holder_t,
		interface< Tag, Tags... >,
		overloads< Signature< Return(Args...) >, other... > >
    : public Holder< Holder_t, interface< Tag, Tags... >, overloads<other...> >
  {
  public:
    virtual Return call( type<Tag>*, Args... args ) override final {
      auto& holder = static_cast< Holder_t& >(*this); // CRTP
      return impl__::call<Tag>( holder, std::forward<Args>(args)... );
    }
    virtual ~Holder() = default;
  };

  // const invoker call implementation
  // Consumes a single Signature<> from overloads<> typelist  
  template< typename Holder_t,
	    template< typename... > class interface,
	    typename Tag, typename... Tags,
	    typename Return, typename... Args, typename... other >
  class Holder< Holder_t,
		interface< const Tag, Tags... >,
		overloads< Signature< Return(Args...) >, other... > >
    : public Holder< Holder_t, interface< const Tag, Tags... >, overloads<other...> >
  {
  public:
    virtual Return call( type<const Tag>*, Args... args ) const override final {
      auto& holder = static_cast< const Holder_t& >(*this); // CRTP
      return impl__::call<const Tag>( holder, std::forward<Args>(args)... );
    }
    virtual ~Holder() = default;
  };

    // overloads<> recursion termination, consumes Invoker
  template< typename Holder_t,
	    template< typename... > class interface,
	    typename Tag, typename... Tags >
  class Holder< Holder_t, interface< Tag, Tags... >, overloads<> >
    : public Holder< Holder_t, interface< Tags... > >
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
