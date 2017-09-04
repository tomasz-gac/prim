#include "ReprVisitor.hpp"

namespace AST{

std::string ReprVisitor::visit( const  Regex& node ) {
  return "<Terminal parser (Regex)>";
}

std::string ReprVisitor::visit( const Alternative& node ) {
  return "<Alternative>" + visitChildren( node );
}

std::string ReprVisitor::visit( const Sequence& node ) {
  return "<Sequence>" + visitChildren( node );
}

}

