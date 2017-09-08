#ifndef __AST_HPP__
#define __AST_HPP__

#include "tree/tree.hpp"
#include "tree/descriptor.hpp"
 
namespace AST{
  using Rule = Tree< class Alternative, class Sequence, class Regex >;

  struct Alternative
    : public Rule::Binary<Alternative>
  {
    Alternative( Rule lhs, Rule rhs )
      : Rule::Binary<Alternative>( lhs, rhs )
    {  }
  };
  
  struct Sequence
    : public Rule::Binary<Sequence>
  {
    Sequence( Rule lhs, Rule rhs )
      : Rule::Binary<Sequence>( lhs, rhs )
    {  }
  };
  struct Regex : public Rule::Terminal<Regex>{
  public:
    Regex( const Regex& ){}
    Regex(){}
  };

  inline Rule operator|( Rule   lhs, Rule   rhs ){
    return Rule::make<Alternative>( std::move(lhs), std::move(rhs));
  }

  inline Rule operator&( Rule   lhs, Rule   rhs ){
    return Rule::make<Sequence>( std::move(lhs), std::move(rhs));
  }
  
}

std::string descriptor_name( const AST::Alternative& ){ return "AST::Alternative"; };
std::string descriptor_name( const AST::Sequence& ){ return "AST::Sequence"; };
std::string descriptor_name( const AST::Regex& ){ return "AST::Regex"; };

#endif // __AST_HPP__
