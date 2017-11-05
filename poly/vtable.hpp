#ifndef __VTABLE_HPP__
#define __VTABLE_HPP__

#include "thunk.hpp"

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
  template< typename T >
  static Thunk make(){
    using Type = std::conditional_t<
      std::is_const<Tag>::value,
      std::add_const_t<T>,
      std::remove_const_t<T>
      >;
    return { get_thunk<Tag, Type, os>()... };
  };

  template< typename Sig >
  thunk_type< Sig > get() const{
    return std::get< thunk_type< Sig > >( thunks_ );
  }
};

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
  
  template< typename Tag, typename Signature >
  thunk_type< Signature > get() const {
    return std::get< Thunk<Tag> >( thunks_ ).template get<Signature>();
  }
};


template< typename... Tags >
class Remote{
private:
  using VTable = Local<Tags...>;
  
  VTable& vtable_;

  Remote( VTable& vtbl )
    : vtable_( vtbl )
  {  };
  
public:
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

template< typename Invoker, typename VTable, typename... Args >
decltype(auto) call( const VTable& vtbl, Args&&... args ){
  using Signature = decltype( unerase_signature< Invoker >( std::forward<Args>(args)... ) );
  return (*vtbl.template get<Invoker, Signature>())( std::forward<Args>(args)... );
}

#endif // __VTABLE_HPP__

