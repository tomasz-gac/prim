#ifndef __HELPERS_HPP__
#define __HELPERS_HPP__



template< typename T >
void print_type( T&& value ){
  std::cout << (std::is_const<std::remove_reference_t<T&&>>::value ? "const " : "")
	    << (std::is_volatile<std::remove_reference_t<T&&>>::value ? "volatile " : "")
	    << typeid(value).name() 
	    << (std::is_rvalue_reference<T&&>::value ? "&&" : "&") << " == " << value << std::endl;
}

struct print  : Invoker< print, void (const T&) >{  };
struct test   : Invoker< test, void (const int&)
			     , void (const double&) >{  };

template< typename T >
struct as : Invoker< as<T>, T( ::T& ) >{  };

template< typename T >
void invoke( print, const T& value ){
  print_type( std::forward<const T&>(value) );
}


template< typename T >
T& invoke( as<T&>, T& value ){ return value; }

template< typename T, typename U >
T& invoke( as<T&>, U& ){ throw std::bad_cast(); }

template< typename T, typename U >
T invoke( as<T>, const U& value ){ return value; }

template< typename T >
void invoke( test, const T& val ){ std::cout << typeid(T).name() << " " << val << std::endl; }

#endif // __HELPERS_HPP__
