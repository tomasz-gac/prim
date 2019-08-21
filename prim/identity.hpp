#ifndef __PRIM_IDENTITY_HPP__
#define __PRIM_IDENTITY_HPP__

#include "value.hpp"
#include "allocator.hpp"
#include "vtable/vtable.hpp"

template< typename T, typename Interface, typename Alloc = prim::HeapAllocator >
class identity
  : public prim::value< prim::NoVT<Interface, T>, Alloc >
{
private:
  using base = prim::value< prim::NoVT<Interface, T>, Alloc >;
public:
  template< typename U,
	    typename = std::enable_if_t< std::is_same<T,std::decay_t<U>>::value > >
  identity( U&& v )
    : base( in_place<T>(), std::forward<U>(v) )
  {  }

  identity( const identity&  ) = default;
  identity(       identity&& ) = default;

  const identity& operator=( const identity&  ) = default;
        identity& operator=(       identity&& ) = default;

  const T& get() const { return *reinterpret_cast<const T*>(this->value()); }
        T& get()       { return *reinterpret_cast<      T*>(this->value()); }
};

#endif //__PRIM_IDENTITY_HPP__
