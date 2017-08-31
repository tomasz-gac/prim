#include "AST.hpp"


namespace AST{
class Alternative : public Visitable< Alternative >::extends< NodeBinaryOp  > {};
class Sequence    : public Visitable< Sequence >::extends< NodeBinaryOp > {};
class Regex    : public Visitable< Regex >::extends< NodeTerminal > {};

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
  
void NodePrinter::visit(        Regex& node ) { std::cout << "Regex node" << std::endl; }  
void NodePrinter::visit( const  Regex& node ) { std::cout << "Regex node" << std::endl; }

void NodePrinter::visit( Alternative& node ) {
  std::cout << "Alternative node" << std::endl;    
  visit_children( node );
}  
void NodePrinter::visit( const Alternative& node ) {
  std::cout << "const Alternative node" << std::endl;
  visit_children( node );
}

void NodePrinter::visit( Sequence& node ) {
  std::cout << "Sequence node" << std::endl;    
  visit_children( node );
}  
void NodePrinter::visit( const Sequence& node ) {
  std::cout << "const Sequence node" << std::endl;
  visit_children( node );
}

void NodePrinter::visit( NodeTerminal& node ) {
  std::cout << "NodeTerminal node" << std::endl;    
  visit_children( node );
}  
void NodePrinter::visit( const NodeTerminal& node ) {
  std::cout << "const NodeTerminal node" << std::endl;
  visit_children( node );
}

void NodePrinter::visit( NodeUnaryOp& node ) {
  std::cout << "NodeUnaryOp node" << std::endl;    
  visit_children( node );
}  
void NodePrinter::visit( const NodeUnaryOp& node ) {
  std::cout << "const NodeUnaryOp node" << std::endl;
  visit_children( node );
}

void NodePrinter::visit( NodeBinaryOp& node ) {
  std::cout << "NodeBinaryOp node" << std::endl;    
  visit_children( node );
}  
void NodePrinter::visit( const NodeBinaryOp& node ) {
  std::cout << "const NodeBinaryOp node" << std::endl;
  visit_children( node );
}


}
