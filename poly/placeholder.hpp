#ifndef __PLACEHOLDER_HPP__
#define __PLACEHOLDER_HPP__

template< size_t N >
struct T_{};

template< typename T >
struct is_T : std::false_type{  };

template< size_t N >
struct is_T< T_<N> > : std::true_type{  };

#define DEFINE_T( N ) \
  using T ## N = T_< N >; 

using T = T_<0>;
DEFINE_T(1)
DEFINE_T(2)
DEFINE_T(3)
DEFINE_T(4)
DEFINE_T(5)
DEFINE_T(6)
DEFINE_T(7)
DEFINE_T(8)
DEFINE_T(9)
DEFINE_T(10)

template< typename From, typename To >
struct copy_cv{ using type = std::remove_const_t<std::remove_volatile_t<To>>; };
template< typename From, typename To >
struct copy_cv< const From, To >{ using type = std::add_const_t<std::remove_volatile_t<To>>; };
template< typename From, typename To >
struct copy_cv< volatile From, To >{ using type = std::remove_const_t<std::add_volatile_t<To>>; };
template< typename From, typename To >
struct copy_cv< const volatile From, To >{ using type = std::add_const_t<std::add_volatile_t<To>>; };

template< typename From, typename To >
using copy_cv_t = typename copy_cv<From,To>::type;

template< typename From, typename To >
struct copy_ref{ using type = To; };
template< typename From, typename To >
struct copy_ref< From&, To >{ using type = std::add_lvalue_reference_t<To>; };
template< typename From, typename To >
struct copy_ref< From&&, To >{ using type = std::add_rvalue_reference_t<To>; };

template< typename From, typename To >
using copy_ref_t = typename copy_ref<From,To>::type;

template< typename From, typename To >
using copy_cv_ref_t = copy_ref_t< From, copy_cv_t< std::remove_reference_t<From>, To> >;

template< typename T >
using is_placeholder = is_T<std::remove_cv_t<std::decay_t<T>>>;

template< typename SigT >
using erased_t = std::conditional_t<
  is_placeholder< SigT >::value,
  copy_cv_ref_t< SigT, void* >,
  SigT
>;

template<
  typename T, typename SignatureType,
  bool = is_placeholder<SignatureType>::value
  >
struct Unerase_impl;

template< typename ActualT, typename SignatureType >
struct Unerase_impl< SignatureType, ActualT, false >
{
  template< typename U >
  static decltype(auto) apply( U&& value ){
    return std::forward<U>( value );
  }
};


template< typename ActualT, typename SignatureT >
struct Unerase_impl< ActualT, SignatureT, true >{
private:
  using noref_T = std::remove_reference_t<SignatureT>;
public:
  using erased = erased_t< SignatureT >;
  
  static decltype(auto) apply( erased data ){
    using cv_T = copy_cv_t< noref_T, std::decay_t<ActualT> >;
    using ref_T = copy_ref_t< SignatureT, cv_T >;
    return static_cast<ref_T&&>(*reinterpret_cast<cv_T*>(data));
  }
};

template< typename SignatureT >
struct EraseVoidPtr{
  using type = erased_t< SignatureT >;

  template< typename ActualT >
  using Reverse = Unerase_impl< ActualT, SignatureT >;
};

#endif // __PLACEHOLDER_HPP__
