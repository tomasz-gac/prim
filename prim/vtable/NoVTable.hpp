#ifndef __NO_VTABLE_HPP__
#define __NO_VTABLE_HPP__

#include <cassert>

#include "thunk.hpp"

namespace prim{

template< typename Interface, typename erased_t, typename T >
class NoVTable
{
public:
  using interface = interface_t<Interface>;
  using erased_type = erased_t;

private:
  
  NoVTable() = default;

public:
  template< typename U >
  static NoVTable make(){
    static_assert( !std::is_same<T,U>::value || std::is_same<T, Invalid>::value,
		   "Requested type is not supported by NoVTable.");
    
    return {};
  }

  // Get the thunk based on tag
  template< typename Invoker >
  auto get() const {
    return [this]( auto&&... args ) -> decltype(auto){
      return call<Invoker>( std::forward<decltype(args)>(args)... );
    };
  }

  template< typename Invoker >
  auto operator[]( const Invoker& ) const {
    return get< Invoker >();
  }

  NoVTable( const NoVTable& )           = default;
  NoVTable(       NoVTable&& ) noexcept = default;

  NoVTable& operator=( const NoVTable& ) = default;
  NoVTable& operator=(       NoVTable&& ) noexcept = default;

  bool operator==( const NoVTable& other ) const { return true; }
  bool operator!=( const NoVTable& other ) const { return false; }
private:

  template< typename Invoker, typename... Args >
  static decltype(auto) call( Args&&... args )
  {
    using Signature = unerase_signature< Invoker, erased_type, Args&&... >;
    static_assert( !std::is_same< Signature, invalid_arguments >::value,
    		   "Invoker cannot be called with supplied arguments" );
    return (*get_thunk<Signature, erased_type, T>())( std::forward<Args>(args)... );
  }

};

}

#endif // __NO_VTABLE_HPP__
