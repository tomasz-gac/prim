#ifndef __ERASE_HPP__
#define __ERASE_HPP__

#include "../placeholder.hpp"

namespace poly{

template< typename Ptr >
struct Erased;

template< typename Ptr >
struct Erased< Ptr* >
{
  using pointer_type = Ptr*;
  pointer_type data;
};

template< typename SignatureT, typename Ptr_t = void*>
struct Erase;
  

template< typename SignatureT >
struct Erase< SignatureT, void* >{
  static constexpr bool is_placeholder = is_placeholder< SignatureT >::value;  
  using pointer_type = void*;
  using type = std::conditional_t< is_placeholder, Erased< pointer_type >, SignatureT >;

  template< typename T >
  static Erased<pointer_type> apply( T* ptr ){
    return { reinterpret_cast<pointer_type>(ptr) };
  }

  template< typename ActualT, typename T >
  static decltype(auto) reverse( T&& data ){
    return reverse_impl<ActualT>(std::forward<T>(data), std::integral_constant<bool, is_placeholder >()  );
  }

private:
  using noref_SignatureT = std::remove_reference_t<SignatureT>;

  // If SignatureT is placeholder:
  // Unerase data given ActualT and apply cv-ref qualifiers
  template< typename ActualT >
  static decltype(auto) reverse_impl( Erased<pointer_type> data, std::true_type /*is_placeholder*/ ){
    using cv_ActualT = copy_cv_t< noref_SignatureT, std::decay_t<ActualT> >;
    using cv_ref_ActualT = copy_ref_t< SignatureT, cv_ActualT >;
    return static_cast<cv_ref_ActualT&&>(*reinterpret_cast<cv_ActualT*>(data.data));
  }

  // Forwards if data is not erased
  template< typename ActualT, typename U >
  static decltype(auto) reverse_impl( U&& value, std::false_type /*is_placeholder*/ ){
    return std::forward<U>( value );
  }
};
  
template< typename T, typename PtrT >
decltype(auto) erased_cast( Erased<PtrT> erased  ){
  using SigT = copy_cv_ref_t< T, poly::T >;
  
  return Erase<SigT, PtrT>::template reverse<T>( erased );
}  
  
template< typename T, typename U >
decltype(auto) erased_cast( U&& data ){
  return std::forward<U>(data);
}  

// #define __POLY_ERASED_STATIC_TEST__
#ifdef __POLY_ERASED_STATIC_TEST__
  namespace test__
  {

  template< typename T, typename ptr_t = void* >
  void test_cast_erased(){
    using return_t = decltype( erased_cast<T>( Erased<ptr_t>{ nullptr } ) );
    constexpr bool is_ref = std::is_reference< T >::value;
    static_assert( is_ref && std::is_same< return_t, T >::value ||
  		  !is_ref && std::is_same< return_t, std::add_rvalue_reference_t<T> >::value,
  		   "Test_cast_erased failed" );
  }
  
  
    template void test_cast_erased<int, void*>();
    template void test_cast_erased<int&, void*>();
    template void test_cast_erased<int&&, void*>();
    template void test_cast_erased<const int&, void*>();
    template void test_cast_erased<const int&&, void*>();
    template void test_cast_erased<volatile int, void*>();
    template void test_cast_erased<volatile int&, void*>();
    template void test_cast_erased<volatile int&&, void*>();
    template void test_cast_erased<const volatile int&, void*>();
    template void test_cast_erased<const volatile int&&, void*>();
  }
#endif // __POLY_ERASED_STATIC_TEST__

}
#endif // __ERASE_HPP__
