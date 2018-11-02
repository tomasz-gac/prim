#ifndef __VALUE_HPP__
#define __VALUE_HPP__

#include "allocator.hpp" //TODO : usunac
#include "view.hpp"
#include "builtins.hpp"
#include "allocator_traits.hpp"

template< typename T >
struct in_place{  };

namespace poly{

template< typename Implementation, typename Allocator, bool enable_copy, bool enable_move >
struct value_construct;

template< typename Impl, typename Alloc>
class value_construct_impl;

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
private:
  using base = value_base< Impl, Alloc >;

public:
  using implementation = impl_t<Impl>;
  using interface = interface_t<implementation>;

  static constexpr bool nothrow_copy = std::is_nothrow_copy_constructible<base>::value;
  static constexpr bool nothrow_move = std::is_nothrow_move_constructible<base>::value;
  static constexpr bool nothrow_copy_assign = std::is_nothrow_copy_assignable<base>::value;
  static constexpr bool nothrow_move_assign = std::is_nothrow_move_assignable<base>::value;

  template< typename... Args >
  static constexpr bool nothrow_constructs = std::is_nothrow_constructible<base, Args...>::value;

  template< typename... Args >
  static constexpr bool nothrow_assigns = std::is_nothrow_assignable<base, Args...>::value;

  using base::operator[];
  using base::get;
  using base::call;
  using base::valueless_by_exception;
  using base::vtable;
  using base::emplace;
 
  Value( in_place<Invalid> ) = delete;

  template< typename T, typename... Args >
  Value( in_place<T> p, Args&&... args )
    : base( p, std::forward<Args>(args)... )
  {  }

  Value( const Value&  ) noexcept(nothrow_copy) = default;
  Value(       Value&& ) noexcept(nothrow_move) = default;
  
  template< typename Alloc_ >
  Value( const Value< Impl, Alloc_ >& other  )
    noexcept(nothrow_constructs< const Value< Impl, Alloc_ >&>)
    : base( value_cast_tag(), other )
  {  }

  template< typename Alloc_ >
  Value(       Value< Impl, Alloc_ >&& other )
    noexcept(nothrow_constructs< Value< Impl, Alloc_ >&&>)
    : base( value_cast_tag(), std::move(other) )
  {  }

  Value& operator=( const Value& other )
    noexcept(nothrow_copy_assign) = default;
  
  Value& operator=( Value&& other )
    noexcept(nothrow_move_assign) = default;

  template< typename Alloc_ >
  Value& operator=( const Value<Impl, Alloc_>& other )
    noexcept(nothrow_assigns<const Value<Impl, Alloc_>&>)
  {
    static_cast<base&>(*this) = static_cast<const typename Value<Impl,Alloc>::base&>(other);
    return *this;
  }

  template< typename Alloc_ >
  Value& operator=(        Value<Impl, Alloc_>&& other )
    noexcept(nothrow_assigns<Value<Impl, Alloc_>&&>)
  {
    static_cast<base&>(*this) = static_cast<typename Value<Impl,Alloc>::base&&>(other);
    return *this;
  }

  template< typename, typename >
  friend class Value;
};

template< typename Impl, typename Alloc>
class value_construct_impl
  : public Alloc, public Reference< Impl >
{
public:
  using Reference_t = Reference< Impl >;
  using Alloc_t = Alloc;

  static constexpr bool move_is_noexcept = 
    supports< interface_t<Impl>, move_noexcept>();

   using move_tag = typename std::conditional< move_is_noexcept, move_noexcept, move >::type;

public:
  // Check if object is in an invalid state
  bool valueless_by_exception() const {
    try{
      storage::call(*this);
      return false;
    } catch ( const invalid_vtable_call& e ){
      return true;
    }
  }

  //Construct object of type T in-place
  template< typename T, typename... Args >
  void emplace( Args&&... args ){
    static_assert( !std::is_same<T,Invalid>::value, "Cannot construct objects of type Invalid" );
    this->reset();
    this->Reference_t::operator=( allocate_construct<T>( std::forward<Args>(args)... ) );
  }
  
  template< typename T, typename... Args >
  value_construct_impl( in_place<T>, Args&&... args )
    : Alloc()
    , Reference_t( this->allocate_construct<T>( std::forward<Args>(args)... ) )
  {  }

  value_construct_impl( value_construct_impl&& other ) noexcept( move_is_noexcept )
    : value_construct_impl( value_cast_tag(), std::move(other) )
  {  } 

  value_construct_impl( const value_construct_impl& other )
    : value_construct_impl( value_cast_tag(), other)
  {  }

  

  template< typename Alloc_ >
  value_construct_impl( value_cast_tag, const value_construct_impl<Impl, Alloc_>& other  )
    : Alloc(), Reference_t(static_cast<const Reference<Impl>&>(other))
  {
    this->construct_from<copy>( other );
  }

  
  template< typename Alloc_ >
  value_construct_impl( value_cast_tag, value_construct_impl<Impl, Alloc_>&& other  )
    noexcept( move_is_noexcept )
    : Alloc(), Reference_t( static_cast<Reference<Impl>&&>(other) )
  {
    using optimize_move =
      std::integral_constant< bool, allocator_traits<Alloc_t, Alloc_t>::optimize_move >;

    optimized_move_construct( optimize_move(), std::move(other) );
  }

  ~value_construct_impl(){ this->reset(); }
  
  template< typename Alloc_ >
  value_construct_impl& operator=( const value_construct_impl<Impl, Alloc_>& other ){
    return this->assign<copy>( other );
  }

  template< typename Alloc_ >
  value_construct_impl& operator=(  value_construct_impl<Impl, Alloc_>&& other )
    noexcept( move_is_noexcept )
  {
    using optimize_move =
      std::integral_constant< bool, allocator_traits<Alloc_t, Alloc_t>::optimize_move >;
    return optimized_move_assign( optimize_move(), std::move(other) );
  }

  value_construct_impl& operator=( const value_construct_impl& other ){
    return this->assign<copy>( other );
  }

  value_construct_impl& operator=( value_construct_impl&& other )
    noexcept( move_is_noexcept ) {
    using optimize_move =
      std::integral_constant< bool, allocator_traits<Alloc_t, Alloc_t>::optimize_move >;
    return optimized_move_assign( optimize_move(), std::move(other) );
  }

  template< typename I, typename A >
  friend class value_construct_impl;  //For Alloc casting

private:
  // Assigns from other
  // calls operation (poly::move, poly::move_noexcept, poly::copy)
  template< typename operation, typename Other >
  value_construct_impl& assign( Other&& other ){
    this->reset();
    this->Reference_t::operator=( std::forward<Other>(other) ); // copy or move the vtable
    this->construct_from<operation>( std::forward<Other>(other));
    return *this;
  }

  // Overload for move optimization
  // Selected when allocator defines optimize_move trait
  template< typename Other >
  value_construct_impl& optimized_move_assign( std::true_type /*optimize move*/ , Other&& other ){
    Alloc_t& this_ = *this;
    typename Other::Alloc_t& other_ = other;
    if( other_.move_to( this_ ) ){
      this->reset();
      this->Reference_t::operator=( std::move(other) );
      other.invalidate();
      return *this;
    } else {
      return this->optimized_move_assign( std::false_type(), std::move(other) );
    }
  }

  //No optimization
  template< typename Other >
  value_construct_impl& optimized_move_assign( std::false_type /*optimize move*/ , Other&& other ){
    return this->assign<move_tag>( std::move(other) );
  }

  //Allocates memory for object contained in other
  //Calls operation (copy or move) into allocated memory
  //In case of exception - sets the object into invalid state
  //Does not handle the VTable
  // WARNING : assumes the object to be empty
  template< typename Operation, typename Other >
  void construct_from( Other&& other ){
    void* ptr = nullptr;
    try{
      auto info = storage::call( other );
      ptr = this->allocate( info );
      Operation::call( other, ptr );
      this->value() = ptr;
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

  // Overload for move optimization
  // Selected when allocator defines optimize_move trait
  template< typename Other >
  void optimized_move_construct( std::true_type /*optimize move*/, Other&& other ){
    Alloc_t& this_ = *this;
    typename Other::Alloc_t& other_ = other;
    if( other_.move_to( this_ ) ){
      // Other's view has already been moved from in initialization
      other.invalidate();
    } else {
      this->optimized_move_construct( std::false_type(), std::move(other) );
    }
  }

  // No optimization
  template< typename Other >
  void optimized_move_construct( std::false_type /*optimize move*/, Other&& other ){
    this->template construct_from<move_tag>( std::move( other ) );
  }
  
  //Allocates memory for type T and constructs it using Args
  //In case the T's constructor, or Alloc throws
  //sets the object as invalid and propagates the exception
  //Does not handle the VTable
  // WARNING : assumes the object to be empty
  template< typename T, typename... Args >
  Reference_t allocate_construct( Args&&... args ){
    void* ptr = nullptr;
    try{
      ptr = this->allocate( storage_info::template get<T>() );
      new (ptr) T(std::forward<Args>(args)...);
      return Reference_t( *reinterpret_cast<T*>(ptr) );
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
      destroy::call(*this);
      this->Alloc::deallocate( this->value() );
    } catch ( const invalid_vtable_call& e ){
      //Object is valueless by exception
      //In that case - it contains object of type Invalid
      //Since the object is a singleton, there is no need to destroy it
    }
  }
  
  //Function sets the object in an invalid state
  // WARNING : assumes the object to be empty
  void invalidate(){
    this->Reference_t::operator=({ Invalid::get() });
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

