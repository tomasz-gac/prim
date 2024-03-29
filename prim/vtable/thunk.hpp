#ifndef __THUNK_HPP__
#define __THUNK_HPP__

#include <tuple>

#include "get_thunk.hpp"


namespace prim{

// Thunk of a tag that contains function pointers for all overloads
template< typename Tag, typename erased_t >
class Thunk
{
public:
  using erased_type = erased_t;
  
  template< typename Invoker >
  using Tthunk_type = thunk_type< Invoker, erased_type >;

  using thunk_tuple = tl::repack_t< tl::map_t< overloads_t<Tag>, Tthunk_type >, std::tuple<> >;
private:
  Thunk( thunk_tuple thunks )
    : thunks_( std::move( thunks ) )
  {  }

  thunk_tuple thunks_;

  template< typename T, typename... Overloads >
  static Thunk make_impl( overloads< Overloads... >* ){
    return { std::make_tuple( get_thunk<Overloads, erased_type, T>()... ) };
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
    using Signature = unerase_signature< Tag, erased_type, Args&&... >;
    static_assert( !std::is_same< Signature, invalid_arguments >::value,
		   "Invoker cannot be called with supplied arguments" );
    return (*std::get< thunk_type< Signature, erased_type > >(thunks_))( std::forward<Args>(args)... );
  }

  bool operator==( const Thunk& other ) const {
    return thunks_ == other.thunks_;
  }

  bool operator!=( const Thunk& other ) const {
    return thunks_ != other.thunks_;
  }
};
  
}

#endif // __THUNK_HPP__
