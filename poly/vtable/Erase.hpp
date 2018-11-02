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

template<
  typename SignatureT,
  typename Ptr,
  bool isPlaceholder
> struct Erase_impl;

template< typename SignatureT, typename Ptr = void* >
using Erase = Erase_impl< SignatureT, Ptr, is_placeholder<SignatureT>::value >;

  // Version for placeholders
template< typename SignatureT >
struct Erase_impl< SignatureT, void*, true >{
  using pointer_type = void*;
  using type = Erased< pointer_type >;

  template< typename ActualT >
  struct Reverse{
  private:
    using noref_SignatureT = std::remove_reference_t<SignatureT>;

  public:
    // Given ActualT unerase data and apply cv-ref qualifiers
    static decltype(auto) apply( Erased<pointer_type> data ){
      using cv_ActualT = copy_cv_t< noref_SignatureT, std::decay_t<ActualT> >;
      using cv_ref_ActualT = copy_ref_t< SignatureT, cv_ActualT >;
      return static_cast<cv_ref_ActualT&&>(*reinterpret_cast<cv_ActualT*>(data.data));
    }
  };

  template< typename T >
  static Erased<pointer_type> apply( T* ptr ){
    return { reinterpret_cast<void*>(ptr) };
  }
};
  // version for non-placeholders  
template< typename SignatureT >
struct Erase_impl< SignatureT, void*, false >{

  using pointer_type = void*;
  using type = SignatureT;

  template< typename ActualT >
  struct Reverse{ 
    // Forwards if data is not erased
    template< typename U >
    static decltype(auto) apply( U&& value ){
      return std::forward<U>( value );
    }
  };

  template< typename T >
  static Erased<pointer_type> apply( T* ptr ){
    return { reinterpret_cast<void*>(ptr) };
  }
};

template< typename T, typename PtrT >
decltype(auto) cast_erased( Erased<PtrT> erased  ){
  using SigT = copy_cv_ref_t< T, poly::T >;
  
  return Erase<SigT>::template Reverse<T>::apply( erased );
}  
  

  // namespace test__
  // {

  // template< typename T, typename ptr_t = void* >
  // void test_cast_erased(){
  //   using return_t = decltype( cast_erased<T>( Erased<ptr_t>{ nullptr } ) );
  //   constexpr bool is_ref = std::is_reference< T >::value;
  //   static_assert( is_ref && std::is_same< return_t, T >::value ||
  // 		  !is_ref && std::is_same< return_t, std::add_rvalue_reference_t<T> >::value,
  // 		   "Test_cast_erased failed" );
  // }
  
  
  //   template void test_cast_erased<int, void*>();
  //   template void test_cast_erased<int&, void*>();
  //   template void test_cast_erased<int&&, void*>();
  //   template void test_cast_erased<const int&, void*>();
  //   template void test_cast_erased<const int&&, void*>();
  //   template void test_cast_erased<volatile int, void*>();
  //   template void test_cast_erased<volatile int&, void*>();
  //   template void test_cast_erased<volatile int&&, void*>();
  //   template void test_cast_erased<const volatile int&, void*>();
  //   template void test_cast_erased<const volatile int&&, void*>();
  // }
}
#endif // __ERASE_HPP__
