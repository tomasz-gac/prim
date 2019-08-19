#include <iostream>
#include "prim/value.hpp"
#include "prim/allocator.hpp"
#include "prim/vtable/vtable.hpp"
#include "helpers.hpp"

#include <chrono>
#include <vector>

template< typename T, typename Children >
struct node_holder{
  T value;
  Children children;

  node_holder( T v, Children c = Children() )
    : value(v), children( std::move(c) )
  {  }
};

template< typename T, typename C >
struct prim::wrapper_traits< node_holder<T,C> > {
  template< size_t index, typename U >
  static decltype(auto) get( U&& u ){
    return std::get<index>( std::tie( u, u.value, u.children ) );
  }
};


class Rule
{
private:
  struct compile_ :
    prim::declare< compile_, void (const prim::T&, const std::vector<Rule>&) >
  {  };
  
  template< typename T >
  friend void invoke( compile_, const T& value, const std::vector<Rule>& c ){
    value.compile(c);
  }
  
  struct Rule_
    : prim::Interface< compile_, prim::move_noexcept, prim::copy, prim::destroy, prim::storage >
  {  };

  using value_type = prim::value< prim::RemoteVT<Rule_>, prim::HeapAllocator >;

public:
  void compile() const {
    prim::call<compile_>(*value_, children_);
  }

  Rule( const Rule&  )          = default;
  Rule(       Rule&& ) noexcept = default;

  template< typename T, typename... Children, typename = std::enable_if_t<!std::is_same<T, Rule>::value> >
  Rule( T& node, Children&&... children )
    : value_( in_place<T>(), node )
    , children_( { Rule( std::forward<Children>(children)  )... } )
  {  }

private:
  value_type value_;
  std::vector< Rule > children_;
};


struct A{
  void compile( const std::vector<Rule>& v ) const {
    std::cout << "compile A" << std::endl;
  }
};
struct B{
  void compile( const std::vector<Rule>& v ) const {
    std::cout << "compile B" << std::endl;
  }
};
struct C{
  void compile( const std::vector<Rule>& v ) const {
    std::cout << "compile C" << std::endl;
  }
};


int main()
{
  auto start = std::chrono::high_resolution_clock::now();
  
  auto finish = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = finish - start;

  A a; B b; C c;
  Rule r( a, b, c );
  r.compile();
  Rule q(r);
  std::cout << "Constructed" << std::endl;
  q.compile();

  std::cout << "Passed : " << elapsed.count() << "s" << std::endl;
  return 0;
}
