#ifndef __THUNK_HPP__
#define __THUNK_HPP__

#include "invoker.hpp"
#include "../helpers.hpp"
#include <exception>

struct Invalid{  };

class invalid_vtable_call
  : public std::exception
{
private:
  std::string tag_;
  
public:
  virtual const char* what() const noexcept override {
    return (std::string() + "Function call to " + tag_ + " in purposefully invalid vtable ").c_str();
  }

  invalid_vtable_call( const std::string& tag_name )
    : tag_( tag_name )
  {  }
};

namespace impl__{
  template< typename Invoker, template< typename > class Transform >
  struct thunk_impl_;

  template< typename Tag, typename Return, typename... Args,
	    template< typename > class Transform >
  struct thunk_impl_< Invoker< Tag, Return(Args...) >, Transform >{

    template< typename T >
    static typename std::enable_if< !std::is_same<T, Invalid>::value,
				    Return >::type
    thunk( typename Transform<Args>::type... args ) {
      static_assert( std::is_empty< Tag >::value, "Tags may have no data members" );
      return invoke( Tag(), Transform<Args>::template Reverse<T>::apply
		     ( static_cast<typename Transform<Args>::type&&>(args) )... );
    }

    template< typename T >
    static typename std::enable_if< std::is_same<T, Invalid>::value,
				    Return >::type
    thunk( typename Transform<Args>::type... args ) {
      throw invalid_vtable_call( T2Str( Tag() ) );
    }
  };
  
  template< typename Invoker, template< typename > class Transform >
  struct thunk_type;

  template< typename Tag, typename Return, typename... Args, template< typename > class Transform >
  struct thunk_type< Invoker< Tag, Return(Args...) >, Transform >{
    using type = Return (*)( typename Transform<Args>::type... );
    
    template< typename T >
    static type get_thunk(){
      return &thunk_impl_< Invoker<Tag, Return(Args...)>, Transform >::template thunk<T>;
    }
  };
}

// returns a type of thunk given Invoker and Transform
template< typename S, template< typename > class Transform >
using thunk_type = typename impl__::thunk_type<S, Transform>::type;

// returns a Thunk given Invoker, Transform and a seed type T
template< typename Invoker, template< typename > class Transform, typename T >
thunk_type<Invoker, Transform > get_thunk(){
  return impl__::thunk_type<Invoker, Transform >::template get_thunk<T>();
}

// Helper struct signifying that an invoker cannot be called with supplied arguments
struct invalid_arguments;

namespace impl__{
  template< template< typename > class Transform, typename overloads >
  struct unerase_signature;

  template< template< typename > class Transform,
	    typename Tag, typename Return, typename... Args, typename... rest
	    >
  struct unerase_signature< Transform, overloads< Invoker< Tag, Return(Args...)>, rest... > >
    : unerase_signature< Transform, overloads< rest... > >
  {
    using unerase_signature< Transform, overloads< rest... > >::get;
  
    static Invoker< Tag, Return(Args...)> get( typename Transform<Args>::type... );
  };

  template< template< typename > class Transform >
  struct unerase_signature< Transform, overloads<> >
  {
    static invalid_arguments get( ... );
  };
}

// Selects Invoker unerased signature that matches erased Args given supplied Transform
template< typename Invoker, template< typename > class Transform, typename... Args >
using unerase_signature =
  decltype( impl__::unerase_signature< Transform, overloads_t<Invoker> >::get( std::declval<Args>()... ) );

// Thunk of a tag that contains function pointers for all overloads
template< typename Tag, template< typename > class Transform >
class Thunk
{
private:
  template< typename Invoker >
  using Tthunk_type = thunk_type< Invoker, Transform >;

  using thunk_tuple = repack_t< map_t< overloads_t<Tag>, Tthunk_type >, std::tuple<> >;

  Thunk( thunk_tuple thunks )
    : thunks_( std::move( thunks ) )
  {  }

  thunk_tuple thunks_;

  template< typename T, typename... Overloads >
  static Thunk make_impl( overloads< Overloads... >* ){
    return { std::make_tuple( get_thunk<Overloads, Transform, T>()... ) };
  };
  
public:
  // Make thunk for type T
  template< typename T >
  static Thunk make(){
    // for Overloads... parameter pack
    return make_impl<T>(static_cast< overloads_t<Tag>* >(nullptr));
  };

  template< typename... Args >
  decltype(auto) operator()( Args&&... args ) const {
    using Signature = unerase_signature< Tag, Transform, Args&&... >;
    static_assert( !std::is_same< Signature, invalid_arguments >::value,
		   "Invoker cannot be called with supplied arguments" );
    return (*std::get< thunk_type< Signature, Transform > >(thunks_))( std::forward<Args>(args)... );
  }
};


#endif // __THUNK_HPP__
