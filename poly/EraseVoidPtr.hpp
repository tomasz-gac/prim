#ifndef __ERASE_VOID_PTR_HPP__
#define __ERASE_VOID_PTR_HPP__

#include "placeholder.hpp"


template< typename SigT >
using erased_t =
  std::conditional_t<
    is_placeholder< SigT >::value
  , copy_cv_ref_t< SigT, void* >
  , SigT>;

template< typename SignatureT >
struct EraseVoidPtr{
  using type = erased_t< SignatureT >;

  template< typename ActualT >
  struct Reverse{ 
  private:
    using noref_T = std::remove_reference_t<SignatureT>;
    using erased = erased_t< SignatureT >;
  public:
    // Given ActualT unerase data and apply cv-ref qualifiers if SignatureT is a placeholder
    template< typename = std::enable_if_t< is_placeholder<SignatureT>::value> >
    static decltype(auto) apply( erased data ){
      using cv_T = copy_cv_t< noref_T, std::decay_t<ActualT> >;
      using ref_T = copy_ref_t< SignatureT, cv_T >;
      return static_cast<ref_T&&>(*reinterpret_cast<cv_T*>(data));
    }
    // Forwards if data is not erased
    template< typename U >
    static decltype(auto) apply( U&& value ){
      return std::forward<U>( value );
    }
  };
};

#endif // __ERASE_VOID_PTR_HPP__
