#ifndef __AST_HPP__
#define __AST_HPP__

#include "tree/tree.hpp"
 
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
  struct Regex : public Rule::INode::extend<Regex>{
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

  inline Rule* children_begin( Regex& ){ return nullptr; }
  inline Rule* children_end( Regex& ){ return nullptr; }
  inline const Rule* children_cbegin( const Regex& ){ return nullptr; }
  inline const Rule* children_cend( const Regex& ){ return nullptr; }
  inline Rule* children_rbegin( Regex& ){ return nullptr; }
  inline Rule* children_rend( Regex& ){ return nullptr; }
  inline const Rule* children_crbegin( const Regex& ){ return nullptr; }
  inline const Rule* children_crend( const Regex& ){ return nullptr; }

  
}
#endif // __AST_HPP__
