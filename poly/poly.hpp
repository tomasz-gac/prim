#ifndef __POLY_HPP__
#define __POLY_HPP__

#include <memory>

template< typename Tag, typename T >
static constexpr auto Invoker = Tag();

template< typename >
class Signature;

template< typename Return, typename... Args >
struct Signature< Return(Args...)>{
  using return_type = Return;
  using signature_type = Signature;
};

template< typename... Ts >
struct Interface
{  };

template< typename Interface >
class IHolder;

template< typename T, typename Interface >
class Holder;

namespace impl__{






  
  template< typename Tag, typename Signature = typename Tag::signature_type >
  class IHolder;
  
  template< typename Tag, typename Return, typename... Args >
  class IHolder< Tag, Signature< Return(Args...) > >
  {
  public:
    virtual Return  call( Args... args ) = 0;
    virtual ~IHolder() = default;
  };

  template< typename Tag, typename Return, typename... Args >
  class IHolder< const Tag, Signature< Return(Args...) > >
  {
  public:
    virtual Return  call( Args... args ) const = 0;
    virtual ~IHolder() = default;
  };







  
  template< typename Holder_t, typename Tag, typename Sig = typename Tag::signature_type >
  class Holder;

  template< typename T, typename Interface, typename Tag, typename Return, typename... Args >
  class Holder< ::Holder<T, Interface >,       Tag, Signature< Return(Args...) > >
    : public virtual ::IHolder< Interface >
  {
  public:
    virtual Return call( Args... args ) override {
      auto& value = static_cast< ::Holder< T, Interface >* >(this)->value;
      return Invoker< Tag, std::remove_const_t<T> >( value, args... );
    }
    virtual ~Holder() = default;
  };

  template< typename T, typename Interface, typename Tag, typename Return, typename... Args >
  class Holder< ::Holder<T, Interface >, const Tag, Signature< Return(Args...) > >
    : public virtual ::IHolder< Interface >
  {
  public:
    virtual Return call( Args... args ) const override {
      const auto& value = static_cast< const ::Holder< T, Interface >* >(this)->value;
      return Invoker< Tag, std::add_const_t<T> >( value, args... );
    }
    virtual ~Holder() = default;
  };








  

}

template< typename Tag >
class IHolder
  : public impl__::IHolder< Tag >
{
public:
  template< typename Tag_, typename... Args >
  typename Tag_::return_type call( Args&&... args )       {
    using IHolder_t =       impl__::IHolder<        Tag_ >;
    auto impl = static_cast< IHolder_t* >(this);
    return impl->call( std::forward<Args>(args)... );
  }
  virtual ~IHolder() = default;
};

template< typename Tag >
class IHolder< const Tag >
  : public impl__::IHolder< const Tag >
{
public:
  template< typename Tag_, typename... Args >
  typename Tag_::return_type call( Args&&... args ) const {
    using IHolder_t = const impl__::IHolder< const Tag_ >;
    auto impl = static_cast< IHolder_t* >(this);
    return impl->call( std::forward<Args>(args)... );
  }
  virtual ~IHolder() = default;
};

template< typename... Tags >
class IHolder< Interface< Tags... > >
  : public IHolder< Tags >...
{  };

template< typename T, typename... Tags >
class Holder< T, Interface< Tags... > >
  : public impl__::Holder<
    Holder< T, Interface< Tags... > >
  , Tags >...
{
public:
  template< typename... Ts >
  Holder( Ts&&... vs )
    : value( std::forward<Ts>(vs)... )
  {  }
  virtual ~Holder() = default;

  T value;
};

template< typename Interface >
class Poly{
private:
  using IHolder = IHolder< Interface >;
  template< typename T >
  using Holder = Holder<T, Interface>;
public:
  template< typename T >
  Poly& operator=( T v ){
    Poly< T > poly( std::move(v) );
    data_.reset( poly.data_.release() );
    return *this;
  }

  template< typename Tag, typename... Ts >
  typename Tag::return_type
  call( Ts&&... vs ){
    IHolder& data = *data_;
    using tag_t = std::remove_const_t<Tag>;
    return data.template call<tag_t>( std::forward<Ts>(vs)... );
  }

  template< typename Tag, typename... Ts >
  typename Tag::return_type
  call( Ts&&... vs ) const {
    const IHolder& data = *data_;
    using tag_t = std::add_const_t<Tag>;
    return data.template call<tag_t>( std::forward<Ts>(vs)... );
  }

  template< typename T >
  Poly( T v )
    : data_( std::unique_ptr<IHolder>( std::make_unique<Holder<T>>(std::move(v))) )
  { }
  
private:
  std::unique_ptr< IHolder > data_;
};

#endif // __POLY_HPP__
