#ifndef __COMPILER_VTABLE_HPP__
#define __COMPILER_VTABLE_HPP__

#include "signature.hpp"
#include "typelist.hpp"
#include "thunk.hpp"

// Abstract base class for data vtables of poly
template< typename Interface >
class IVTable;

// Concrete data vtable of poly
template< typename T, typename Interface >
class VTable_impl;

// IVTable and VTable implementations
namespace impl__{

  // Concrete vtable implementation
  // Implements call methods by using invoke template variable
  template<
    typename Interface
  , typename overloads = interface_overloads_t<Interface>
  > class IVTable;

  
  // non-const invoker call implementation
  // Consumes a single Signature<> from overloads<> typelist  
  template< template< typename... > class interface,
	    typename Tag, typename... Tags,
	    typename Return, typename... Args, typename... other >
  class IVTable< interface< Tag, Tags... >,
		overloads< Signature< Return(Args...) >, other... > >
    : public IVTable< interface< Tag, Tags... >, overloads<other...> >
  {
  public:
    using IVTable< interface< Tag, Tags... >, overloads<other...> >::call;
    virtual Return call( type<Tag>*, erased_t<Args>... args ) = 0;
    virtual ~IVTable() = default;
  };

  // non-const invoker call implementation
  // Consumes a single Signature<> from overloads<> typelist  
  template< template< typename... > class interface,
	    typename Tag, typename... Tags,
	    typename Return, typename... Args, typename... other >
  class IVTable< interface< const Tag, Tags... >,
		overloads< Signature< Return(Args...) >, other... > >
    : public IVTable< interface< const Tag, Tags... >, overloads<other...> >
  {
  public:
    using IVTable< interface< const Tag, Tags... >, overloads<other...> >::call;
    virtual Return call( type<const Tag>*, erased_t<Args>... args ) const = 0;
    virtual ~IVTable() = default;
  };

    // overloads<> recursion termination, consumes Invoker
  template< template< typename... > class interface,
	    typename Tag, typename... Tags >
  class IVTable< interface< Tag, Tags... >, overloads<> >
    : public IVTable< interface< Tags... > >
  {
  public:
    using IVTable< interface< Tags... > >::call;
  };
    // interface<> recursion termination, inherits from appropriate ::IVTable
  template< template< typename... > class interface >
  class IVTable< interface<>, overloads<> >
  {
  protected:
    // Dummy call method for consistency in forwarding    
    virtual void call(){};
  public:
    virtual ~IVTable() = default;
  };

  //deduces appropriate IVTable for a given vtable_t to inherit from
  template< typename vtable_t >
  struct vtable_interface;

  template< typename T, typename Interface >
  struct vtable_interface< ::VTable_impl< T, Interface > >{
    using type = ::IVTable< typename Interface::interface_type >;
  };

  template< typename vtable_t >
  using vtable_interface_t = typename vtable_interface<vtable_t>::type;

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
    virtual Return call( type<Tag>* tag, erased_t<Args>... args ) override final {
      using T = typename VTable_t::type;
      return thunk<Tag,T,Return,Args...>( static_cast<erased_t<Args>&&>(args)... );
    }
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
    virtual Return call( type<const Tag>* tag, erased_t<Args>... args ) const override final {
      using T = const typename VTable_t::type;
      return thunk<const Tag,T,Return,Args...>( static_cast<erased_t<Args>&&>(args)... );
    }
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
class VTable_impl
  : public VTable_CRTP< VTable_impl< T, Interface >, typename Interface::interface_type >
{
public:
  using type = T;
};

template< typename Interface >
class CVTable{
public:
  using impl = VTable_impl<void*, Interface>;
  using storage_type = std::aligned_storage_t< sizeof( impl ), alignof( impl ) >;

  IVTable< Interface >* vtable_;
  storage_type storage_;

public:
  template< typename T >
  static CVTable make(){
    auto instance = CVTable();

    auto ptr = instance.storage();
    auto space = sizeof(storage_);
    auto result = std::align( alignof(storage_), space, ptr, space );
    assert( result == instance.storage() );

    instance.vtable_ = new(instance.storage()) VTable_impl<T,Interface>();
    
    return instance;
  }

  template<
    typename Invoker
  , typename = std::enable_if_t< in_typelist<Interface, std::remove_const_t<Invoker>>::value >
  , typename... Ts 
  > auto call( Ts&&... vs )
  {
    type<Invoker>* invoker = nullptr;
    return vtable_->call( invoker, std::forward<Ts>(vs)... );
  }

  template<
    typename Invoker
  , typename = std::enable_if_t< in_typelist<Interface, std::add_const_t<Invoker>>::value >
  , typename... Ts
  > auto call( Ts&&... vs ) const
  {
    type<const Invoker>* invoker = nullptr;
    return vtable_->call( invoker, std::forward<Ts>(vs)... );
  }

  ~CVTable(){ vtable_->~IVTable< Interface >(); }
  
private:
  CVTable() = default;
  void* storage(){
    return reinterpret_cast< void* >(&storage_);
  };
  const void* storage() const {
    return reinterpret_cast< const void* >(&storage_);
  };
};


#endif // __COMPILER_VTABLE_HPP__
