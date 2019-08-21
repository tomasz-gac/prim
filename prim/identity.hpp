#ifndef __PRIM_IDENTITY_HPP__
#define __PRIM_IDENTITY_HPP__

#include "value.hpp"
#include "allocator.hpp"
#include "vtable/vtable.hpp"

namespace prim{

template< typename T, typename Interface, typename Alloc = HeapAllocator >
class identity
  : public value< NoVT<Interface, T>, Alloc >
{
private:
  using base = value< NoVT<Interface, T>, Alloc >;
public:
  template< typename U,
	    typename = std::enable_if_t< std::is_same<T,std::decay_t<U>>::value > >
  identity( U&& v )
    : base( in_place<T>(), std::forward<U>(v) )
  {  }

  identity( const identity&  ) = default;
  identity(       identity&& ) = default;

  identity& operator=( const identity&  ) = default;
  identity& operator=(       identity&& ) = default;

  const T& get() const {
    return *reinterpret_cast<const T*>(this->base::reference_t::value().data);
  }
  T& get() {
    return *reinterpret_cast<T*>(this->base::reference_t::value().data);
  }

        T* operator->()       { return &this->get(); }
  const T* operator->() const { return &this->get(); }

  template< typename I, typename A >
  friend class value;
};

}
#endif //__PRIM_IDENTITY_HPP__
