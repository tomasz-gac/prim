#ifndef __VISITOR__HPP__
#define __VISITOR__HPP__

#include <type_traits>

  //Visitable base class
template< typename  >
class Visitable;

  //Visitable class interface
template< typename... >
class IVisitor;

  //Assures that the visitor type derives from IVisitor
template< typename IVisitor_type >
class IVisitable
 : public IVisitable<typename IVisitor_type::derives_from_IVisitor>
{  };

  //Implements the Visitor adapter
template< typename Derived, typename Root, typename... Ts >
class VisitorWrapper;

  //IVisitable base class
template< typename... Ts >
class IVisitable< IVisitor<Ts...> >{
public:
    //Visitor iterface type
  using IVisitor = IVisitor<Ts...>;

    //Visitor adapter type
  template< typename F >
  class VisitorAdapter;

    //Helper function
  template< typename F >
  static VisitorAdapter<F> adaptVisitor( F&& f )
  { return { std::forward<F>(f) }; }

    //Accept visitor pure virtual methods
  virtual void accept( IVisitor& )       = 0;
  virtual void accept( IVisitor& ) const = 0;

protected:
  virtual ~IVisitable() = default;
};

  //Implements the visitor adapter of F class
template< typename... Ts >
template< typename F >
class IVisitable<IVisitor<Ts...>>::VisitorAdapter
  : public VisitorWrapper< VisitorAdapter<F>, ::IVisitor<Ts...>, Ts... >
      //Derives from VisitorWrapper that implements all of the virtual methods
{
public:
  template< typename U >
  void visit( U&& u ){ f( std::forward<U>(u) ); }

  VisitorAdapter( F f_ ) : f(f_) {  }

  F f;
};

  //Basic IVisitor of T
template< typename T >
class IVisitor<T>{
public:
  using derives_from_IVisitor = IVisitor;

  virtual void visit(       T& )  = 0;
  virtual void visit( const T& )  = 0;

  virtual ~IVisitor()              = default;
};

  //IVisitor of Ts derives from Visitor<T>
template< typename T, typename... Ts >
struct IVisitor<T, Ts...>
 : IVisitor<T>, IVisitor<Ts...>
{
  using derives_from_IVisitor = IVisitor;
};

  //Visitable base class. Final to prevent errors
template< typename Derived >
struct Visitable final
{
    //Extends class wraps the Base class inheritance
  template< typename Base >
  class extends : public Base
  {
  public:
      //Propagate the IVisitor interface.
    using IVisitor = typename Base::IVisitor;

    //Prevents incomprehensible errors when visitor doesn't
    //define Derived as its visited class
    static_assert(
      std::is_base_of<::IVisitor<Derived>, IVisitor>::value
    , "Base class visitor interface does not support visitation of this type"
    );
      
    //Implement accept method via CRTP
    virtual void accept( IVisitor& v ) override{
    
      static_cast< ::IVisitor<Derived>& >(v).visit(   // Disambiguation
               static_cast<Derived&>(*this)           // CRTP
      );
    }
    virtual void accept( IVisitor& v ) const override {
      
      static_cast< ::IVisitor<Derived>& >(v).visit(   // Disambiguation
         static_cast<const Derived&>(*this)           // CRTP
      );      
    }
    
    template< typename... Ts >
    extends( Ts&&... Vs )
    : Base{ std::forward<Ts>(Vs)...}
    { }
  protected:
    virtual ~extends() = default;
  };

  ~Visitable() = delete;
};

template< typename > struct print_type;

//Uses CRTP to implement visit method of IVisitor
//Consumes the list of Ts... so that the inheritance hierarchy is linear
template<
  typename Derived  //  CRTP
, typename... Rs    //  Base class (IVisitor<Rs...>)
, typename T        //  Currently implemented type
, typename... Ts    //  The rest of types
>  class VisitorWrapper< Derived, IVisitor<Rs...>, T, Ts... >
 : public VisitorWrapper< Derived, IVisitor<Rs...>, Ts... >  //Consume T
{
    //Cast to Derived and call visit method
  virtual void visit(       T& v ){ static_cast<Derived*>(this)->visit(v); }
  virtual void visit( const T& v ){ static_cast<Derived*>(this)->visit(v); }
};

//Uses CRTP to implement visit method of IVisitor
template< typename Derived, typename... Rs, typename T >
class VisitorWrapper< Derived, IVisitor<Rs...>, T >
 : public IVisitor<Rs...>  //IVisitor<Ts...>
{
  virtual void visit(       T& v ){ static_cast<Derived*>(this)->visit(v); }
  virtual void visit( const T& v ){ static_cast<Derived*>(this)->visit(v); }
};

#endif // __VISITOR__HPP__

