#ifndef __PLACEHOLDER_HPP__
#define __PLACEHOLDER_HPP__

namespace poly{

template< size_t N >
struct T_{
  static constexpr size_t index = N;
};

struct wrapper{  };

template< typename T >
struct is_T : std::false_type{  };

template<>
struct is_T<wrapper> : std::true_type{  };

template< size_t N >
struct is_T< T_<N> > : std::true_type{  };

#define DEFINE_T( N )				\
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

}

#endif // __PLACEHOLDER_HPP__
