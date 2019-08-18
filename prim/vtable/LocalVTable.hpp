#ifndef __LOCAL_VTABLE_HPP__
#define __LOCAL_VTABLE_HPP__

#include "thunk.hpp"
#include "erased.hpp"

namespace prim{

// VTable that holds thunks locally
template< typename Interface, typename erased_t = Erased<void*> >
class LocalVTable{
public:
  using interface = interface_t<Interface>;
  using erased_type = erased_t;
private:
  
  template< typename Invoker >
  using TThunk = Thunk< Invoker, erased_type >;
  
  using thunk_tuple =
    tl::repack_t< tl::map_t< interface_t<Interface>, TThunk >, std::tuple<> >;
  
  thunk_tuple thunks_;

  LocalVTable( thunk_tuple thunks )
    : thunks_( std::move( thunks ) )
  {  }

  template< typename T, typename... Tags >
  static LocalVTable make_impl( prim::Interface<Tags...>*  ){
    return { std::make_tuple( TThunk<Tags>::template make<T>() ... ) };
  }

  template< typename To, typename... Tags >
  LocalVTable<To, erased_type> cast_impl( prim::Interface<Tags...>* ) const {
    return LocalVTable<To, erased_type>(std::make_tuple( std::get< TThunk<Tags> >(thunks_)... ) );
  }

  template< typename I, typename ptr_t__ >
  friend class LocalVTable;
public:

  template< typename To >
  explicit operator LocalVTable< To, erased_type >() const
  { return cast_impl<To>( static_cast< interface_t<To>* >(nullptr) ); }

  template< typename T >
  static LocalVTable make(){
    return make_impl<T>( static_cast< interface_t<Interface>* >(nullptr) );
  }

  template< typename To, typename From, typename ptr_t >
  friend LocalVTable<To,ptr_t> interface_cast( const LocalVTable<From,ptr_t>& vtbl );

  // Get a thunk based on tag
  template< typename Tag >
  const TThunk< Tag >& get() const {
    return std::get< TThunk<Tag> >( thunks_ );

  }

  template< typename Tag >
  const TThunk< Tag >& operator[]( const Tag& ) const {
    return get< Tag >();
  }

  bool operator==( const LocalVTable& other ) const {
    return thunks_ == other.thunks_;
  }

  bool operator!=( const LocalVTable& other ) const {
    return thunks_ != other.thunks_;
  }
};

}

#endif // __LOCAL_VTABLE_HPP__
