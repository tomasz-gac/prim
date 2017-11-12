#ifndef __VTABLE_HPP__
#define __VTABLE_HPP__

#include "thunk.hpp"

// Thunk of a tag that contains function pointers for all overloads
template< typename Tag, template< typename > class Transform >
class Thunk
{
private:
  template< typename Invoker >
  using Tthunk_type = thunk_type< Invoker, Transform >;

  using thunk_tuple = repack_t< map_t< overloads_t<Tag>, Tthunk_type >, std::tuple<> >;

  Thunk( thunk_tuple thunks )
    : thunks_( std::move( thunks ) )
  {  }

  thunk_tuple thunks_;

  template< typename T, typename... Overloads >
  static Thunk make_impl( overloads< Overloads... >* ){
    return { std::make_tuple( get_thunk<Overloads, Transform, T>()... ) };
  };
  
public:
  // Make thunk for type T
  template< typename T >
  static Thunk make(){
    // for Overloads... parameter pack
    return make_impl<T>(static_cast< overloads_t<Tag>* >(nullptr));
  };

  template< typename... Args >
  decltype(auto) operator()( Args&&... args ) const {
    using Signature = unerase_signature< Tag, Transform, Args&&... >;
    static_assert( !std::is_same< Signature, invalid_arguments >::value,
		   "Invoker cannot be called with supplied arguments" );
    return (*std::get< thunk_type< Signature, Transform > >(thunks_))( std::forward<Args>(args)... );
  }
};

// VTable that holds thunks locally
template< typename Interface, template< typename > class Transform >
class Local{
private:
  template< typename Invoker >
  using TThunk = Thunk< Invoker, Transform >;
  
  using thunk_tuple =
    repack_t< map_t< interface_t<Interface>, TThunk >, std::tuple<> >;
  
  thunk_tuple thunks_;// std::tuple< Thunk<Tags>... > thunks_;

  Local( thunk_tuple thunks )
    : thunks_( std::move( thunks ) )
  {  }

  template< typename T, typename... Tags >
  static Local make_impl( ::Interface<Tags...>*  ){
    return { std::make_tuple( TThunk<Tags>::template make<T>() ... ) };
  }

  template< typename To, typename... Tags >
  Local<To, Transform> cast_impl( ::Interface<Tags...>* ) const {
    return Local<To, Transform>(std::make_tuple( std::get< TThunk<Tags> >(thunks_)... ) );
  }

  template< typename I, template< typename > class T >
  friend class Local;
public:
  template< typename T >
  static Local make(){
    return make_impl<T>( static_cast< interface_t<Interface>* >(nullptr) );
  }

  template< typename To, typename From, template< typename > class T >
  friend Local<To,T> interface_cast( const Local<From,T>& vtbl );

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

// VTable that holds thunks remotely
template< typename Interface, template< typename > class Transform >
class Remote{
private:
  using VTable = Local<Interface, Transform>;
  template< typename Invoker >
  using TThunk = Thunk< Invoker, Transform >;
  // Singleton reference
  VTable& vtable_;

  Remote( VTable& vtbl )
    : vtable_( vtbl )
  {  };
  
public:
  // Makes the VTable for type T using a singleton pattern
  template< typename T = void >
  static Remote make(){
    static VTable instance = VTable::template make<T>();
    
    return { instance };
  }

  template< typename To, typename From, template< typename > class T >
  friend Local<To,T> interface_cast( const Remote<From,T>& vtbl );

  template< typename Tag >
  const TThunk< Tag >& operator[]( const Tag& ) const {
    return get< Tag >();
  }

  template< typename Tag >
  const TThunk< Tag >& get() const {
    return vtable_.template get< TThunk<Tag> >();
  }
};

template< typename To, typename From, template< typename > class Transform >
Local<To, Transform> interface_cast( const Local<From, Transform>& vtbl ){
  return vtbl.template cast_impl<To>( static_cast< interface_t<To>* >(nullptr) );
}

template< typename To, typename From, template< typename > class Transform >
Local<To, Transform> interface_cast( const Remote<From, Transform>& vtbl ){
  return interface_cast< To >(vtbl.vtable_);
}

#endif // __VTABLE_HPP__

