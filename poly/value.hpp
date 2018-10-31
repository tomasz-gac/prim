#ifndef __VALUE_HPP__
#define __VALUE_HPP__

#include "allocator.hpp"
#include "view.hpp"
#include "builtins.hpp"

template< typename T >
struct in_place{  };

namespace poly{

template< typename Implementation, typename Allocator, bool enable_copy, bool enable_move >
struct value_construct;

struct value_cast_tag {};

template< typename Impl, typename Alloc >
using value_base = value_construct< Impl, Alloc,
				  supports< interface_t<Impl>, copy>(),
				  supports< interface_t<Impl>, move>() ||
				  supports< interface_t<Impl>, move_noexcept>()>;


template< typename Impl, typename Alloc = HeapAllocator >
class Value
  : private value_base< Impl, Alloc >
{
public:
  using implementation = impl_t<Impl>;
  using interface = interface_t<implementation>;
private:
  using base = value_base< Impl, Alloc >;

  Value( in_place<Invalid> i )
    : base( i )
  {  }  
public:
  using base::operator[];
  using base::get;
  using base::call;
  using base::valueless_by_exception;
  
  template< typename T, typename... Args >
  Value( in_place<T> p, Args&&... args )
    : base( p, std::forward<Args>(args)... )
  {  }

  Value( const Value&  )
    noexcept(std::is_nothrow_copy_constructible<base>::value) = default;
  Value(       Value&& )
    noexcept(std::is_nothrow_move_constructible<base>::value) = default;
  
  template< typename Alloc_ >
  Value( const Value< Impl, Alloc_ >& other  )
    noexcept(std::is_nothrow_constructible<base, const Value< Impl, Alloc_ >&>::value)
    : base( value_cast_tag(), other )
  {  }

  template< typename Alloc_ >
  Value(       Value< Impl, Alloc_ >&& other )
    noexcept(std::is_nothrow_constructible<base, Value< Impl, Alloc_ >&&>::value)
    : base( value_cast_tag(), std::move(other) )
  {  }

  Value& operator=( const Value& other )
    noexcept(std::is_nothrow_copy_assignable<base>::value) = default;
  
  Value& operator=( Value&& other )
    noexcept(std::is_nothrow_move_assignable<base>::value) = default;

  template< typename Alloc_ >
  Value& operator=( const Value<Impl, Alloc_>& other )
    noexcept(std::is_nothrow_assignable<base, const Value<Impl,Alloc_>&>::value)
  {
    static_cast<base&>(*this) = static_cast<const typename Value<Impl,Alloc>::base&>(other);
    return *this;
  }

  template< typename Alloc_ >
  Value& operator=(        Value<Impl, Alloc_>&& other )
    noexcept(std::is_nothrow_assignable<base, Value<Impl,Alloc_>&&>::value)
  {
    static_cast<base&>(*this) = static_cast<typename Value<Impl,Alloc>::base&&>(other);
    return *this;
  }

  template< typename, typename >
  friend class Value;
};

template< typename Impl, typename Alloc>
class value_construct_impl
  : public Alloc, public View< Impl >
{
private:
  using View_t = View< Impl >;

  static constexpr bool move_is_noexcept = 
    supports< interface_t<Impl>, move_noexcept>();

   using move_tag = typename std::conditional< move_is_noexcept, move_noexcept, move >::type;

public:
  // Check if object is in an invalid state
  bool valueless_by_exception() const {
    try{
      this->template call< storage >();
      return false;
    } catch ( const invalid_vtable_call& e ){
      return true;
    }
  }
  
  template< typename T, typename... Args >
  value_construct_impl( in_place<T>, Args&&... args )
    : Alloc()
    , View_t( this->allocate_construct<T>( std::forward<Args>(args)... ) )
  {  }

  value_construct_impl( value_construct_impl&& other ) noexcept( move_is_noexcept )
    : value_construct_impl( value_cast_tag(), std::move(other) )
  {  } 

  value_construct_impl( const value_construct_impl& other )
    : value_construct_impl( value_cast_tag(), other)
  {  }

  template< typename Alloc_ >
  value_construct_impl( value_cast_tag, const value_construct_impl<Impl, Alloc_>& other  )
    : Alloc(), View_t()
  {
    this->construct_from<copy>( other );
  }

  
  template< typename Alloc_ >
  value_construct_impl( value_cast_tag, value_construct_impl<Impl, Alloc_>&& other  )
    noexcept( move_is_noexcept )
    : Alloc(), View_t()
  {
    this->construct_from<move_tag>( std::move(other) );
  }

  ~value_construct_impl(){ this->reset(); }

  template< typename Alloc_ >
  value_construct_impl& operator=( const value_construct_impl<Impl, Alloc_>& other ){
    return this->assign_impl<copy>( other );
  }

  template< typename Alloc_ >
  value_construct_impl& operator=(  value_construct_impl<Impl, Alloc_>&& other )
    noexcept( move_is_noexcept ) {
    return this->assign_impl<move_tag>( std::move(other) );
  }

  value_construct_impl& operator=( const value_construct_impl& other ){
    return this->assign_impl<copy>( other );
  }

  value_construct_impl& operator=( value_construct_impl&& other )
    noexcept( move_is_noexcept ) {
    return this->assign_impl<move_tag>( std::move(other) );
  }

  template< typename I, typename A >
  friend class value_construct_impl;

private:
  //Assigns from other
  template< typename operation, typename Other >
  value_construct_impl& assign_impl( Other&& other ){
    this->reset();
    this->construct_from<operation>(std::forward<Other>(other));
    return *this;
  }

  //Allocates memory for object contained in other
  //Calls operation (copy or move) into allocated memory
  //In case of exception - sets the object into invalid state
  // WARNING : assumes the object to be empty
  template< typename Operation, typename Other >
  void construct_from( Other&& other ){
    void* ptr = nullptr;
    static_cast<View_t&>(*this) = std::forward<Other>(other); // copy or move the vtable
    try{ 
      auto info = other.template call<storage>();      
      ptr = this->allocate( info );
      other.template call<Operation>( ptr );
      this->data_ = { ptr };
    } catch ( const invalid_vtable_call& e ) {
      // other is valueless by exception
      if( ptr != nullptr ){
	this->deallocate( ptr );
      }
      invalidate(); // set as invalid
    } catch(...) {
      // Constructor or Alloc throws
      if( ptr != nullptr ){
	this->deallocate( ptr );
      }
      invalidate();
      throw; // propagate the exception
    }
  }

  //Allocates memory for type T and constructs it using Args
  //In case the T's constructor, or Alloc throws
  //sets the object as invalid and propagates the exception
  // WARNING : assumes the object to be empty
  template< typename T, typename... Args >
  View_t allocate_construct( Args&&... args ){
    void* ptr = nullptr;
    try{
      ptr = this->allocate( storage_info::template get<T>() );
      new (ptr) T(std::forward<Args>(args)...);
      return View_t( *reinterpret_cast<T*>(ptr) );
    } catch(...) {
      if( ptr != nullptr ){
	this->deallocate( ptr );
      }
      invalidate();
      throw; // propagate the exception
    }
  }

  //Destroys the contained object and deallocates memory
  //The class invariant is not maintained after reset!
  //If the contained object's destructor throws - propagate the exception
  void reset(){
    try{
      this->View_t::template call<destroy>();
    } catch ( const invalid_vtable_call& e ){
      //Object is valueless by exception
      //In that case it contains object of type Invalid
      Invalid& contents = static_cast<View_t&>(*this).template cast<Invalid&>();
      contents.~Invalid();
    }
    this->Alloc::deallocate( this->data_.data );
  }
  
  //Function sets the object in an invalid state
  // WARNING : assumes the object to be empty
  void invalidate(){
    static_cast<View_t&>(*this) = allocate_construct<Invalid>();
  }
};

template< typename Impl, typename Alloc>
struct value_construct< Impl, Alloc, true, true >
  : value_construct_impl< Impl, Alloc >
{
private:
  using base = value_construct_impl< Impl, Alloc >;
  
  template< typename Alloc_ >
  using other_base = value_construct_impl< Impl, Alloc_ >;
public:
  template< typename T, typename... Args >
  value_construct( in_place<T> p, Args&&... args )
    : base( p, std::forward<Args>(args)... )
  {  }
 
  value_construct( const value_construct& other )
    noexcept(std::is_nothrow_copy_constructible<base>::value) = default;
  value_construct( value_construct&& other )
    noexcept(std::is_nothrow_move_constructible<base>::value) = default;
  
  value_construct& operator=( const value_construct&  )
    noexcept(std::is_nothrow_copy_assignable<base>::value) = default;
  value_construct& operator=(       value_construct&& )
    noexcept(std::is_nothrow_move_assignable<base>::value) = default;
  
  template< typename Alloc_ >
  value_construct( value_cast_tag, const value_construct< Impl, Alloc_, true, true >& other )
    noexcept(std::is_nothrow_constructible<base, const other_base<Alloc_>& >::value)
    : base( value_cast_tag(), other )
  {  }
  template< typename Alloc_ >
  value_construct( value_cast_tag, value_construct< Impl, Alloc_, true, true >&& other )
    noexcept(std::is_nothrow_constructible<base, other_base<Alloc_>&& >::value)
  : base( value_cast_tag(), std::move(other) )
  {  }
};

template< typename Impl, typename Alloc>
struct value_construct< Impl, Alloc, false, true >
  : value_construct_impl< Impl, Alloc >
{
private:
  using base = value_construct_impl< Impl, Alloc >;

  template< typename Alloc_ >
  using other_base = value_construct_impl< Impl, Alloc_ >;
public:
  template< typename T, typename... Args >
  value_construct( in_place<T> p, Args&&... args )
    : base( p, std::forward<Args>(args)... )
  {  }
 
  value_construct( const value_construct& other )
    noexcept(std::is_nothrow_copy_constructible<base>::value) = delete;
  value_construct( value_construct&& other )
    noexcept(std::is_nothrow_move_constructible<base>::value) = default;

  value_construct& operator=( const value_construct&  )
    noexcept(std::is_nothrow_copy_assignable<base>::value) = delete;
  value_construct& operator=(       value_construct&& )
    noexcept(std::is_nothrow_move_assignable<base>::value) = default;
  
  template< typename Alloc_ >
  value_construct( value_cast_tag, value_construct< Impl, Alloc_, false, true >&& other )
    noexcept(std::is_nothrow_constructible<base, other_base<Alloc_>&& >::value)
    : base( value_cast_tag(), std::move(other) )
  {  }
};

template< typename Impl, typename Alloc>
struct value_construct< Impl, Alloc, true, false >
  : value_construct_impl< Impl, Alloc >
{
private:
  using base = value_construct_impl< Impl, Alloc >;

  template< typename Alloc_ >
  using other_base = value_construct_impl< Impl, Alloc_ >;
public:
  template< typename T, typename... Args >
  value_construct( in_place<T> p, Args&&... args )
    : base( p, std::forward<Args>(args)... )
  {  }
 
  value_construct( const value_construct& other )
    noexcept(std::is_nothrow_copy_constructible<base>::value) = default;
  value_construct( value_construct&& other )
    noexcept(std::is_nothrow_move_constructible<base>::value) = delete;
  
  value_construct& operator=( const value_construct&  )
    noexcept(std::is_nothrow_copy_assignable<base>::value) = default;
  value_construct& operator=(       value_construct&& )
    noexcept(std::is_nothrow_move_assignable<base>::value) = delete;
  
  template< typename Alloc_ >
  value_construct( value_cast_tag, const value_construct< Impl, Alloc_, true, false >& other )
    noexcept(std::is_nothrow_constructible<base, const other_base<Alloc_>& >::value)    
    : base( value_cast_tag(), other )
  {  }
};

template< typename Impl, typename Alloc>
struct value_construct< Impl, Alloc, false, false >
  : value_construct_impl< Impl, Alloc >
{
private:
  using base = value_construct_impl< Impl, Alloc >;
public:
  template< typename T, typename... Args >
  value_construct( in_place<T> p, Args&&... args )
    : base( p, std::forward<Args>(args)... )
  {  }
 
  value_construct( const value_construct& other )
    noexcept(std::is_nothrow_copy_constructible<base>::value) = delete;
  value_construct( value_construct&& other )
    noexcept(std::is_nothrow_move_constructible<base>::value) = delete;

  value_construct& operator=( const value_construct&  )
    noexcept(std::is_nothrow_copy_assignable<base>::value) = delete;
  value_construct& operator=(       value_construct&& )
    noexcept(std::is_nothrow_move_assignable<base>::value) = delete;
};

}
#endif // __VALUE_HPP__

