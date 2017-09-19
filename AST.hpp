#ifndef __AST_HPP__
#define __AST_HPP__

#include "tree/node.hpp"
#include "tree/descriptor.hpp"
#include <iostream>
 
namespace AST{
  using IRule =
    INode<
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
    , class Recursion
    >;

  using Rule = Node<IRule>;

}

template<>
class INode_interface< AST::IRule >{
public:
  INode_interface(){
    std::cout << "INode() : " << (size_t)this << std::endl;
  }
  
  virtual ~INode_interface(){
    std::cout << "~INode() : " << (size_t)this << std::endl;
  }
};

namespace AST{

  struct Alternative : IRule::Binary<Alternative> {
    Alternative( Rule&& lhs, Rule&& rhs )
      :IRule::Binary<Alternative>( std::move(lhs), std::move(rhs) )
    {  }
  };
  
  struct Sequence : IRule::Binary<Sequence>{
    Sequence( Rule&& lhs, Rule&& rhs )
      :IRule::Binary<Sequence>( std::move(lhs), std::move(rhs) )
    {  }
  };

  struct Handle : IRule::Unary< Handle >{
    Handle()
      : IRule::Unary<Handle>( Rule::make<Never>() )
      , rule( children[0] )
    {  }

    Rule& rule;
  };

  struct Not      : IRule::Unary<Not>{ Not( Rule&& rhs ):IRule::Unary<Not>(std::move(rhs)) {} };
  struct Optional : IRule::Unary<Optional>{ Optional( Rule&& rhs ):IRule::Unary<Optional>(std::move(rhs)) {} };
  struct Repeat   : IRule::Unary<Repeat>{ Repeat( Rule&& rhs ):IRule::Unary<Repeat>(std::move(rhs)) {} };
  struct Ignore   : IRule::Unary<Ignore>{ Ignore( Rule&& rhs ):IRule::Unary<Ignore>(std::move(rhs)) {} };

  struct Always : IRule::Terminal<Always>{ };
  struct Never  : IRule::Terminal<Never>{ };

  struct Recursion : IRule::extend<Recursion>{
    Recursion( IRule& rule_ )
      : rule(rule_)
    {  }
    
    IRule& rule;
  };
  
  struct Regex :  IRule::Terminal<Regex>{
    Regex( const char* re_ )
      : re( re_ )
    {  }

    std::string re;
  };

  inline Rule operator+( Rule&& rhs ){ return Rule::make< Repeat >( std::move(rhs) ); }
  inline Rule operator-( Rule&& rhs ){ return Rule::make< Optional >( std::move(rhs) ); }
  inline Rule operator!( Rule&& rhs ){ return Rule::make< Not >( std::move(rhs) ); }

  inline Rule operator|( Rule&&   lhs, Rule&&   rhs ){
    return Rule::make<Alternative>( std::move(lhs), std::move(rhs) );
  }
  inline Rule operator&( Rule&&   lhs, Rule&&   rhs ){
    return Rule::make<Sequence>( std::move(lhs), std::move(rhs) );
  }

  
}

std::string descriptor_name( const AST::Alternative& ){ return "AST::Alternative"; };
std::string descriptor_name( const AST::Sequence& ){ return "AST::Sequence"; };

std::string descriptor_name( const AST::Recursion& ){ return "AST::Recursion"; };
std::string descriptor_name( const AST::Not& ){ return "AST::Not"; };
std::string descriptor_name( const AST::Optional& ){ return "AST::Optional"; };
std::string descriptor_name( const AST::Repeat& ){ return "AST::Repeat"; };
std::string descriptor_name( const AST::Ignore& ){ return "AST::Ingore"; };

std::string descriptor_name( const AST::Always& ){ return "AST::Always"; };
std::string descriptor_name( const AST::Never& ){ return "AST::Never"; };
std::string descriptor_name( const AST::Regex& node ){ return "AST::Regex("+node.re+")"; };

AST::IRule* children_begin( AST::Recursion& node ){ return &node.rule; }
AST::IRule* children_end( AST::Recursion& node ){ return &node.rule+1;}
const AST::IRule* children_cbegin( const AST::Recursion& node ){ return &node.rule; }
const AST::IRule* children_cend( const AST::Recursion& node ){ return &node.rule+1; }
AST::IRule* children_rbegin( AST::Recursion& node ){ return children_begin(node); }
AST::IRule* children_rend( AST::Recursion& node ){ return children_end(node); }
const AST::IRule* children_crbegin( const AST::Recursion& node ){ return children_cbegin(node); }
const AST::IRule* children_crend( const AST::Recursion& node ){ return children_cend(node); }

#endif // __AST_HPP__
