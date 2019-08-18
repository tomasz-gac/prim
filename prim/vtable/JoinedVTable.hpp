#ifndef __JOINED_VTABLE_HPP__
#define __JOINED_VTABLE_HPP__

namespace prim{

template< typename... Tags >
class JoinedVTable
{
public:
  using interface = join_t< interface_t<Tags>... >;
private:
  using tuple_type = std::tuple< impl_t<Tags>... >;
  tuple_type vtables_;

  JoinedVTable( tuple_type tuple )
    : vtables_( std::move(tuple) )
  {  }

  template< typename... Ps >
  friend class JoinedJoinedVTable;

public:
  template< typename T >
  static JoinedVTable make(){
    return { std::make_tuple( impl_t<Tags>::template make<T>() ... ) };
  };

  template< typename... Ps >
  explicit operator JoinedVTable<Ps...>() const {
    return JoinedVTable<Ps...>( std::make_tuple( std::get< impl_t<Ps> >(vtables_)... ) );
  };

  template< typename Tag >
  decltype(auto) operator[]( const Tag& ) const {
    return get< Tag >();
  }

  template< typename Tag >
  decltype(auto) get() const {
    return get<Tag>( std::make_index_sequence< 1 + sizeof...(Tags) >(),
		     std::integer_sequence< bool,
		     supports< interface_t<Tags>, Tag >()...
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
    static_assert( alwaysFalse, "JoinedVTable does not support Tag" );
  }
};

}  
#endif // __JOINED_VTABLE_HPP__
