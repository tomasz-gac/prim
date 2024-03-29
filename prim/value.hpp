#ifndef __VALUE_HPP__
#define __VALUE_HPP__

#include "reference.hpp"
#include "builtins.hpp"
#include "allocator_traits.hpp"

template< typename T >
struct in_place{  };

namespace prim{

template< typename Implementation, typename Allocator, bool enable_copy, bool enable_move >
struct value_construct;

template< typename Impl, typename Alloc>
class value_impl;

struct value_cast_tag {};

template< typename Impl, typename Alloc >
using value_base = value_construct< Impl, Alloc,
				  supports< interface_t<Impl>, copy>(),
				  supports< interface_t<Impl>, move>() ||
				  supports< interface_t<Impl>, move_noexcept>()>;


template< typename Impl, typename Alloc >
class value
  : public value_base< Impl, Alloc >
{
private:
  using base = value_base< Impl, Alloc >;
  using reference = reference< Impl >;

public:
  using implementation = Impl;
  using interface = interface_t<implementation>;

  static constexpr bool nothrow_copy = base::nothrow_copy;
  static constexpr bool nothrow_move =  base::nothrow_move;
  static constexpr bool nothrow_copy_assign = base::nothrow_copy_assign;
  static constexpr bool nothrow_move_assign = base::nothrow_move_assign;

  using base::valueless_by_exception;
  using base::vtable;
  using base::emplace;

  
  value( in_place<Invalid> ) = delete;
  
  template< typename T, typename... Args >
  value( in_place<T> p, Args&&... args )
    : base( p, std::forward<Args>(args)... )
  {  }

  value( const value&  ) noexcept(nothrow_copy) = default;
  value(       value&& ) noexcept(nothrow_move) = default;
  
  template< typename Alloc_ >
  value( const value< Impl, Alloc_ >& other  )
    noexcept( nothrow_copy )    
    : base( value_cast_tag(), other )
  {  }

  template< typename Alloc_ >
  value(       value< Impl, Alloc_ >&& other )
    noexcept( nothrow_move )
    : base( value_cast_tag(), std::move(other) )
  {  }

  value& operator=( const value& other )
    noexcept(nothrow_copy_assign) = default;
  
  value& operator=( value&& other )
    noexcept(nothrow_move_assign) = default;

  template< typename Alloc_ >
  value& operator=( const value<Impl, Alloc_>& other )
    noexcept( nothrow_copy_assign )
  {
    static_cast<base&>(*this) = static_cast<const typename value<Impl,Alloc>::base&>(other);
    return *this;
  }

  template< typename Alloc_ >
  value& operator=(        value<Impl, Alloc_>&& other )
    noexcept( nothrow_move_assign )
  {
    static_cast<base&>(*this) = static_cast<typename value<Impl,Alloc>::base&&>(other);
    return *this;
  }

  explicit operator pointer< Impl >() const {
    return { static_cast<const pointer<Impl>&>(*this) };
  }

  template< typename, typename >
  friend class value;
};

  
template< typename Impl, typename Alloc>
class value_impl
  : public Alloc, public reference< Impl >
{
public:
  using reference_t = reference< Impl >;
  using Alloc_t = Alloc;

  static constexpr bool move_is_noexcept = 
    supports< interface_t<Impl>, move_noexcept>();

   using move_tag = typename std::conditional< move_is_noexcept, move_noexcept, move >::type;

  static constexpr bool nothrow_copy = false;
  static constexpr bool nothrow_move = move_is_noexcept;
  static constexpr bool nothrow_copy_assign = false;
  static constexpr bool nothrow_move_assign = move_is_noexcept;
 
protected:
  template< typename T, typename... Args >
  value_impl( in_place<T>, Args&&... args )
    : Alloc()
    , reference_t( this->allocate_construct<T>( std::forward<Args>(args)... ) )
  {  }

  value_impl( value_impl&& other ) noexcept( nothrow_move )
    : value_impl( value_cast_tag(), std::move(other) )
  {  } 

  value_impl( const value_impl& other ) noexcept( nothrow_copy )
    : value_impl( value_cast_tag(), other)
  {  }

  template< typename Alloc_ >
  value_impl( value_cast_tag, const value_impl<Impl, Alloc_>& other  )
    : Alloc(), reference_t(static_cast<const reference<Impl>&>(other))
  {
    this->construct_from<copy>( other );
  }

  
  template< typename Alloc_ >
  value_impl( value_cast_tag, value_impl<Impl, Alloc_>&& other  )
    noexcept( nothrow_move )
    : Alloc(), reference_t( static_cast<reference<Impl>&&>(other) )
  {
    using optimize_move =
      std::integral_constant< bool, allocator_traits<Alloc_t, Alloc_t>::optimize_move >;

    optimized_move_construct( optimize_move(), std::move(other) );
  }

  ~value_impl(){ this->reset(); }
  
  template< typename Alloc_ >
  value_impl& operator=( const value_impl<Impl, Alloc_>& other )
    noexcept( nothrow_copy_assign )
  {
    return this->assign<copy>( other );
  }

  template< typename Alloc_ >
  value_impl& operator=(  value_impl<Impl, Alloc_>&& other )
    noexcept( nothrow_move_assign )
  {
    using optimize_move =
      std::integral_constant< bool, allocator_traits<Alloc_t, Alloc_t>::optimize_move >;
    return optimized_move_assign( optimize_move(), std::move(other) );
  }

  value_impl& operator=( const value_impl& other )
    noexcept( nothrow_copy_assign )
  {
    return this->assign<copy>( other );
  }

  value_impl& operator=( value_impl&& other )
    noexcept( nothrow_move_assign )
  {
    using optimize_move =
      std::integral_constant< bool, allocator_traits<Alloc_t, Alloc_t>::optimize_move >;
    return optimized_move_assign( optimize_move(), std::move(other) );
  }

  template< typename I, typename A >
  friend class value_impl;  //For Alloc casting
  
  // Check if object is in an invalid state
  bool valueless_by_exception() const {
    using erased_t = typename reference_t::erased_type;
    return this->value() == erased_t( &Invalid::get() );
  }

  //Construct object of type T in-place
  template< typename T, typename... Args >
  void emplace( Args&&... args ){
    static_assert( !std::is_same<T,Invalid>::value, "Cannot construct objects of type Invalid" );
    this->reset();
    this->reference_t::operator=( allocate_construct<T>( std::forward<Args>(args)... ) );
  }
  
private:
  // Assigns from other
  // calls operation (prim::move, prim::move_noexcept, prim::copy)
  template< typename operation, typename Other >
  value_impl& assign( Other&& other ){
    this->reset();
    this->reference_t::operator=( std::forward<Other>(other) ); // copy or move the vtable
    this->construct_from<operation>( std::forward<Other>(other));
    return *this;
  }

  // Overload for move optimization
  // Selected when allocator defines optimize_move trait
  template< typename Other >
  value_impl& optimized_move_assign( std::true_type /*optimize move*/ , Other&& other ){
    Alloc_t& this_ = *this;
    typename Other::Alloc_t& other_ = other;
    if( other_.move_to( this_ ) ){
      this->reset();
      this->reference_t::operator=( std::move(other) );
      other.invalidate();
      return *this;
    } else {
      return this->optimized_move_assign( std::false_type(), std::move(other) );
    }
  }

  //No optimization
  template< typename Other >
  value_impl& optimized_move_assign( std::false_type /*optimize move*/ , Other&& other ){
    return this->assign<move_tag>( std::move(other) );
  }

  //Allocates memory for object contained in other
  //Calls operation (copy or move) into allocated memory
  //In case of exception - sets the object into invalid state
  //Does not handle the VTable
  // WARNING : assumes the object to be empty
  template< typename Operation, typename Other >
  void construct_from( Other&& other ){
    if( !other.valueless_by_exception() ){
      void* ptr = nullptr;
      try{
	auto info = call<type>( *other );
	ptr = this->allocate( info );
	call<Operation>( *other, ptr );
	this->value() = ptr;
      } catch(...) {
	// Constructor or Alloc throws
	if( ptr != nullptr ){
	  this->deallocate( ptr );
	}
	invalidate();
	throw; // propagate the exception
      }
    } else {
      invalidate(); // other is valueless_by_exception
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
  reference_t allocate_construct( Args&&... args ){
    void* ptr = nullptr;
    try{
      ptr = this->allocate( type_info::template get<T>() );
      new (ptr) T(std::forward<Args>(args)...);
      return reference_t( *reinterpret_cast<T*>(ptr) );
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
    if( !this->valueless_by_exception() ){
      call<destroy>(**this);
      this->Alloc::deallocate( this->value().data );
    } else {
      //Object is valueless by exception
      //In that case - it contains object of type Invalid
      //Since the object is a singleton, there is no need to destroy it
    }
  }
  
  //Function sets the object in an invalid state
  // WARNING : assumes the object to be empty
  void invalidate(){
    this->reference_t::operator=({ Invalid::get() });
  }
};

template< typename Impl, typename Alloc>
struct value_construct< Impl, Alloc, true, true >
  : value_impl< Impl, Alloc >
{
private:
  using base = value_impl< Impl, Alloc >;
  
public:
  template< typename T, typename... Args >
  value_construct( in_place<T> p, Args&&... args )
    : base( p, std::forward<Args>(args)... )
  {  }
 
  value_construct( const value_construct& other )
    noexcept(base::nothrow_copy) = default;
  value_construct( value_construct&& other )
    noexcept(base::nothrow_move) = default;
  
  value_construct& operator=( const value_construct&  )
    noexcept(base::nothrow_copy_assign) = default;
  value_construct& operator=(       value_construct&& )
    noexcept(base::nothrow_move_assign) = default;
  
  template< typename Alloc_ >
  value_construct( value_cast_tag, const value_construct< Impl, Alloc_, true, true >& other )
    : base( value_cast_tag(), other )
  {  }
  template< typename Alloc_ >
  value_construct( value_cast_tag, value_construct< Impl, Alloc_, true, true >&& other )
  : base( value_cast_tag(), std::move(other) )
  {  }
};

template< typename Impl, typename Alloc>
struct value_construct< Impl, Alloc, false, true >
  : value_impl< Impl, Alloc >
{
private:
  using base = value_impl< Impl, Alloc >;

public:
  template< typename T, typename... Args >
  value_construct( in_place<T> p, Args&&... args )
    : base( p, std::forward<Args>(args)... )
  {  }
 
  value_construct( const value_construct& other )
    noexcept(base::nothrow_copy) = delete;
  value_construct( value_construct&& other )
    noexcept(base::nothrow_move) = default;
  
  value_construct& operator=( const value_construct&  )
    noexcept(base::nothrow_copy_assign) = delete;
  value_construct& operator=(       value_construct&& )
    noexcept(base::nothrow_move_assign) = default;

  template< typename Alloc_ >
  value_construct( value_cast_tag, value_construct< Impl, Alloc_, false, true >&& other )
    : base( value_cast_tag(), std::move(other) )
  {  }
};

template< typename Impl, typename Alloc>
struct value_construct< Impl, Alloc, true, false >
  : value_impl< Impl, Alloc >
{
private:
  using base = value_impl< Impl, Alloc >;

  template< typename Alloc_ >
  using other_base = value_impl< Impl, Alloc_ >;
public:
  template< typename T, typename... Args >
  value_construct( in_place<T> p, Args&&... args )
    : base( p, std::forward<Args>(args)... )
  {  }
 
  value_construct( const value_construct& other )
    noexcept(base::nothrow_copy) = default;
  value_construct( value_construct&& other )
    noexcept(base::nothrow_move) = delete;
  
  value_construct& operator=( const value_construct&  )
    noexcept(base::nothrow_copy_assign) = default;
  value_construct& operator=(       value_construct&& )
    noexcept(base::nothrow_move_assign) = delete;

  template< typename Alloc_ >
  value_construct( value_cast_tag, const value_construct< Impl, Alloc_, true, false >& other )
    : base( value_cast_tag(), other )
  {  }
};

template< typename Impl, typename Alloc>
struct value_construct< Impl, Alloc, false, false >
  : value_impl< Impl, Alloc >
{
private:
  using base = value_impl< Impl, Alloc >;
public:
  template< typename T, typename... Args >
  value_construct( in_place<T> p, Args&&... args )
    : base( p, std::forward<Args>(args)... )
  {  }
 
  value_construct( const value_construct& other )
    noexcept(base::nothrow_copy) = delete;
  value_construct( value_construct&& other )
    noexcept(base::nothrow_move) = delete;
  
  value_construct& operator=( const value_construct&  )
    noexcept(base::nothrow_copy_assign) = delete;
  value_construct& operator=(       value_construct&& )
    noexcept(base::nothrow_move_assign) = delete;
};

}
#endif // __VALUE_HPP__

