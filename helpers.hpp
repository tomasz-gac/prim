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
struct assign : Invoker< assign, void ( T&, forward<T> ) > {  };
struct test   : Invoker< test, void (const int&)
			     , void (const double&) >{  };
struct type_id : Invoker< type_id, const std::type_info& ( const T& ) >{};

template< typename T >
struct as : Invoker< as<T>, T( ::T& ) >{  };

template< typename T >
void invoke( print, const T& value ){
  print_type( std::forward<const T&>(value) );
}

template< typename T, typename T2 >
void invoke( assign, T& value, T2&& v ){
  value = std::forward<T2>(v);
}

template< typename T >
T& invoke( as<T&>, T& value ){ return value; }

template< typename T, typename U >
T& invoke( as<T&>, U& ){ throw std::bad_cast(); }

template< typename T, typename U >
T invoke( as<T>, const U& value ){ return value; }

template< typename T >
void invoke( test, const T& val ){ std::cout << typeid(T).name() << " " << val << std::endl; }

template< typename T >
const std::type_info& invoke( type_id, const T& ){ return typeid(T); }

#endif // __HELPERS_HPP__
