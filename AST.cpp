#include "AST.hpp"

namespace AST{

Rule operator|( Rule   lhs, Rule   rhs ){
  auto rule =  Rule::make<Alternative>();
  auto& node = static_cast<Alternative&>(rule.node());
  node.children[0] = lhs.node().ref();
  node.children[1] = rhs.node().ref();
  return rule;
}

Rule operator&( Rule   lhs, Rule   rhs ){
  auto rule =  Rule::make<Sequence>();
  auto& node = static_cast<Sequence&>(rule.node());
  node.children[0] = lhs.node().ref();
  node.children[1] = rhs.node().ref();
  return rule;
}

}
