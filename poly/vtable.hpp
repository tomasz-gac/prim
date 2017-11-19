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
class LocalVTable{
public:
  using interface = interface_t<Interface>;
private:
  template< typename Invoker >
  using TThunk = Thunk< Invoker, Transform >;
  
  using thunk_tuple =
    repack_t< map_t< interface_t<Interface>, TThunk >, std::tuple<> >;
  
  thunk_tuple thunks_;

  LocalVTable( thunk_tuple thunks )
    : thunks_( std::move( thunks ) )
  {  }

  template< typename T, typename... Tags >
  static LocalVTable make_impl( ::Interface<Tags...>*  ){
    return { std::make_tuple( TThunk<Tags>::template make<T>() ... ) };
  }

  template< typename To, typename... Tags >
  LocalVTable<To, Transform> cast_impl( ::Interface<Tags...>* ) const {
    return LocalVTable<To, Transform>(std::make_tuple( std::get< TThunk<Tags> >(thunks_)... ) );
  }

  template< typename I, template< typename > class T >
  friend class LocalVTable;
public:
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

// VTable that holds thunks remotely
template< typename Interface, template< typename > class Transform >
class RemoteVTable{
public:
  using interface = interface_t<Interface>;
private:
  using VTable = LocalVTable<Interface, Transform>;
  template< typename Invoker >
  using TThunk = Thunk< Invoker, Transform >;
  // Singleton reference
  VTable& vtable_;

  RemoteVTable( VTable& vtbl )
    : vtable_( vtbl )
  {  };
  
public:
  // Makes the VTable for type T using a singleton pattern
  template< typename T = void >
  static RemoteVTable make(){
    static VTable instance = VTable::template make<T>();
    
    return { instance };
  }

  template< typename To, typename From, template< typename > class T >
  friend LocalVTable<To,T> interface_cast( const RemoteVTable<From,T>& vtbl );

  template< typename Tag >
  const TThunk< Tag >& operator[]( const Tag& ) const {
    return get< Tag >();
  }

  template< typename Tag >
  const TThunk< Tag >& get() const {
    return vtable_.template get< Tag >();
  }
};

template< typename VT, typename... VTs >
class VTable{
public:
  using interface = interface_t< join_t< VT, VTs... > >;
private:
  using tuple_type =   std::tuple< VT, VTs... >;
  tuple_type vtables_;

  VTable( tuple_type tuple )
    : vtables_( std::move(tuple) )
  {  }
public:
  template< typename T >
  static VTable make(){
    return { std::make_tuple( VT::template make<T>(), VTs::template make<T>() ... ) };
  };

  template< typename Tag >
  decltype(auto) operator[]( const Tag& ) const {
    return get< Tag >();
  }

  template< typename Tag >
  decltype(auto) get() const {
    return get<Tag>( std::make_index_sequence< 1 + sizeof...(VTs) >(),
		     std::integer_sequence< bool,
		     supports<interface_t<VT>, Tag >(),
		     supports< interface_t<VTs>, Tag >()...
		     >() );
  }

private:
  template< typename Tag, std::size_t Index, std::size_t... Indices, bool... contains >
  decltype(auto) get(
    std::index_sequence<Index, Indices... >,
    std::integer_sequence< bool, true, contains... >
  ) const {
    return std::get<Index>(vtables_).template get< Tag >();
  }

  template< typename Tag, std::size_t Index, std::size_t... Indices, bool... contains >
  decltype(auto) get(
    std::index_sequence<Index, Indices... >,
    std::integer_sequence< bool, false, contains... >
  ) const {
    return get<Tag>( std::index_sequence<Indices... >(),
		     std::integer_sequence< bool, contains... >() );

  }
};

template< typename To, typename From, template< typename > class Transform >
LocalVTable<To, Transform> interface_cast( const LocalVTable<From, Transform>& vtbl ){
  return vtbl.template cast_impl<To>( static_cast< interface_t<To>* >(nullptr) );
}

template< typename To, typename From, template< typename > class Transform >
LocalVTable<To, Transform> interface_cast( const RemoteVTable<From, Transform>& vtbl ){
  return interface_cast< To >(vtbl.vtable_);
}

template< typename SigT >
using erased_t = std::conditional_t< is_placeholder< SigT >::value, copy_cv_ref_t< SigT, void* >, SigT>;

template< typename SignatureT >
struct EraseVoidPtr{
  using type = erased_t< SignatureT >;

  template< typename ActualT >
  struct Reverse{ 
  private:
    using noref_T = std::remove_reference_t<SignatureT>;
    using erased = erased_t< SignatureT >;
  public:
    static decltype(auto) apply( erased data ){
      using cv_T = copy_cv_t< noref_T, std::decay_t<ActualT> >;
      using ref_T = copy_ref_t< SignatureT, cv_T >;
      return static_cast<ref_T&&>(*reinterpret_cast<cv_T*>(data));
    }
    
    template< typename U >
    static decltype(auto) apply( U&& value ){
      return std::forward<U>( value );
    }
  };
};

template< typename Interface >
using Local = LocalVTable< Interface, EraseVoidPtr >;

template< typename Interface >
using Remote = RemoteVTable< Interface, EraseVoidPtr >;

#endif // __VTABLE_HPP__

