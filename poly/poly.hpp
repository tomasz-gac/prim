#ifndef __POLY_HPP__
#define __POLY_HPP__

template< typename Tag, typename T >
static constexpr auto Invoker = Tag();

template< typename >
class Signature;

template< typename Return, typename... Args >
struct Signature< Return(Args...)>{
  using return_type = Return;
  using signature_type = Signature;
};


namespace impl__{

  template< typename Tag >
  class Interface;

  template< typename Return, typename Args >
  class Interface< Signature< Return(Args...) > >
  {
  public:
    virtual return_type call( Args... args ) = 0;
    virtual return_type call( Args... args ) const = 0;
    virtual ~Interface() = default;
  };

  template< typename T, typename Tag, typename Signature = typename Tag::signature_type >
  class Implementation;

  template< typename T, typename Tag, typename Return, typename Args >
  class Implementation< T, Tag, Signature< Return(Args...) > >
    : public Interface< Signature< Return(Args...) > >
  {
  public:
    virtual Return call( Args... args ) override {
      return Invoker< Tag, std::remove_const_t<T> >( args... );
    }
    virtual return_type call( Args... args ) const override {
      return Invoker< Tag, std::add_const_t<T> >( args... );
    }
    virtual ~Implementation() = default;
  };
}




template< typename Interface >
class Poly{
public:
  

private:
  class IHolder
    : private impl__::Interface< Interface >
  {
    template< typename Tag, typename... Args >
    typename Tag::return_type call( Args&&... args )       {
      auto impl = static_cast<      impl__::Interface< Tag >* >(this)l
      return impl->call( std::forward<Args>(args)... );
    }

    template< typename Tag, typename... Args >
    typename Tag::return_type call( Args&&... args ) const {
      auto impl = static_cast< const impl__::Interface< Tag >* >(this)l
      return impl->call( std::forward<Args>(args)... );
    }

    virtual IHolder* clone() const = 0;
    
  public:    
    virtual ~IHolder() = default;
  };


  template< typename T >
  class Holder
    : public impl__::Implementation< T, Interface >
  {
  public:
    virtual IHolder* clone() const override {
      return new T( value_ );
    }
    
  private:
    T value_;    
  };
  
};

#endif // __POLY_HPP__
