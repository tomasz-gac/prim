#include "AST.hpp"


namespace AST{
class Rule::Alternative{  };
class Rule::Sequence{  };
class Rule::Terminal{  };

Rule Rule::operator|( const Rule&   other ) const{
  return Rule();
}
Rule Rule::operator|(       Rule&&  other ) const{
  return Rule();
}
Rule Rule::operator&( const Rule&   other ) const{
  return Rule();
}
Rule Rule::operator&(       Rule&&  other ) const{
  return Rule();
}

/*Alternative::Alternative( const NodeBase& lhs, const NodeBase& rhs )
{ 
  rules_[0] = std::move(lhs.clone()); 
  rules_[1] = std::move(rhs.clone());
}

Sequence::Sequence( const NodeBase& lhs, const NodeBase& rhs )
{ 
  rules_[0] = std::move(lhs.clone()); 
  rules_[1] = std::move(rhs.clone());
}


void NodePrinter::visit(        Terminal& Node ) { std::cout << "Terminal node" << std::endl; }  
void NodePrinter::visit( const  Terminal& Node ) { std::cout << "Terminal node" << std::endl; }

void NodePrinter::visit( Alternative& Node ) {
  std::cout << "Alternative node" << std::endl;    
  for( auto& node : Node.rules_ ){
    node->accept(*this);
  }
}  

void NodePrinter::visit( const Alternative& Node ) {
  std::cout << "const Alternative node" << std::endl;
  for( auto& node : Node.rules_ ){
    node->accept(*this);
  }
};

void NodePrinter::visit( Sequence& Node ) {
  std::cout << "Sequence node" << std::endl;    
  for( auto& node : Node.rules_ ){
    node->accept(*this);
  }
}  

void NodePrinter::visit( const Sequence& Node ) {
  std::cout << "const Sequence node" << std::endl;
  for( auto& node : Node.rules_ ){
    node->accept(*this);
  }
};*/


}
