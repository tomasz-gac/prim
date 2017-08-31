#ifndef __TREE_HPP__
#define __TREE_HPP__

#include "visitor.hpp"
#include "bool_mpl.hpp"
#include <vector>
#include <array>
#include <memory>

/*note:   no known conversion for argument 1 from '
IVisitable<IVisitor<tree<int, float, bool, std::basic_string<char, std::char_traits<char>, std::allocator<char> > >::Node<int>, tree<int, float, bool, std::basic_string<char, std::char_traits<char>, std::allocator<char> > >::Node<float>, tree<int, float, bool, std::basic_string<char, std::char_traits<char>, std::allocator<char> > >::Node<bool>, tree<int, float, bool, std::basic_string<char, std::char_traits<char>, std::allocator<char> > >::Node<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > >
  ::VisitorAdapter<main()::<lambda(const auto:5&)>&>
' to '
IVisitable<IVisitor<tree<int, float, bool, std::basic_string<char, std::char_traits<char>, std::allocator<char> > >::Node<int>, tree<int, float, bool, std::basic_string<char, std::char_traits<char>, std::allocator<char> > >::Node<float>, tree<int, float, bool, std::basic_string<char, std::char_traits<char>, std::allocator<char> > >::Node<bool>, tree<int, float, bool, std::basic_string<char, std::char_traits<char>, std::allocator<char> > >::Node<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > >::IVisitor&
{
  aka
IVisitor<tree<int, float, bool, std::basic_string<char, std::char_traits<char>, std::allocator<char> > >::Node<int>, tree<int, float, bool, std::basic_string<char, std::char_traits<char>, std::allocator<char> > >::Node<float>, tree<int, float, bool, std::basic_string<char, std::char_traits<char>, std::allocator<char> > >::Node<bool>, tree<int, float, bool, std::basic_string<char, std::char_traits<char>, std::allocator<char> > >::Node<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > >&}
*/

template< typename... Ts >
class tree{
private:
  template< typename T >
  class Node;

private:
  class IVisitor :
      public ::IVisitor<
        Node<Ts>...
      >
  {};
  // using IVisitor = IVisitor< Node<Ts>... >;

  class INode : public IVisitable< IVisitor >
  {
  public:
    template< typename F >
    void operator<<( F&& f ){
      auto wrapper = INode::adaptVisitor(f);
      this->accept( wrapper );
    }
  };

  template< typename OnEntryF, typename OnExitF >
  struct treeVisitor {
  public:
    template< typename T >
    void operator()( Node<T>& node ){
      onEntry_( node.value );
      for( const auto& c : node->children_ )
        node->accept( INode::adaptVisitor( *this ) );
      onExit_( node.value );
    }

    template< typename T >
    void operator()( const Node<T>& node ){
      onEntry_( node.value );
      for( const auto& c : node->children_ )
        node->accept( INode::adaptVisitor( *this ) );
      onExit_( node.value );
    }
    template< typename F, typename G >
    treeVisitor( F&& f, G&& g )
    : onEntry_(std::forward<F>(f))
    , onExit_(std::forward<G>(g))
    {  }

  private:
    OnEntryF onEntry_;
    OnExitF  onExit_;
  };

public:

  template< typename OnEntryF, typename OnExitF >
  void visit( OnEntryF& onEntry, OnExitF& onExit ){
    auto wrapper = INode::adaptVisitor( treeVisitor<OnEntryF&, OnExitF&>{ onEntry, onExit } );
    root_.accept( wrapper );
  }

  template< typename OnEntryF >
  void in_order( OnEntryF& onEntry ) {
    auto none = [](auto&){};
    visit( onEntry, none );
  }

  template< typename OnExitF >
  void post_order( OnExitF& onExit ) {
    auto none = [](auto&){};
    visit( none, onExit );
  }

  template< typename OnEntryF, typename OnExitF >
  void visit( OnEntryF& onEntry, OnExitF& onExit ) const {
    auto wrapper = INode::adaptVisitor( treeVisitor<OnEntryF&, OnExitF&>{ onEntry, onExit } );
    root_.accept( wrapper );
  }

  template< typename OnEntryF >
  void in_order( OnEntryF& onEntry ) const {
    auto none = [](auto&){};
    visit( onEntry, none );
  }

  template< typename OnExitF >
  void post_order( OnExitF& onExit ) const {
    auto none = [](auto&){};
    visit( none, onExit );
  }

        INode& root()       { return *root_; }
  const INode& root() const { return *root_; }

  template<
    typename T
  , typename std::enable_if<
    Any< std::is_same<T,Ts>::value ... >::value
    , bool>::type = true
  > explicit tree( T&& v )
   : root_( std::make_unique<Node<T>>( std::forward<T>(v) ) )
  {  }

private:
  std::unique_ptr< INode > root_;
};

template< typename... Ts >
template< typename T >
class tree<Ts...>::Node
: public Visitable<Node<T>>::template extends<INode>
{
public:
  Node( const T&  v ) : value(v) {}
  Node(       T&& v ) : value(std::move(v)) {}

  T value;
  std::vector< std::unique_ptr<INode> > children_;
};


#endif // __TREE_HPP__

