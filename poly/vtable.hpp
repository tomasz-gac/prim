#ifndef __VTABLE_HPP__
#define __VTABLE_HPP__

#include "thunk.hpp"

// Thunk of a tag that contains function pointers for all overloads
template< typename Tag, typename = overloads_t<Tag> >
class Thunk;

template< typename Tag, typename... os  >
class Thunk<Tag, overloads<os...> >
{
private:
  Thunk( thunk_type<os>... ts )
    : thunks_( std::make_tuple( ts... ) )
  {  }

  std::tuple< thunk_type<os>... >  thunks_;
  
public:
  // Make thunk for type T
  template< typename T >
  static Thunk make(){
    // Forwarding const from Tag to T
    using Type =
      std::conditional_t<
	std::is_const<Tag>::value,
        std::add_const_t<T>,
        std::remove_const_t<T>
      >;
    return { get_thunk<Tag, Type, os>()... };
  };
  // Resolve thunk based on signature
  template< typename Sig >
  thunk_type< Sig > get() const{
    return std::get< thunk_type< Sig > >( thunks_ );
  }
};

// VTable that holds thunks locally
template< typename... Tags >
class Local{
private:
  std::tuple< Thunk<Tags>... > thunks_;

  Local( Thunk<Tags>... thunks )
    : thunks_( std::make_tuple( thunks... ) )
  {  }

public:
  template< typename T >
  static Local make(){
    return { Thunk<Tags>::template make<T>() ... };
  }
  // Get a thunk based on tag and signature
  template< typename Tag, typename Signature >
  thunk_type< Signature > get() const {
    return std::get< Thunk<Tag> >( thunks_ ).template get<Signature>();
  }
};

// VTable that holds thunks remotely
template< typename... Tags >
class Remote{
private:
  using VTable = Local<Tags...>;
  // Singleton reference
  VTable& vtable_;

  Remote( VTable& vtbl )
    : vtable_( vtbl )
  {  };
  
public:
  // Makes the VTable for type T using a singleton pattern
  template< typename T >
  static Remote make(){
    static VTable instance = VTable::template make<T>();
    
    return { instance };
  }

  template< typename Tag, typename Signature >
  thunk_type< Signature > get() const {
    return vtable_.template get< Tag, Signature >();
  }
};

// calls thunk with given Args. Selects Signature based on C++ overload resolution
template< typename Invoker, typename VTable, typename... Args >
decltype(auto) call( const VTable& vtbl, Args&&... args ){
  using Signature = unerase_signature< Invoker, Args&&... >;
  return (*vtbl.template get<Invoker, Signature>())( std::forward<Args>(args)... );
}

#endif // __VTABLE_HPP__

