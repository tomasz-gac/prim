#ifndef __VTABLE_HPP__
#define __VTABLE_HPP__

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
class VTable_impl;


// IVTable and VTable implementations
namespace impl__{

  //calls generate_overloads on Interface if it is non-empty.
  template< typename Interface >
  using interface_overloads_t = typename
    std::conditional_t< length<Interface>::value == 0,
  		        overloads<>,
		        overloads_t<head_or_t<Interface,Signature< void()>>> >;


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

  // --------------------------------------  

 //  // Defines an interface for call
 //  // Terminates on empty Interface
 //  template< typename Interface >
 //  class IVTable;

 //  // non-const version
 //  template<   template< typename... > class typelist, typename Tag, typename... Tags
 // 	    , typename Return, typename... Args, typename... sigs >
 //  class IVtable< typelist< Tag, Tags...>, overloads< Signature< Return(Args...) >, sigs... >>
 //    : public IVTable< typelist< Tag, Tags... >, overloads< sigs... > >
 //  {
 //  public:
 //    // Forward the already defined call method from base class
 //    using IVTable< typelist< Tags... > >::call;
 
 //    virtual Return call( type<Tag>*, erased_t<Args>... args ) = 0;
 //    virtual ~IVTable() = default;
 // };

 //  // const version
 //  template<   template< typename... > class typelist, typename Tag, typename... Tags
 // 	    , typename Return, typename... Args, typename... sigs >
 //  class IVtable< typelist< const Tag, Tags...>, overloads< Signature< Return(Args...) >, sigs... >>
 //    : public IVTable< typelist< const Tag, Tags... >, overloads< sigs... > >
 //  {
 //  public:
 //    // Forward the already defined call method from base class
 //    using IVTable< typelist< Tags... > >::call;
 
 //    virtual Return call( type<const Tag>*, erased_t<Args>... args ) const = 0;
 //    virtual ~IVTable() = default;
 // };
  
 //  // Recursion termination. Defines a dummy call to keep consistency
 //  // with call forwarding
 //  template< template< typename... > class typelist >
 //  class IVTable< typelist<> >  
 //  {
 //  private:
 //    struct Invalid;
 //  protected:
 //    // Dummy call method for consistency in forwarding    
 //    virtual void call( type<Invalid>* ){};
 //  public:
 //    virtual ~IVTable() = default;
 //  };

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
    virtual Return call( type<Tag>*, erased_t<Args>... args ) override final {
      using resolved_invoker = resolve_invoker< Tag, Args... >;
      using T = typename VTable_t::type;
      return invoke< resolved_invoker, T >
      	( Eraser<Args>::template unerase<T>
	  ( static_cast<erased_t<Args>&&>(args) )...
	  );
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
    virtual Return call( type<const Tag>*, erased_t<Args>... args ) const override final {
      using resolved_invoker = const resolve_invoker< Tag, Args... >;
      using T = const typename VTable_t::type;  
      return invoke< resolved_invoker, T >
      	( Eraser<Args>::template unerase<T>
	  ( static_cast<erased_t<Args&&>>(args) )...
	  );
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
class VTable{
public:
  using impl = VTable_impl<void*, Interface>;
  using storage_type = std::aligned_storage_t< sizeof( impl ), alignof( impl ) >;

  IVTable< Interface >* vtable_;
  storage_type storage_;

  template< typename T >
  class type;
  
public:
  template< typename T >
  static VTable make(){
    auto instance = VTable();

    auto ptr = instance.storage();
    auto space = sizeof(storage_);
    auto result = std::align( alignof(storage_), space, ptr, space );
    assert( result == instance.storage() );

    instance.vtable_ = new(instance.storage()) VTable_impl<T,Interface>();
    
    return instance;
  }

  IVTable< Interface >* operator->(){
    return vtable_;
  }

  const IVTable< Interface >* operator->() const {
    return vtable_;
  }

  ~VTable(){ vtable_->~IVTable< Interface >(); }
  
private:
  VTable() = default;
  void* storage(){
    return reinterpret_cast< void* >(&storage_);
  };
  const void* storage() const {
    return reinterpret_cast< const void* >(&storage_);
  };
};


#endif // __VTABLE_HPP__
