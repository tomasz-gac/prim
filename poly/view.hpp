#ifndef __POINTER_HPP__
#define __POINTER_HPP__

#include <memory>
#include <tuple>
#include "invoker.hpp"
#include "./vtable/implementation.hpp"
#include "./vtable/invalid.hpp"
#include "./vtable/Erase.hpp"

template<typename A, typename B>
using disable_if_same_or_derived =
    typename std::enable_if<
        !std::is_base_of<A,typename
             std::remove_reference<B>::type
        >::value
    >::type;


namespace poly{

template< typename T >
struct unwrap_{
  T value;
};

template< typename VTable >
class Pointer;

template< typename T >
struct is_unwrap : std::false_type{};

template< typename T >
struct is_unwrap< unwrap_< T > > : std::true_type{};

template< typename T > struct is_pointer : std::false_type{};
template< typename Interface>
struct is_pointer< Pointer<Interface> > : std::true_type{};

template< typename VTable >
class Pointer
{
public:
  using implementation = impl_t<VTable>;
  using interface = interface_t<implementation>;
  using pointer_type = typename VTable::pointer_type;

private:
  template< typename Invoker >
  using enable_if_supports = std::enable_if_t< supports<interface, Invoker>() >;

protected:
  using erased_t = Erased<pointer_type>;
  template< typename SignatureT >
  using Transform = Erase<SignatureT, pointer_type>;
  
  implementation vtable_;
  erased_t data_;

public:

  template< typename T >
  Pointer& operator=( T* v ){
    return *this = Pointer( v );
  }
  
  Pointer& operator=( const Pointer&  other ) = default;
  Pointer& operator=(       Pointer&& other ) noexcept = default;

  template< typename Invoker, typename = enable_if_supports< Invoker > >
  auto operator[]( const Invoker& )
  { return get< Invoker >(); }

  template< typename Invoker, typename = enable_if_supports< Invoker > >
  auto operator[]( const Invoker& ) const
  { return get< Invoker >(); }

  
  template< typename Invoker, typename = enable_if_supports< Invoker > >
  auto get()
  {
    return [this]( auto&&... args ) -> decltype(auto){
      return
	vtable_.template get<Invoker>()
	  ( data_, unpack( std::forward<decltype(args)>(args) )... );
    };
  }
  
  template< typename Invoker, typename = enable_if_supports< Invoker > >
  auto get() const
  {
    return [this]( auto&&... args ) -> decltype(auto) {
      return 
	vtable_.template get<Invoker>()
	  ( data_, unpack( std::forward<decltype(args)>(args) )... );
    };
  }

  template< typename Invoker, typename... Args, typename = enable_if_supports< Invoker > >
  decltype(auto) call( Args&&... args ){
    return get<Invoker>()( std::forward<Args>(args)... );
  }
  
  template< typename Invoker, typename... Args, typename = enable_if_supports< Invoker > >
  decltype(auto) call( Args&&... args ) const {
    return get<Invoker>()( std::forward<Args>(args)... );
  }

        implementation& vtable()       { return vtable_; }
  const implementation& vtable() const { return vtable_; }

        pointer_type& value()       { return data_.data; }
  const pointer_type& value() const { return data_.data; }

  Pointer()
    : vtable_( implementation::template make< Invalid >() )
    , data_{ nullptr }
  {  }

  template< typename T >
  Pointer( T* v )
    : vtable_( implementation::template make< T >() )
    , data_{ Transform<poly::T>::apply(v) }
  {  }

  Pointer( const Pointer& ) = default;
  Pointer( Pointer&& ) noexcept = default;

  template< typename OtherImplementation >
  Pointer( const Pointer< OtherImplementation >& other )
    : vtable_( other.vtable_ )
    , data_( other.data_ )
  {  }

private:
  template< typename I >
  friend class Pointer;

  // unwraps the parameter if it is wrapped in unwrap_ struct
  // If it is, the function returns its cv-reference-qualified erased_t data_ member
  // Forwards the parameter otherwise
  template< typename U >
  static decltype(auto) unpack( U&& value ){
    return unpack_impl( is_unwrap< std::decay_t<U> >(), std::forward<U>(value) );
  }
  
  template< typename U >
  static decltype(auto)
  unpack_impl( std::false_type, U&& v ){
    return std::forward<U>(v);
  }
  
  template< typename U >
  static copy_cv_ref_t<U&&, erased_t >
  unpack_impl( std::true_type, U&& v )
  {
    return static_cast< copy_cv_ref_t<U&&, erased_t > >(v.value.data_);
  }
  
};
#define DEFINE_UNWRAP__( CLASS )				\
  template< typename VTable >                                   \
  auto unwrap( CLASS v ){ return unwrap_<CLASS>{static_cast<CLASS>(v)}; }

DEFINE_UNWRAP__(                Pointer<VTable>& )  
DEFINE_UNWRAP__( const          Pointer<VTable>& )  
DEFINE_UNWRAP__(                Pointer<VTable>&& )  
DEFINE_UNWRAP__( const          Pointer<VTable>&& )  
DEFINE_UNWRAP__(       volatile Pointer<VTable>& )  
DEFINE_UNWRAP__( const volatile Pointer<VTable>& )  
DEFINE_UNWRAP__(       volatile Pointer<VTable>&& )  
DEFINE_UNWRAP__( const volatile Pointer<VTable>&& )  
  

template< typename VTable >
class Reference
  : protected Pointer<VTable>
{
private:
  using base = Pointer<VTable>;
protected:
    // poly::Value can be implicitly cast to Reference& and we can't allow meddling with its insides
  Reference& operator=( const Reference&  other )          = default;
  Reference& operator=(       Reference&& other ) noexcept = default;
  
public:
  using implementation = typename base::implementation;
  using interface      = typename base::interface;
  using pointer_type = typename base::pointer_type;


  using base::operator[];
  using base::get;
  using base::call;
  
  const implementation& vtable() const { return this->base::vtable(); }
  // No pointer reassignment
  pointer_type          value()  const { return this->base::value(); }


  template< typename T, typename = disable_if_same_or_derived< Reference, T > >
  Reference( T& v )
    : base( &v )
  {  }

  Reference( const Reference& ) = default;
  Reference( Reference&& ) noexcept = default;

  template< typename OtherImplementation >
  Reference( const Reference< OtherImplementation >& other )
    : base( static_cast<const Pointer<OtherImplementation>&>(other) )
  {  }

  template< typename OtherImpl >
  friend class Reference;
};

DEFINE_UNWRAP__(                Reference<VTable>& )  
DEFINE_UNWRAP__( const          Reference<VTable>& )  
DEFINE_UNWRAP__(                Reference<VTable>&& )  
DEFINE_UNWRAP__( const          Reference<VTable>&& )  
DEFINE_UNWRAP__(       volatile Reference<VTable>& )  
DEFINE_UNWRAP__( const volatile Reference<VTable>& )  
DEFINE_UNWRAP__(       volatile Reference<VTable>&& )  
DEFINE_UNWRAP__( const volatile Reference<VTable>&& )  

  
}  

  
#endif // __POINTER_HPP__
