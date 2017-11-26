#ifndef __VTABLE_HPP__
#define __VTABLE_HPP__

#include "invoker.hpp"
#include "EraseVoidPtr.hpp"
#include "LocalVTable.hpp"
#include "RemoteVTable.hpp"

template< typename T >
using impl_t =  typename T::implementation;

template< typename... Policies >
class VTable
{
public:
  using interface = join_t< interface_t<Policies>... >;
private:
  using tuple_type =   std::tuple< impl_t<Policies>... >;
  tuple_type vtables_;

  VTable( tuple_type tuple )
    : vtables_( std::move(tuple) )
  {  }

  template< typename... Ps >
  friend class VTable;
public:
  template< typename T >
  static VTable make(){
    return { std::make_tuple( impl_t<Policies>::template make<T>() ... ) };
  };

  template< typename... Ps >
  explicit operator VTable<Ps...>() const {
    return VTable<Ps...>( std::make_tuple( std::get< impl_t<Ps> >(vtables_)... ) );
  };

  template< typename Tag >
  decltype(auto) operator[]( const Tag& ) const {
    return get< Tag >();
  }

  template< typename Tag >
  decltype(auto) get() const {
    return get<Tag>( std::make_index_sequence< 1 + sizeof...(Policies) >(),
		     std::integer_sequence< bool,
		     supports< interface_t<Policies>, Tag >()...
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
  template< typename Tag, bool alwaysFalse = false >
  decltype(auto) get( std::index_sequence<>, std::integer_sequence<bool> ){
    static_assert( alwaysFalse, "VTable does not support Tag" );
  }
};

template< typename Interface >
struct Local{
  using implementation = LocalVTable< Interface, EraseVoidPtr >;
  using interface = Interface;
};

template< typename Interface >
struct Remote{
  using implementation = RemoteVTable< Interface, EraseVoidPtr >;
  using interface = Interface;  
};

template< typename Interface >
Local< Interface > local( Interface ){ return {}; };

template< typename Interface >
Remote< Interface > remote( Interface ){ return {}; };

template< typename... Ts, bool AlwaysFalse = false > 
VTable< Ts...> vtable( Ts... ){
  static_assert( AlwaysFalse, "Function used for type deduction only. Use with decltype" );
};

#endif // __VTABLE_HPP__

