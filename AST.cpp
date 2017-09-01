#include "AST.hpp"

namespace AST{

Rule operator|( const Rule&   lhs, const Rule&   rhs ){
  return Rule::make<Alternative>( lhs.node(), rhs.node() );
}
Rule operator&( const Rule&   lhs, const Rule&   rhs ){
    return Rule::make<Sequence>( lhs.node(), rhs.node() );
}
  
void NodePrinter::operator()(        Regex& node ) { std::cout << "Regex node" << std::endl; }  
void NodePrinter::operator()( const  Regex& node ) { std::cout << "Regex node" << std::endl; }

void NodePrinter::operator()( Alternative& node ) {
  std::cout << "Alternative node" << std::endl;    
  visit_children( node );
}  
void NodePrinter::operator()( const Alternative& node ) {
  std::cout << "const Alternative node" << std::endl;
  visit_children( node );
}

  
void NodePrinter::operator()( Sequence& node ) {
  std::cout << "Sequence node" << std::endl;    
  visit_children( node );
}  
void NodePrinter::operator()( const Sequence& node ) {
  std::cout << "const Sequence node" << std::endl;
  visit_children( node );
}

void NodePrinter::operator()( Rule::NodeTerminal& node ) {
  std::cout << "NodeTerminal node" << std::endl;    
  visit_children( node );
}  
void NodePrinter::operator()( const Rule::NodeTerminal& node ) {
  std::cout << "const NodeTerminal node" << std::endl;
  visit_children( node );
}

void NodePrinter::operator()( Rule::NodeUnary& node ) {
  std::cout << "NodeUnary node" << std::endl;    
  visit_children( node );
}  
void NodePrinter::operator()( const Rule::NodeUnary& node ) {
  std::cout << "const NodeUnary node" << std::endl;
  visit_children( node );
}

void NodePrinter::operator()( Rule::NodeBinary& node ) {
  std::cout << "NodeBinary node" << std::endl;    
  visit_children( node );
}  
void NodePrinter::operator()( const Rule::NodeBinary& node ) {
  std::cout << "const NodeBinary node" << std::endl;
  visit_children( node );
}


}
