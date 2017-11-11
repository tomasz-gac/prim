#ifndef __VTABLE_HPP__
#define __VTABLE_HPP__

#include "thunk.hpp"

// Thunk of a tag that contains function pointers for all overloads
template< typename Tag >
class Thunk
{
private:
  using thunk_tuple = repack_t< map_t< overloads_t<Tag>, thunk_type >, std::tuple<> >;

  Thunk( thunk_tuple thunks )
    : thunks_( std::move( thunks ) )
  {  }

  thunk_tuple thunks_;

  template< typename T, typename... Overloads >
  static Thunk make_impl( overloads< Overloads... >* ){
    return { std::make_tuple( get_thunk<Tag, T, Overloads>()... ) };
  };
  
public:
  // Make thunk for type T
  template< typename T = void >
  static Thunk make(){
    // for Overloads... parameter pack
    return make_impl<T>(static_cast< overloads_t<Tag>* >(nullptr));
  };

  template< typename... Args >
  decltype(auto) operator()( Args&&... args ) const {
    using Signature = unerase_signature< Tag, Args&&... >;
    static_assert( !std::is_same< Signature, invalid_arguments >::value,
		   "Invoker cannot be called with supplied arguments" );
    return (*std::get< thunk_type< Signature > >(thunks_))( std::forward<Args>(args)... );
  }
};

// VTable that holds thunks locally
template< typename Interface >
class Local{
private:
  using thunk_tuple = repack_t< map_t< interface_t<Interface>, bind<1, Thunk >::template type >, std::tuple<> >;
  
  thunk_tuple thunks_;// std::tuple< Thunk<Tags>... > thunks_;

  Local( thunk_tuple thunks )
    : thunks_( std::move( thunks ) )
  {  }

  template< typename T, typename... Tags >
  static Local make_impl( ::Interface<Tags...>*  ){
    return { std::make_tuple( Thunk<Tags>::template make<T>() ... ) };
  }
public:
  template< typename T = void >
  static Local make(){
    return make_impl<T>( static_cast< interface_t<Interface>* >(nullptr) );
  }

  template< typename... Ts >
  explicit operator Local<Ts...>() const {
    return { std::get< Thunk<Ts> >(thunks_)... };
  }

  // Get a thunk based on tag and signature
  template< typename Tag >
  const Thunk< Tag >& get() const {
    return std::get< Thunk<Tag> >( thunks_ );
  }

  template< typename Tag >
  const Thunk< Tag >& operator[]( const Tag& ) const {
    return get< Tag >();
  }
};

// VTable that holds thunks remotely
template< typename Interface >
class Remote{
private:
  using VTable = Local<Interface>;
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

  template< typename... Ts >
  explicit operator Local<Ts...>() const {
    return static_cast< Local<Ts...> >(vtable_);
  }

  template< typename Tag >
  const Thunk< Tag >& operator[]( const Tag& ) const {
    return get< Tag >();
  }

  template< typename Tag >
  const Thunk< Tag >& get() const {
    return vtable_.template get< Thunk<Tag> >();
  }
};

#endif // __VTABLE_HPP__

