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
  template<
    typename Invoker, typename Signature =
      typename generate_overloads< signature_t< Invoker > >::type
  > class IHolder;

  // non-const invoker specialization with non-const call method
  template< typename Invoker, typename Return, typename... Args, typename... sigs >
  class IHolder< Invoker, overloads< Signature< Return(Args...) >, sigs... > >
    : public IHolder< Invoker, overloads< sigs... > >
  {
  public:
    using IHolder< Invoker, overloads< sigs... > >::call;
    
    virtual Return  call( Args... args ) = 0;
    virtual ~IHolder() = default;
  };
  // Const invoker specialization with const call method
  template< typename Invoker, typename Return, typename... Args, typename... sigs >
  class IHolder< const Invoker, overloads< Signature< Return(Args...) >, sigs... > >
    : public IHolder< const Invoker, overloads< sigs... > >  
  {
  public:
    using IHolder< const Invoker, overloads< sigs... > >::call;
    
    virtual Return  call( Args... args ) const = 0;
    virtual ~IHolder() = default;
  };

  template< typename Invoker, typename Return, typename... Args >
  class IHolder< Invoker, overloads< Signature< Return(Args...) > > >
  {
  public:
    virtual Return  call( Args... args ) = 0;
    virtual ~IHolder() = default;
  };
  // Const invoker specialization with const call method
  template< typename Invoker, typename Return, typename... Args >
  class IHolder< const Invoker, overloads< Signature< Return(Args...) > > >
  {
  public:
    virtual Return  call( Args... args ) const = 0;
    virtual ~IHolder() = default;
  };
  

  // template< typename Invoker, typename o, typename... os >
  // class IHolder< Invoker, overloads< o, os... > >
  //   : public IHolder< Invoker, overloads< os... > >
  //   , public IHolder< Invoker, overloads< o > >
  // {
  // public:
  //   using IHolder< Invoker, overloads< o > >::call;
  //   using IHolder< Invoker, overloads< os... > >::call;
  // };

  // template< typename Invoker >
  // class IHolder< Invoker, overloads< > >{
  //   // static_assert( std::is_same< Invoker, overloads<> >::value, "" );
  // };

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

  // Concrete holder implementation
  // Sig argument is a signature of a given invoker
  template< typename Holder_t, typename Invoker
	    , typename Sig = typename generate_overloads< signature_t< Invoker > >::type
  > class Holder;

  // non-const invoker call implementation
  template< typename Holder_t, typename Invoker, typename Return, typename... Args, typename... other >
  class Holder< Holder_t, Invoker, overloads< Signature< Return(Args...) >, other... > >
    : public Holder< Holder_t, Invoker, overloads<other...> >
  {
  public:
    virtual Return call( Args... args ) override {
      auto& holder = static_cast< Holder_t& >(*this); // CRTP
      return impl__::call<Invoker>( holder, std::forward<Args>(args)... );
    }
    virtual ~Holder() = default;
  };

  // const invoker call implementation
  template< typename Holder_t, typename Invoker, typename Return, typename... Args, typename... other >
  class Holder< Holder_t, const Invoker, overloads< Signature< Return(Args...) >, other... > >
    : public Holder< Holder_t, const Invoker, overloads< other... > >
  {
  public:
    virtual Return call( Args... args ) const override {
      auto& holder = static_cast< const Holder_t& >(*this); // CRTP
      return impl__::call<const Invoker>( holder, std::forward<Args>(args)... );
    }
    virtual ~Holder() = default;
  };

    // non-const invoker overload recursion termination
  template< typename Holder_t, typename Invoker >
  class Holder< Holder_t, Invoker, overloads<> >
    : public ::IHolder< Invoker >
  { };

    // const invoker overload recursion termination
  template< typename Holder_t, typename Invoker >
  class Holder< Holder_t, const Invoker, overloads<> >
    : public ::IHolder< const Invoker >
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
template< typename Invoker >
class IHolder
  : public impl__::IHolder< Invoker >
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
  : public impl__::Holder< Holder_t, Invokers >...
  , public Anchor< typelist<Invokers...> >
{
private:
  using Anchor = Anchor< typelist<Invokers...> >;
  using interface_t = typename Anchor::interface_t;

 public:
  virtual std::unique_ptr< Anchor > copy() const override {
    return
      std::unique_ptr< Anchor >(
        std::make_unique< Holder_t >( static_cast< const Holder_t&>(*this) )
      );
  }

  virtual interface_t interface() override{
    return std::make_tuple( static_cast<IHolder< Invokers >*>(this) ... );
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
