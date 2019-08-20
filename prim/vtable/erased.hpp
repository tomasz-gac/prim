#ifndef __ERASED_HPP__
#define __ERASED_HPP__

namespace prim{

template< typename Ptr >
struct Erased;
  
template< typename T >
struct Erased< T* >
{
  using value_type = T*;
  value_type data;
  // No universal constructor to prevent Erased< Erased<...> >
  Erased( T* v )
    : data( v )
  {  }
};

template<>
struct Erased< void* >
{
  using value_type = void*;
  value_type data;
  // No universal constructor to prevent Erased< Erased<...> >
  template< typename T >
  Erased( T* v )
    : data( reinterpret_cast<void*>(v) )
  {  }
};

template< typename impl_t >
bool operator==( const Erased<impl_t>& lhs, const Erased<impl_t>& rhs ){
  return lhs.data == rhs.data;
}

template< typename impl_t >
bool operator!=( const Erased<impl_t>& lhs, const Erased<impl_t>& rhs ){
  return lhs.data != rhs.data;
}

}


#endif // __ERASED_HPP__
