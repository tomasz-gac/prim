#ifndef __AST_HPP__
#define __AST_HPP__

#include "tree/node.hpp"
#include "tree/descriptor.hpp"
#include <iostream>
 
namespace AST{
  using Rule =
    Node<
      class Alternative
    , class Sequence
    , class Handle
    , class Not
    , class Optional
    , class Repeat
    , class Ignore
    , class Always
    , class Never
    , class Regex
    >;

}

template<>
class INode_interface< AST::Rule >{
public:
  INode_interface(){
    std::cout << "INode() : " << (size_t)this << std::endl;
  }
  
  virtual ~INode_interface(){
    std::cout << "~INode() : " << (size_t)this << std::endl;
  }
};

namespace AST{
  // class Rule : public Rule_t__
  // {
  // public:
  //   Rule( const Rule_t__& other ) = delete;
  //   //   : Rule_t__( other )
  //   // {  }
  //   Rule( Rule_t__&& other )
  //     : Rule_t__( std::move(other) )
  //   {  }
    
  //   Rule( const char* re ) : Rule_t__( Rule_t__::make< Regex >( re ) ){  }
  // };

  struct Alternative : Rule::Binary<Alternative> {
    Alternative( Rule& lhs, Rule& rhs )
      : Rule::Binary<Alternative>( lhs, rhs )
    {  }
  };
  
  struct Sequence : Rule::Binary<Sequence>{
    Sequence( Rule& lhs, Rule& rhs )
      : Rule::Binary<Sequence>( lhs, rhs )
    {  }
  };

  // struct Handle : Rule::Unary< Handle >{
  //   Handle()
  //     : Rule::Unary< Handle >( Rule::make< Never >() )
  //   {  }
  // };

  struct Handle : Rule::INode::extend< Handle >{
    Rule* rule = nullptr;
  };


  struct Not      : Rule::Unary<Not>{ Not( Rule& rhs ): Rule::Unary<Not>(rhs) {} };
  struct Optional : Rule::Unary<Optional>{ Optional( Rule& rhs ): Rule::Unary<Optional>(rhs) {} };
  struct Repeat   : Rule::Unary<Repeat>{ Repeat( Rule& rhs ): Rule::Unary<Repeat>(rhs) {} };
  struct Ignore   : Rule::Unary<Ignore>{ Ignore( Rule& rhs ): Rule::Unary<Ignore>(rhs) {} };

  struct Always : Rule::Terminal<Always>{ };
  struct Never  : Rule::Terminal<Never>{ };
  
  struct Regex :  Rule::Terminal<Regex>{
    Regex( const char* re_ )
      : re( re_ )
    {  }

    std::string re;
  };

  inline Rule operator+( Rule& rhs ){ return Rule::make< Repeat >( rhs ); }
  inline Rule operator-( Rule& rhs ){ return Rule::make< Optional >( rhs ); }
  inline Rule operator!( Rule& rhs ){ return Rule::make< Not >( rhs ); }

  inline Rule operator|( Rule&   lhs, Rule&   rhs ){
    return Rule::make<Alternative>( lhs, rhs );
  }
  inline Rule operator&( Rule&   lhs, Rule&   rhs ){
    return Rule::make<Sequence>( lhs, rhs );
  }

  
}

std::string descriptor_name( const AST::Alternative& ){ return "AST::Alternative"; };
std::string descriptor_name( const AST::Sequence& ){ return "AST::Sequence"; };

std::string descriptor_name( const AST::Handle& ){ return "AST::Handle"; };
std::string descriptor_name( const AST::Not& ){ return "AST::Not"; };
std::string descriptor_name( const AST::Optional& ){ return "AST::Optional"; };
std::string descriptor_name( const AST::Repeat& ){ return "AST::Repeat"; };
std::string descriptor_name( const AST::Ignore& ){ return "AST::Ingore"; };

std::string descriptor_name( const AST::Always& ){ return "AST::Always"; };
std::string descriptor_name( const AST::Never& ){ return "AST::Never"; };
std::string descriptor_name( const AST::Regex& node ){ return "AST::Regex("+node.re+")"; };

AST::Rule* children_begin( AST::Handle& node ){ return node.rule; }
AST::Rule* children_end( AST::Handle& node ){ return node.rule ? node.rule+1 : node.rule; }
const AST::Rule* children_cbegin( const AST::Handle& node ){ return node.rule; }
const AST::Rule* children_cend( const AST::Handle& node ){ return node.rule ? node.rule+1 : node.rule; }
AST::Rule* children_rbegin( AST::Handle& node ){ return children_begin(node); }
AST::Rule* children_rend( AST::Handle& node ){ return children_end(node); }
const AST::Rule* children_crbegin( const AST::Handle& node ){ return children_cbegin(node); }
const AST::Rule* children_crend( const AST::Handle& node ){ return children_cend(node); }

#endif // __AST_HPP__
