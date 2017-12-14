#ifndef __POLY_HPP__
#define __POLY_HPP__

#include "allocator.hpp"
#include "view.hpp"
#include "builtins.hpp"

template< typename Implementation, typename Allocator, bool enable_copy, bool enable_move >
struct poly_construct;

template< typename T >
struct in_place{  };

template< typename Impl, typename Alloc = HeapAllocator >
class Poly
  : private poly_construct< Impl, Alloc,
			    supports< interface_t<Impl>, copy, storage >(),
			    supports< interface_t<Impl>, move, storage >()>
{
public:
  using implementation = impl_t<Impl>;
  using interface = interface_t<implementation>;
private:
  using base = poly_construct< Impl, Alloc,
			       supports< interface_t<Impl>, copy, storage >(),
			       supports< interface_t<Impl>, move, storage >()>;
public:
  using base::operator[];
  using base::get;
  using base::call;
  
  template< typename T, typename... Args >
  Poly( in_place<T> p, Args&&... args )
    : base( p, std::forward<Args>(args)... )
  {  }

  Poly( const Poly&  other ) : base( other ){ std::cout << "copy" << std::endl; }
  Poly(       Poly&& other ) : base( std::move(other) ){ std::cout << "move" << std::endl; }

  ~Poly(){
    this->template call< destroy >();
    static_cast<Alloc&>(*this).deallocate( this->data_.data );
  }
};

template< typename Allocator, typename F >
decltype(auto) allocate( Allocator& alloc, storage_info info, F f )
{
  auto ptr = alloc.allocate( info );
  try{
    return f(ptr);
  } catch(...){
    alloc.deallocate( ptr );
    throw;
  }
}

template< typename Impl, typename Alloc>
struct poly_construct_impl
  : Alloc, View< Impl >
{
  using View_t = View< Impl >;
  
  template< typename T, typename... Args >
  poly_construct_impl( in_place<T>, Args&&... args )
    : Alloc()
    , View_t( allocate( *this, storage_info::get<T>(),
			[&args...]( void* ptr ){
			  new (ptr) T(std::forward<Args>(args)...);
			  return View_t{ *reinterpret_cast<T*>(ptr) };
			} ))
  {  }

  poly_construct_impl( const poly_construct_impl& other  )
    : Alloc()
    , View_t( allocate( *this, other.template call< storage >(),
			[&other]( void* ptr ){
			  other.template call< copy >( ptr );
			  return ptr;
			} ), other.vtable_ )
  { }

  poly_construct_impl( poly_construct_impl&& other  )
    : Alloc()
    , View_t( allocate( *this, other.template call< storage >(),
			[&other]( void* ptr ){
			  other.template call< move >( ptr );
			  return ptr;
			} ), other.vtable_ )
  { }
};

template< typename Impl, typename Alloc>
struct poly_construct< Impl, Alloc, true, true >
  : poly_construct_impl< Impl, Alloc >
{
private:
  using base = poly_construct_impl< Impl, Alloc >;
public:
  template< typename T, typename... Args >
  poly_construct( in_place<T> p, Args&&... args )
    : base( p, std::forward<Args>(args)... )
  {  }
 
  poly_construct( const poly_construct& other ) = default;
  poly_construct( poly_construct&& other ) = default;
};

template< typename Impl, typename Alloc>
struct poly_construct< Impl, Alloc, false, true >
  : poly_construct_impl< Impl, Alloc >
{
private:
  using base = poly_construct_impl< Impl, Alloc >;
public:
  template< typename T, typename... Args >
  poly_construct( in_place<T> p, Args&&... args )
    : base( p, std::forward<Args>(args)... )
  {  }
 
  poly_construct( const poly_construct& other ) = delete;
  poly_construct( poly_construct&& other ) = default;
};

template< typename Impl, typename Alloc>
struct poly_construct< Impl, Alloc, true, false >
  : poly_construct_impl< Impl, Alloc >
{
private:
  using base = poly_construct_impl< Impl, Alloc >;
public:
  template< typename T, typename... Args >
  poly_construct( in_place<T> p, Args&&... args )
    : base( p, std::forward<Args>(args)... )
  {  }
 
  poly_construct( const poly_construct& other ) = default;
  poly_construct( poly_construct&& other ) = delete;
};

template< typename Impl, typename Alloc>
struct poly_construct< Impl, Alloc, false, false >
  : poly_construct_impl< Impl, Alloc >
{
private:
  using base = poly_construct_impl< Impl, Alloc >;
public:
  template< typename T, typename... Args >
  poly_construct( in_place<T> p, Args&&... args )
    : base( p, std::forward<Args>(args)... )
  {  }
 
  poly_construct( const poly_construct& other ) = delete;
  poly_construct( poly_construct&& other ) = delete;
};
#endif // __POLY_HPP__

