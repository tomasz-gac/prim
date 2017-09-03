#include "ReprVisitor.hpp"

namespace AST{
  
std::string ReprVisitor::visit( const  Regex& node ) {
  return "<Terminal parser (Regex)>";
}

std::string ReprVisitor::visit( const Alternative& node ) {
  auto text = std::string("<Alternative { options =\n" );

  for( auto& node : node.children ){
    auto v = fork();
    auto av = Rule::adaptVisitor( v );
    std::cout << "ident = " + std::to_string(av.f_.depth_);
    node->accept( av );
    text += "\n" + v.result; 
  }

  return text;
}

std::string ReprVisitor::visit( const Sequence& node ) {
  auto text = std::string("<Sequence { options =\n" );

  for( auto& node : node.children ){
    auto v = fork();
    auto av = Rule::adaptVisitor( v );
    std::cout << "ident = " + std::to_string(av.f_.depth_);
    node->accept( av );
    text += "\n" + v.result; 
  }
 
  return text;
}

}
