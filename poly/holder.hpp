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

  template< typename T >
  struct type{};

  template< typename T >
  struct t_{ using type = T; };

  // Defines an interface for call
  template<
    typename Interface, typename Signature = typename
    std::conditional< length<Interface>::value == 0,
		      t_< overloads<> >,
		      generate_overloads< head_t<Interface > > >::type::type
  > class IHolder;

  // non-const invoker specialization with non-const call method
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
    using IHolder< typelist< Invoker, Invokers...>, overloads< sigs... > >::call;
    
    virtual Return  call( type<Invoker>, Args... args ) = 0;
    virtual ~IHolder() = default;
  };
  // Const invoker specialization with const call method
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
    using IHolder< typelist< const Invoker, Invokers...>, overloads< sigs... > >::call;
    
    virtual Return  call( type<const Invoker>, Args... args ) const = 0;
    virtual ~IHolder() = default;
  };

  template< template< typename... > class typelist, typename Invoker, typename... Invokers >
  class IHolder< typelist<Invoker, Invokers...>, overloads<  > >
    : public IHolder< typelist< Invokers... > >
  {
  public:
    using IHolder< typelist< Invokers... > >::call;
    
    virtual ~IHolder() = default;
  };


  template<
      template< typename... > class typelist >
  class IHolder< typelist<>, overloads<  > >
  {
  protected:
    struct None{};
    virtual void call( type<None> ){};
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

  template< typename holder_t >
  struct holder_interface;

  template< typename T, template< typename... > class typelist, typename... Invokers >
  struct holder_interface< ::Holder< T, typelist< Invokers ... > > >{
    using type = ::IHolder< typelist< Invokers... > >;
  };

  template< typename holder_t >
  using holder_interface_t = typename holder_interface<holder_t>::type;
  
  // Concrete holder implementation
  // Sig argument is a signature of a given invoker
  template< typename Holder_t, typename Interface
	    , typename Sig =
	    typename generate_overloads< signature_t< head_t<Interface> > >::type
  > class Holder;

  // non-const invoker call implementation
  template< typename Holder_t,
	    template< typename... > class interface,
	    typename Invoker, typename... Invokers,
	    typename Return, typename... Args, typename... other >
  class Holder< Holder_t,
		interface< Invoker, Invokers... >,
		overloads< Signature< Return(Args...) >, other... >
		>
    : public Holder< Holder_t, interface< Invoker, Invokers... >, overloads<other...> >
  {
  public:
    virtual Return call( type<Invoker>, Args... args ) override {
      auto& holder = static_cast< Holder_t& >(*this); // CRTP
      return impl__::call<Invoker>( holder, std::forward<Args>(args)... );
    }
    virtual ~Holder() = default;
  };

  // const invoker call implementation
  template< typename Holder_t,
	    template< typename... > class interface,
	    typename Invoker, typename... Invokers,
	    typename Return, typename... Args, typename... other >
  class Holder< Holder_t,
		interface< const Invoker, Invokers... >,
		overloads< Signature< Return(Args...) >, other... >
		>
    : public Holder< Holder_t, interface< const Invoker, Invokers... >, overloads<other...> >
  {
  public:
    virtual Return call( type<const Invoker>, Args... args ) const override {
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
    : public Holder< Holder_t, interface< Invokers... >, overloads<> >
  { };
    // interface<> recursion termination, inherits from appropriate ::IHolder
  template< typename Holder_t,
	    template< typename... > class interface >
  class Holder< Holder_t, interface<>, overloads<> >
    : public holder_interface_t< Holder_t >
  { };

}

// Function that calls the call method if IHolder of an appropriate base
// Base is selected depending on Invoker type
template< typename Invoker, typename Interface, typename... Args >
return_t<Invoker> call( IHolder< Interface >& iholder, Args&&... args )
{
  using IHolder_t =       impl__::IHolder<        Invoker >;
  auto& impl = static_cast< IHolder_t& >(iholder); // disambiguation
  return impl.call( std::forward<Args>(args)... ); //call
};

template< typename Invoker, typename Interface, typename... Args >
return_t<Invoker> call( const IHolder< Interface >& iholder, Args&&... args )
{
  using IHolder_t = const impl__::IHolder< const Invoker >;
  auto& impl = static_cast< IHolder_t& >(iholder); // disambiguation
  return impl.call( std::forward<Args>(args)... ); //call
};

// Base class of data holders of poly
// Defines interface of call methods and
// implements a call method that serves as disambiguation
template< template< typename... > class typelist, typename... Invokers >
class IHolder< typelist< Invokers... > >
  : public impl__::IHolder< typelist<Invokers...> >
{
 public:  
  virtual ~IHolder() = default;
};

template< typename Interface >
class Anchor;

template< template< typename... > class typelist, typename... Invokers >
class Anchor< typelist< Invokers... > >
{
public:
  using interface_t = std::tuple< IHolder< Invokers >*... >;
  
  virtual std::unique_ptr< Anchor >      copy() const = 0;
  virtual interface_t                    interface() = 0;

  virtual ~Anchor() = default;
};

template< typename Holder_t, typename Interface >
class Holder_CRTP;

template< typename Holder_t, template< typename... > class typelist, typename... Invokers >
class Holder_CRTP< Holder_t, typelist< Invokers... > >
  : public impl__::Holder< Holder_t, typelist< Invokers...> >
  , public Anchor< typelist<Invokers...> >
{
private:
  using Anchor = Anchor< typelist<Invokers...> >;

 public:
  virtual std::unique_ptr< Anchor > copy() const override {
    return
      std::unique_ptr< Anchor >(
        std::make_unique< Holder_t >( static_cast< const Holder_t&>(*this) )
      );
  }

  virtual ~Holder_CRTP() = default;
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
