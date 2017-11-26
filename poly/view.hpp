#ifndef __VIEW_HPP__
#define __VIEW_HPP__

#include <memory>
#include <tuple>
#include "vtable.hpp"

template<
  typename VTable
>
class View;

template< typename T > struct is_view : std::false_type{};
template<
  typename Interface
> struct is_view< View<Interface> > : std::true_type{};

template<
  typename VTable
> class View
{
public:
  using interface = interface_t<VTable>;
private:
  template< typename Invoker >
  using enable_if_supports = std::enable_if_t< supports<interface, Invoker>() >;

  VTable vtable_;
  void* data_;

public:
  template< typename T >
  View& operator=( T v ){
    return *this = View( std::move(v) );
  }
  
  View& operator=( const View&  other ){ return *this = View( other ); }
  View& operator=(       View&& other ) noexcept = default;

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
  
  template< typename T >
  explicit operator T() const { return *reinterpret_cast< T* >(data_); }
 
  template< typename T >
  explicit operator T() { return *reinterpret_cast< T* >(data_); }

  template< typename T, typename = std::enable_if_t< !is_view<std::decay_t<T>>::value > >
  View( T& v )
    : vtable_( VTable::template make< T >() )
    , data_( reinterpret_cast< void* >(&v) )
  {  }

  View( const View& ) = default;
  View( View&& ) noexcept = default;

  template< typename OtherVTable >
  View( const View< OtherVTable >& other )
    : vtable_( other.vtable_ )
    , data_( other.data_ )
  {  }
private:
  template< typename I >
  friend class View;

  View( void* data, VTable vtable )
    : vtable_(vtable)
    , data_(data)
  {  }

  template< typename U >
  static decltype(auto) unpack( U&& value ){
    return unpack_impl( is_view< std::decay_t<U> >(), std::forward<U>(value) );
  }
  
  template< typename U >
  static decltype(auto)
  unpack_impl( std::false_type, U&& v ){
    return std::forward<U>(v);
  }
  
  template< typename U >
  static copy_cv_ref_t<U&&, void*>
  unpack_impl( std::true_type, U&& v )
  {
    return static_cast< copy_cv_ref_t<U&&, void*> >(v.data_);
  }
  
};

#endif // __POLY_HPP__
