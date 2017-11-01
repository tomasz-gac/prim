#ifndef __PLACEHOLDER_HPP__
#define __PLACEHOLDER_HPP__

struct T{};

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


template< typename SignatureType, bool = std::is_same< ::T, std::remove_cv_t<std::decay_t<SignatureType>>>::value>
struct Eraser;

template< typename SignatureType >
struct Eraser< SignatureType, false >
{
  template< typename T, typename U >
  static decltype(auto) unerase( U&& value ){
    return std::forward<U>( value );
  }

  using erased = SignatureType;
};


template< typename SignatureT >
struct Eraser< SignatureT, true >{
private:
  static_assert( std::is_same< std::decay_t< SignatureT >, ::T >::value );
  using noref_T = std::remove_reference_t<SignatureT>;
public:
  
  using erased = copy_cv_ref_t< SignatureT, void* >;
  
  template< typename T >
  static decltype(auto) unerase( erased data ){
    using cv_T = copy_cv_t< noref_T, std::decay_t<T> >;
    using ref_T = copy_ref_t< SignatureT, cv_T >;
    return static_cast<ref_T&&>(*reinterpret_cast<cv_T*>(data));
  }
};

template< typename T >
using erased_t = typename Eraser<T>::erased;

#endif // __PLACEHOLDER_HPP__
