#ifndef __DESCRIPTOR_HPP__
#define __DESCRIPTOR_HPP__

#include "node.hpp"
#include "VisitOnce.hpp"
#include <vector>
#include <algorithm>
#include <string>
#include <typeinfo>

template< typename T >
std::string descriptor_name( const T&) { return typeid( T ).name(); }

class Descriptor
  : public const_visitor<Descriptor>
{
 public:
  template< typename T >
  void operator()( const T& node )
  {
    result = std::string();	// requires empty result on entry to recurse
    auto text = ident();
    
    auto onUnique = [this, &text]( const T& node){
      auto id = visit_.visited().size();
      text += std::to_string( id ) + ":" + visit_node( node ); // handle specific nodes
    };
    
    auto onVisited = [this, &text]( const T& node){
      auto index = visit_.visited().size();
      text += "<<Node " + std::to_string( index  ) + " recursion>> : " + std::to_string( (size_t)&node );
    };
    
    visit_.once( node, onUnique, onVisited );
    result = std::move(text);	// return by member
  }

  std::string result;
    
 private:
  template< typename >
  struct print_type;
  template< typename T >
    std::string visit_node( const T& node ){
    auto text = "<" + descriptor_name( node ) + "> : " + std::to_string( (size_t)&node );
    ident_ += "|";
    auto& separator = ident_.back();
    const auto end_it = children_cend( node );
    for( auto it = children_cbegin(node); it != end_it; ++it ){
      if( std::next( it ) == end_it ){
       	separator = ' ';
       }
      visit( *it );
      text += "\n" + this->result;
    }
    ident_.pop_back();
    return text;
  }
    
  std::string ident() const {
    if( !ident_.empty() ){
      return std::string( ident_.cbegin(), --ident_.cend() ) + "+";
    } else {
      return "";
    }    
  }

  std::string ident_ = "";
  VisitOnce visit_;
};

#endif // __DESCRIPTOR_HPP__
