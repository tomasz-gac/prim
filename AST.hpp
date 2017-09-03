#ifndef __AST_HPP__
#define __AST_HPP__


#include "tree.hpp"
#include <iostream>
 
namespace AST{
  using Rule = Tree< class Alternative, class Sequence, class Regex >;

  struct Alternative
    : public Rule::INode::Binary<Alternative>
  {  };
  
  struct Sequence
    : public Rule::INode::Binary<Sequence>
  {  };
  struct Regex : public Rule::INode::extend<Regex>
  {  };
 
  Rule operator|( Rule, Rule );
  Rule operator&( Rule, Rule );

}
#endif // __AST_HPP__
