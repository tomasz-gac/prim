#ifndef __POLY_HPP__
#define __POLY_HPP__

#include "allocator.hpp"
#include "view.hpp"
#include "builtins.hpp"

template< typename Implementation, typename Allocator, bool enable_copy, bool enable_move >
struct poly_construct;

template< typename T >
struct in_place{  };

struct poly_cast_tag {};

template< typename Impl, typename Alloc >
using poly_base = poly_construct< Impl, Alloc,
				  supports< interface_t<Impl>, copy>(),
				  supports< interface_t<Impl>, move>() ||
				  supports< interface_t<Impl>, move_noexcept>()>;


template< typename Impl, typename Alloc = HeapAllocator >
class Poly
  : private poly_base< Impl, Alloc >
{
public:
  using implementation = impl_t<Impl>;
  using interface = interface_t<implementation>;
private:
  using base = poly_base< Impl, Alloc >;

  Poly( in_place<Invalid> i )
    : base( i )
  {  }  
public:
  using base::operator[];
  using base::get;
  using base::call;
  using base::valueless_by_exception;
  
  template< typename T, typename... Args >
  Poly( in_place<T> p, Args&&... args )
    : base( p, std::forward<Args>(args)... )
  {  }

  Poly( const Poly&  )
    noexcept(std::is_nothrow_copy_constructible<base>::value) = default;
  Poly(       Poly&& )
    noexcept(std::is_nothrow_move_constructible<base>::value) = default;
  
  template< typename Alloc_ >
  Poly( const Poly< Impl, Alloc_ >& other  )
    noexcept(std::is_nothrow_constructible<base, const Poly< Impl, Alloc_ >&>::value)
    : base( poly_cast_tag(), other )
  {  }

  template< typename Alloc_ >
  Poly(       Poly< Impl, Alloc_ >&& other )
    noexcept(std::is_nothrow_constructible<base, Poly< Impl, Alloc_ >&&>::value)
    : base( poly_cast_tag(), std::move(other) )
  {  }

  Poly& operator=( const Poly& other )
    noexcept(std::is_nothrow_copy_assignable<base>::value) = default;
  
  Poly& operator=( Poly&& other )
    noexcept(std::is_nothrow_move_assignable<base>::value) = default;

  template< typename Alloc_ >
  Poly& operator=( const Poly<Impl, Alloc_>& other )
    noexcept(std::is_nothrow_assignable<base, const Poly<Impl,Alloc_>&>::value)
  {
    static_cast<base&>(*this) = static_cast<const typename Poly<Impl,Alloc>::base&>(other);
    return *this;
  }

  template< typename Alloc_ >
  Poly& operator=(        Poly<Impl, Alloc_>&& other )
    noexcept(std::is_nothrow_assignable<base, Poly<Impl,Alloc_>&&>::value)
  {
    static_cast<base&>(*this) = static_cast<typename Poly<Impl,Alloc>::base&&>(other);
    return *this;
  }

  template< typename, typename >
  friend class Poly;
};

template< typename Impl, typename Alloc>
class poly_construct_impl
  : public Alloc, public View< Impl >
{
private:
  using View_t = View< Impl >;

  static constexpr bool move_is_noexcept = 
    supports< interface_t<Impl>, move_noexcept>();

   using move_tag = typename std::conditional< move_is_noexcept, move_noexcept, move >::type;

public:
  template< typename T, typename... Args >
  poly_construct_impl( in_place<T>, Args&&... args )
    : Alloc()
    , View_t( this->allocate_construct<T>( std::forward<Args>(args)... ) )
  {  }

  poly_construct_impl( poly_construct_impl&& other ) noexcept( move_is_noexcept )
    : poly_construct_impl( poly_cast_tag(), std::move(other) )
  {  } 

  poly_construct_impl( const poly_construct_impl& other )
    : poly_construct_impl( poly_cast_tag(), other)
  {  }

  template< typename Alloc_ >
  poly_construct_impl( poly_cast_tag, const poly_construct_impl<Impl, Alloc_>& other  )
    : Alloc(), View_t( static_cast<const View_t&>(other) )
  {
    this->construct_from<copy>( other );
  }

  
  template< typename Alloc_ >
  poly_construct_impl( poly_cast_tag, poly_construct_impl<Impl, Alloc_>&& other  )
    noexcept( move_is_noexcept )
  : Alloc(), View_t( static_cast<View_t&&>(other) )
  {
    this->construct_from<move_tag>( std::move(other) );
  }

  template< typename Alloc_ >
  poly_construct_impl& operator=( const poly_construct_impl<Impl, Alloc_>& other ){
    this->assign_impl<copy>( other );
    return *this;
  }

  template< typename Alloc_ >
  poly_construct_impl& operator=(  poly_construct_impl<Impl, Alloc_>&& other )
    noexcept( move_is_noexcept )
  {
    this->assign_impl<move_tag>( other );
    return *this;
  }

  poly_construct_impl& operator=( const poly_construct_impl& other ){
    this->assign_impl<copy>( other );
    return *this;
  }

  poly_construct_impl& operator=( poly_construct_impl&& other )
    noexcept( move_is_noexcept )
  {
    this->assign_impl<move_tag>( other );
    return *this;
  }

  template< typename I, typename A >
  friend class poly_construct_impl;

  ~poly_construct_impl(){ this->reset(); }

  bool valueless_by_exception() const {
    try{
      this->template call< storage >();
      return false;
    } catch ( const invalid_vtable_call& e ){
      return true;
    }
  }
private:

  template< typename operation, typename Other >
  void assign_impl( Other&& other ){
    this->reset();
    static_cast<View_t&>(*this) = std::forward<Other>(other);
    this->construct_from<operation>(std::forward<Other>(other)); 
  }

  template< typename operation, typename Other >
  void construct_from( Other&& other ){
    void* ptr = nullptr;
    try{ 
      auto info = other.template call<storage>();      
      ptr = this->allocate( info );
      other.template call<operation>( ptr );
      this->data_ = { ptr };
    } catch ( const invalid_vtable_call& e ) {
      invalidate();
    }
    catch(...){
      if( ptr != nullptr )
	this->deallocate( ptr );
      invalidate();
      throw;
    }
  }
  
  template< typename T, typename... Args >
  View_t allocate_construct( Args&&... args ){
    auto ptr = this->allocate( storage_info::template get<T>() );
    try{
      new (ptr) T(std::forward<Args>(args)...);
      return View_t( *reinterpret_cast<T*>(ptr) );
    } catch(...){
      this->deallocate( ptr );
      invalidate();
      throw;
    }
  }

  void reset(){
    try{
      this->View_t::template call<destroy>();
    } catch ( const invalid_vtable_call& e ){
      Invalid& contents = static_cast<View_t&>(*this).template cast<Invalid&>();
      contents.~Invalid();
    }
    this->Alloc::deallocate( this->data_.data );
  }

  void invalidate(){
    static_cast<View_t&>(*this) = allocate_construct<Invalid>();
  }
};

template< typename Impl, typename Alloc>
struct poly_construct< Impl, Alloc, true, true >
  : poly_construct_impl< Impl, Alloc >
{
private:
  using base = poly_construct_impl< Impl, Alloc >;
  
  template< typename Alloc_ >
  using other_base = poly_construct_impl< Impl, Alloc_ >;
public:
  template< typename T, typename... Args >
  poly_construct( in_place<T> p, Args&&... args )
    : base( p, std::forward<Args>(args)... )
  {  }
 
  poly_construct( const poly_construct& other )
    noexcept(std::is_nothrow_copy_constructible<base>::value) = default;
  poly_construct( poly_construct&& other )
    noexcept(std::is_nothrow_move_constructible<base>::value) = default;
  
  poly_construct& operator=( const poly_construct&  )
    noexcept(std::is_nothrow_copy_assignable<base>::value) = default;
  poly_construct& operator=(       poly_construct&& )
    noexcept(std::is_nothrow_move_assignable<base>::value) = default;
  
  template< typename Alloc_ >
  poly_construct( poly_cast_tag, const poly_construct< Impl, Alloc_, true, true >& other )
    noexcept(std::is_nothrow_constructible<base, const other_base<Alloc_>& >::value)
    : base( poly_cast_tag(), other )
  {  }
  template< typename Alloc_ >
  poly_construct( poly_cast_tag, poly_construct< Impl, Alloc_, true, true >&& other )
    noexcept(std::is_nothrow_constructible<base, other_base<Alloc_>&& >::value)
  : base( poly_cast_tag(), std::move(other) )
  {  }
};

template< typename Impl, typename Alloc>
struct poly_construct< Impl, Alloc, false, true >
  : poly_construct_impl< Impl, Alloc >
{
private:
  using base = poly_construct_impl< Impl, Alloc >;

  template< typename Alloc_ >
  using other_base = poly_construct_impl< Impl, Alloc_ >;
public:
  template< typename T, typename... Args >
  poly_construct( in_place<T> p, Args&&... args )
    : base( p, std::forward<Args>(args)... )
  {  }
 
  poly_construct( const poly_construct& other )
    noexcept(std::is_nothrow_copy_constructible<base>::value) = delete;
  poly_construct( poly_construct&& other )
    noexcept(std::is_nothrow_move_constructible<base>::value) = default;

  poly_construct& operator=( const poly_construct&  )
    noexcept(std::is_nothrow_copy_assignable<base>::value) = delete;
  poly_construct& operator=(       poly_construct&& )
    noexcept(std::is_nothrow_move_assignable<base>::value) = default;
  
  template< typename Alloc_ >
  poly_construct( poly_cast_tag, poly_construct< Impl, Alloc_, false, true >&& other )
    noexcept(std::is_nothrow_constructible<base, other_base<Alloc_>&& >::value)
    : base( poly_cast_tag(), std::move(other) )
  {  }
};

template< typename Impl, typename Alloc>
struct poly_construct< Impl, Alloc, true, false >
  : poly_construct_impl< Impl, Alloc >
{
private:
  using base = poly_construct_impl< Impl, Alloc >;

  template< typename Alloc_ >
  using other_base = poly_construct_impl< Impl, Alloc_ >;
public:
  template< typename T, typename... Args >
  poly_construct( in_place<T> p, Args&&... args )
    : base( p, std::forward<Args>(args)... )
  {  }
 
  poly_construct( const poly_construct& other )
    noexcept(std::is_nothrow_copy_constructible<base>::value) = default;
  poly_construct( poly_construct&& other )
    noexcept(std::is_nothrow_move_constructible<base>::value) = delete;
  
  poly_construct& operator=( const poly_construct&  )
    noexcept(std::is_nothrow_copy_assignable<base>::value) = default;
  poly_construct& operator=(       poly_construct&& )
    noexcept(std::is_nothrow_move_assignable<base>::value) = delete;
  
  template< typename Alloc_ >
  poly_construct( poly_cast_tag, const poly_construct< Impl, Alloc_, true, false >& other )
    noexcept(std::is_nothrow_constructible<base, const other_base<Alloc_>& >::value)    
    : base( poly_cast_tag(), other )
  {  }
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
 
  poly_construct( const poly_construct& other )
    noexcept(std::is_nothrow_copy_constructible<base>::value) = delete;
  poly_construct( poly_construct&& other )
    noexcept(std::is_nothrow_move_constructible<base>::value) = delete;

  poly_construct& operator=( const poly_construct&  )
    noexcept(std::is_nothrow_copy_assignable<base>::value) = delete;
  poly_construct& operator=(       poly_construct&& )
    noexcept(std::is_nothrow_move_assignable<base>::value) = delete;
;
  

};
#endif // __POLY_HPP__

