#ifndef __REFERENCE_HPP__
#define __REFERENCE_HPP__

#include "pointer.hpp"


namespace poly {

template< typename VTable >
class reference
  : protected pointer<VTable>
{
private:
  using base = pointer<VTable>;
protected:
    // poly::Value can be implicitly cast to reference& and we can't allow meddling with its insides
  reference& operator=( const reference&  other )          = default;
  reference& operator=(       reference&& other ) noexcept = default;
  
public:
  using implementation = typename base::implementation;
  using interface      = typename base::interface;
  using erased_type    = typename base::erased_type;


  friend unwrap_<                base&  > operator*(                reference&  ref ){ return {static_cast<base&>(ref)}; }
  friend unwrap_< const          base&  > operator*( const          reference&  ref ){ return {static_cast<const base&>(ref)}; }
  friend unwrap_<                base&& > operator*(                reference&& ref ){ return {static_cast<base&&>(ref)}; }
  friend unwrap_< const          base&& > operator*( const          reference&& ref ){ return {static_cast<const base&&>(ref)}; }
  friend unwrap_<       volatile base&  > operator*(       volatile reference&  ref )
  { return {static_cast<volatile base&>(ref)}; }
  friend unwrap_< const volatile base&  > operator*( const volatile reference&  ref )
  { return {static_cast<const volatile base&>(ref)}; }
  friend unwrap_<       volatile base&& > operator*(       volatile reference&& ref )
  { return {static_cast<volatile base&&>(ref)}; }
  friend unwrap_< const volatile base&& > operator*( const volatile reference&& ref )
  { return {static_cast<const volatile base&&>(ref)}; }
  
  const implementation& vtable()  const { return this->base::vtable(); }
  // No pointer reassignment
  erased_type           address() const { return this->base::value(); }


  template< typename T, typename = disable_if_same_or_derived< reference, T > >
  reference( T& v )
    : base( &v )
  {  }

  reference( const reference& ) = default;
  reference( reference&& ) noexcept = default;

  template< typename OtherImplementation >
  reference( const reference< OtherImplementation >& other )
    : base( static_cast<const pointer<OtherImplementation>&>(other) )
  {  }

  template< typename OtherImpl >
  friend class reference;
};

}

#endif // __REFERENCE_HPP__

