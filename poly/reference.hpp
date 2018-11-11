#ifndef __REFERENCE_HPP__
#define __REFERENCE_HPP__

#include "pointer.hpp"


namespace poly {

template< typename VTable >
class Reference
  : protected Pointer<VTable>
{
private:
  using base = Pointer<VTable>;
protected:
    // poly::Value can be implicitly cast to Reference& and we can't allow meddling with its insides
  Reference& operator=( const Reference&  other )          = default;
  Reference& operator=(       Reference&& other ) noexcept = default;
  
public:
  using implementation = typename base::implementation;
  using interface      = typename base::interface;
  using pointer_type = typename base::pointer_type;


  friend unwrap_<                base&  > operator*(                Reference&  ref ){ return {static_cast<base&>(ref)}; }
  friend unwrap_< const          base&  > operator*( const          Reference&  ref ){ return {static_cast<const base&>(ref)}; }
  friend unwrap_<                base&& > operator*(                Reference&& ref ){ return {static_cast<base&&>(ref)}; }
  friend unwrap_< const          base&& > operator*( const          Reference&& ref ){ return {static_cast<const base&&>(ref)}; }
  friend unwrap_<       volatile base&  > operator*(       volatile Reference&  ref )
  { return {static_cast<volatile base&>(ref)}; }
  friend unwrap_< const volatile base&  > operator*( const volatile Reference&  ref )
  { return {static_cast<const volatile base&>(ref)}; }
  friend unwrap_<       volatile base&& > operator*(       volatile Reference&& ref )
  { return {static_cast<volatile base&&>(ref)}; }
  friend unwrap_< const volatile base&& > operator*( const volatile Reference&& ref )
  { return {static_cast<const volatile base&&>(ref)}; }
  
  // using base::operator[];
  // using base::get;
  // using base::call;
  // using base::operator*;
  
  const implementation& vtable()  const { return this->base::vtable(); }
  // No pointer reassignment
  pointer_type          address() const { return this->base::value(); }


  template< typename T, typename = disable_if_same_or_derived< Reference, T > >
  Reference( T& v )
    : base( &v )
  {  }

  Reference( const Reference& ) = default;
  Reference( Reference&& ) noexcept = default;

  template< typename OtherImplementation >
  Reference( const Reference< OtherImplementation >& other )
    : base( static_cast<const Pointer<OtherImplementation>&>(other) )
  {  }

  template< typename OtherImpl >
  friend class Reference;
};



}

#endif // __REFERENCE_HPP__

