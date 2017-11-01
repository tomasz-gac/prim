#ifndef __HOLDER_HPP__
#define __HOLDER_HPP__

#include "signature.hpp"
#include "typelist.hpp"

// Template variable that implements invocation of an Invoker for type T
// Requires a function call operator ( T, args... ) 
template< typename Invoker, typename T, typename = void >
static constexpr auto invoke = Invoker();

// Abstract base class for data vtables of poly
template< typename Interface >
class IVTable;

// Concrete data vtable of poly
template< typename T, typename Interface >
class VTable;


// IVTable and VTable implementations
namespace impl__{

  // Defines an interface for call
  // Terminates on empty Interface
  template< typename Interface >
  class IVTable;

  // Consume a single Tag and delegate implementation to an Invoker
  template< template< typename... > class typelist, typename Tag, typename... Tags >
  class IVTable< typelist< Tag, Tags...>>
    : public IVTable< typelist< Tags... > >
    , public ::Invoker< Tag >
  {
  public:
    // Forward the already defined call method from base class
    using ::Invoker< Tag >::call;
    using IVTable< typelist< Tags... > >::call;
  };

  // Recursion termination. Defines a dummy call to keep consistency
  // with call forwarding
  template< template< typename... > class typelist >
  class IVTable< typelist<> >  
  {
  private:
    struct Invalid;
  protected:
    // Dummy call method for consistency in forwarding    
    virtual void call( type<Invalid>* ){};
  public:
    virtual ~IVTable() = default;
  };

  //deduces appropriate IVTable for a given vtable_t to inherit from
  template< typename vtable_t >
  struct vtable_interface;

  template< typename T, typename Interface >
  struct vtable_interface< ::VTable< T, Interface > >{
    using type = ::IVTable< typename Interface::interface_type >;
  };

  template< typename vtable_t >
  using vtable_interface_t = typename vtable_interface<vtable_t>::type;

  //calls generate_overloads on Interface if it is non-empty.
  template< typename Interface >
  using interface_overloads_t = typename
    std::conditional_t< length<Interface>::value == 0,
  		        overloads<>,
		        overloads_t<head_or_t<Interface,Signature< void()>>> >;
  

  // Concrete vtable implementation
  // Implements call methods by using invoke template variable
  template<
    typename VTable_t
  , typename Interface
  , typename overloads = interface_overloads_t<Interface>
  > class VTable;

  
  // non-const invoker call implementation
  // Consumes a single Signature<> from overloads<> typelist  
  template< typename VTable_t,
	    template< typename... > class interface,
	    typename Tag, typename... Tags,
	    typename Return, typename... Args, typename... other >
  class VTable< VTable_t,
		interface< Tag, Tags... >,
		overloads< Signature< Return(Args...) >, other... > >
    : public VTable< VTable_t, interface< Tag, Tags... >, overloads<other...> >
  {
  public:
    virtual Return call( type<Tag>*, erased_t<Args>... args ) override final {
      using resolved_invoker = resolve_invoker< Tag, Args... >;
      using T = typename VTable_t::type;
      return invoke< resolved_invoker, T >
      	( Eraser<Args>::template unerase<T>
	  ( static_cast<erased_t<Args>&&>(args) )...
	  );
    }
    virtual ~VTable() = default;
  };

  // const invoker call implementation
  // Consumes a single Signature<> from overloads<> typelist  
  template< typename VTable_t,
	    template< typename... > class interface,
	    typename Tag, typename... Tags,
	    typename Return, typename... Args, typename... other >
  class VTable< VTable_t,
		interface< const Tag, Tags... >,
		overloads< Signature< Return(Args...) >, other... > >
    : public VTable< VTable_t, interface< const Tag, Tags... >, overloads<other...> >
  {
  public:
    virtual Return call( type<const Tag>*, erased_t<Args>... args ) const override final {
      using resolved_invoker = const resolve_invoker< Tag, Args... >;
      using T = const typename VTable_t::type;  
      return invoke< resolved_invoker, T >
      	( Eraser<Args>::template unerase<T>
	  ( static_cast<erased_t<Args&&>>(args) )...
	  );
    }
    virtual ~VTable() = default;
  };

    // overloads<> recursion termination, consumes Invoker
  template< typename VTable_t,
	    template< typename... > class interface,
	    typename Tag, typename... Tags >
  class VTable< VTable_t, interface< Tag, Tags... >, overloads<> >
    : public VTable< VTable_t, interface< Tags... > >
  { };
    // interface<> recursion termination, inherits from appropriate ::IVTable
  template< typename VTable_t,
	    template< typename... > class interface >
  class VTable< VTable_t, interface<>, overloads<> >
    : public vtable_interface_t< VTable_t >
  { };

}

// Base class of data vtables of poly
// Defines interface of call methods
template< template< typename... > class typelist, typename... Invokers >
class IVTable< typelist< Invokers... > >
  : public impl__::IVTable< typelist<Invokers...> >
{
public:
  virtual std::unique_ptr< IVTable > copy() const = 0;
};


// VTable_CRTP is needed to hide the interface typelist in VTable
// to prevent excessive compilation error lengths
template< typename VTable_t, typename Interface >
class VTable_CRTP;

template< typename VTable_t, template< typename... > class typelist, typename... Invokers >
class VTable_CRTP< VTable_t, typelist< Invokers... > >
  : public impl__::VTable< VTable_t, typelist< Invokers...> >
{
private:
  using IVTable = IVTable< typelist< Invokers...> >;

public:
  virtual std::unique_ptr< IVTable > copy() const override {
    return
      std::unique_ptr< IVTable >(
        std::make_unique< VTable_t >( static_cast< const VTable_t&>(*this) )
      );
  }
};

// Concrete data vtable of poly
// Implements the interface of IVTable< Interface >
template< typename T, typename Interface >
class VTable
  : public VTable_CRTP< VTable< T, Interface >, typename Interface::interface_type >
{
public:
  using type = T;
  // template< typename... Ts >
  // VTable( Ts&&... vs )
  //   : value( std::forward<Ts>(vs)... )
  // {  }

  // T value;
};


#endif // __HOLDER_HPP__
