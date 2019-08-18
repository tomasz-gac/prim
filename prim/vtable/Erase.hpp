#ifndef __ERASE_HPP__
#define __ERASE_HPP__

#include "../placeholder.hpp"
#include "wrapper_traits.hpp"

namespace prim{

template< typename SignatureT, typename erased_t>
struct Erase{
  static constexpr bool is_placeholder = is_placeholder< SignatureT >::value;  
  // copy_cv_ref_t required for proper overload resolution of parameters
  using type = std::conditional_t< is_placeholder,
				   copy_cv_ref_t< SignatureT, erased_t >,
				   SignatureT >;

  template< typename ActualT, typename T >
  static decltype(auto) reverse( T&& data ){
    return reverse_impl<ActualT>(std::forward<T>(data), std::integral_constant<bool, is_placeholder >()  );
  }

private:
  using noref_SignatureT = std::remove_reference_t<SignatureT>;

  // If SignatureT is placeholder:
  // Unerase data given ActualT and pass to unwrap
  template< typename ActualT >
  static decltype(auto) reverse_impl( erased_t data, std::true_type /*is_placeholder*/ ){
    using cv_ActualT = copy_cv_t< noref_SignatureT, std::decay_t<ActualT> >;
    return unwrap( *reinterpret_cast<cv_ActualT*>(data.data) );
  }

  // Forwards if data is not erased
  template< typename ActualT, typename U >
  static decltype(auto) reverse_impl( U&& value, std::false_type /*is_placeholder*/ ){
    return std::forward<U>( value );
  }

  // Using wrapper_traits get underlying type depending on placeholder's index
  template< typename Wrapper >
  static decltype(auto) unwrap( Wrapper&& multi ){
    static constexpr size_t index = std::decay_t<SignatureT>::index;
    using wrapper_t = std::decay_t<Wrapper>;
    return apply_cv_ref( wrapper_traits<wrapper_t>::template get<index>(multi) );
  };

  // Apply cv qualifiers and forward reference from SignatureT to T
  template< typename T >
  static decltype(auto) apply_cv_ref( T&& v ){
    using cv_ref_ActualT = copy_cv_ref_t< SignatureT, std::decay_t<T&&> >;
    return static_cast< cv_ref_ActualT >( v );
  }
};

}
#endif // __ERASE_HPP__
