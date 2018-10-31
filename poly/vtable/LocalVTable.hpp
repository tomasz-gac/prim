#ifndef __LOCAL_VTABLE_HPP__
#define __LOCAL_VTABLE_HPP__

#include "thunk.hpp"

namespace poly{

// VTable that holds thunks locally
template< typename Interface, template< typename > class Transform >
class LocalVTable{
public:
  using interface = interface_t<Interface>;
private:
  template< typename Invoker >
  using TThunk = Thunk< Invoker, Transform >;
  
  using thunk_tuple =
    tl::repack_t< tl::map_t< interface_t<Interface>, TThunk >, std::tuple<> >;
  
  thunk_tuple thunks_;

  LocalVTable( thunk_tuple thunks )
    : thunks_( std::move( thunks ) )
  {  }

  template< typename T, typename... Tags >
  static LocalVTable make_impl( poly::Interface<Tags...>*  ){
    return { std::make_tuple( TThunk<Tags>::template make<T>() ... ) };
  }

  template< typename To, typename... Tags >
  LocalVTable<To, Transform> cast_impl( poly::Interface<Tags...>* ) const {
    return LocalVTable<To, Transform>(std::make_tuple( std::get< TThunk<Tags> >(thunks_)... ) );
  }

  template< typename I, template< typename > class T >
  friend class LocalVTable;
public:

  template< typename To >
  explicit operator LocalVTable< To, Transform >() const
  { return cast_impl<To>( static_cast< interface_t<To>* >(nullptr) ); }

  template< typename T >
  static LocalVTable make(){
    return make_impl<T>( static_cast< interface_t<Interface>* >(nullptr) );
  }

  template< typename To, typename From, template< typename > class T >
  friend LocalVTable<To,T> interface_cast( const LocalVTable<From,T>& vtbl );

  // Get a thunk based on tag and signature
  template< typename Tag >
  const TThunk< Tag >& get() const {
    return std::get< TThunk<Tag> >( thunks_ );

  }

  template< typename Tag >
  const TThunk< Tag >& operator[]( const Tag& ) const {
    return get< Tag >();
  }
};

}

#endif // __LOCAL_VTABLE_HPP__
