#ifndef __POINTER_HPP__
#define __POINTER_HPP__

#include <memory>
#include <tuple>
#include "invoker.hpp"
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

template< typename VTable >
class pointer;
  
template< typename VTable >
class pointer
{
public:
  using implementation = VTable;
  using interface = interface_t<implementation>;
  using erased_type = typename VTable::erased_type;

private:
  template< typename Invoker >
  using enable_if_supports = std::enable_if_t< supports<interface, Invoker>() >;

protected:
  template< typename SignatureT >
  using Transform = Erase<SignatureT, erased_type>;
  
  implementation vtable_;
  erased_type data_;

public:

  template< typename T >
  pointer& operator=( T* v ){
    return *this = pointer( v );
  }
  
  pointer& operator=( const pointer&  other ) = default;
  pointer& operator=(       pointer&& other ) noexcept = default;

        implementation& vtable()       { return vtable_; }
  const implementation& vtable() const { return vtable_; }

        erased_type& value()       { return data_; }
  const erased_type& value() const { return data_; }

  pointer()
    : vtable_( implementation::template make< Invalid >() )
    , data_{ nullptr }
  {  }

  template< typename T >
  pointer( T* v )
    : vtable_( implementation::template make< T >() )
    , data_{ v }
  {  }

  pointer( const pointer& ) = default;
  pointer( pointer&& ) noexcept = default;

  template< typename OtherImplementation >
  pointer( const pointer< OtherImplementation >& other )
    : vtable_( other.vtable_ )
    , data_( other.data_ )
  {  }

private:
  template< typename I >
  friend class pointer;

  friend class call_impl;

};

template< typename T >
class unwrap_{
public:
  friend class call_impl;

  template< typename U >
  unwrap_( U&& v )
    : value( std::forward<U>(v) )
  {  }
  
private:
  T value;
};

template< typename T >
struct is_unwrap : std::false_type{};

template< typename T >
struct is_unwrap< unwrap_< T > > : std::true_type{};
  
class call_impl{
public:
  template< typename Invoker, std::size_t index = 0, typename... Ts >
  static decltype(auto) call( Ts&&... vs ){
    auto& vtable = std::get<index>( get_vtables( std::forward<Ts>(vs)... ) );
    return vtable.template get<Invoker>()( unwrap( std::forward<Ts>(vs) )... );
  }
  
private:
  // Collects vtables of vs into an std::tuple
  template< typename... Us >
  static auto get_vtables( Us&&... vs ) {
    return std::tuple_cat( get_vtables_impl( is_unwrap< std::decay_t<Us> >(), std::forward<Us>(vs) )... );
  }

  template< typename U >
  static std::tuple<> get_vtables_impl( std::false_type, U&& v ){
    return {};
  }

  template< typename U >
  static auto get_vtables_impl( std::true_type, U&& v ){
    return std::tie( v.value.vtable() );
  }
  
  
  // unwraps the parameter if it is wrapped in unwrap_ struct
  // If it is, the function returns its cv-reference-qualified erased_t data_ member
  // Forwards the parameter otherwise
  template< typename U >
  static decltype(auto) unwrap( U&& value ) {
    return unwrap_impl( is_unwrap< std::decay_t<U> >(), std::forward<U>(value) );
  }
  
  template< typename U >
  static decltype(auto)
  unwrap_impl( std::false_type, U&& v ) {
    return std::forward<U>(v);
  }

  template< typename U >
  static decltype(auto) //copy_cv_ref_t<U&&, erased_t >
  unwrap_impl( std::true_type, U&& v )
  {
    using erased_t = decltype(v.value.data_);
    using unwrapped_t = decltype( v.value );
    using erased_t_cv_ref = copy_cv_ref_t<unwrapped_t, erased_t >;
    return static_cast< erased_t_cv_ref >(v.value.data_);
  }
};

template< typename Invoker, typename... Ts >
decltype(auto) call( Ts&&... vs ){
  return call_impl::template call<Invoker>( std::forward<Ts>(vs)... );
}


  // Unary * operator for wrapping pointers in unwrap_ with proper cv-ref qualification
template< typename Impl >
unwrap_<       pointer<Impl>&  > operator*(       pointer<Impl>& ptr ){ return {ptr}; }
template< typename Impl >
unwrap_< const pointer<Impl>&  > operator*( const pointer<Impl>& ptr ){ return {ptr}; }
template< typename Impl >
unwrap_<       pointer<Impl>&& > operator*(       pointer<Impl>&& ptr ){ return {std::move(ptr)}; }
template< typename Impl >
unwrap_< const pointer<Impl>&& > operator*( const pointer<Impl>&& ptr ){ return {std::move(ptr)}; }
template< typename Impl >
unwrap_< volatile pointer<Impl>&  > operator*( volatile pointer<Impl>& ptr ){ return {ptr}; }
template< typename Impl >
unwrap_< const volatile pointer<Impl>&  > operator*( const volatile pointer<Impl>& ptr ){ return {ptr}; }
template< typename Impl >
unwrap_< volatile pointer<Impl>&& > operator*(       volatile pointer<Impl>&& ptr ){ return {std::move(ptr)}; }
template< typename Impl >
unwrap_< const volatile pointer<Impl>&& >operator*( const volatile pointer<Impl>&& ptr ){ return {std::move(ptr)}; }
  
}  

  
#endif // __POINTER_HPP__
