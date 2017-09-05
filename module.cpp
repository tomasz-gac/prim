#include "boost/python/module.hpp"
#include "boost/python/def.hpp"

#include "AST.hpp"
#include "ReprVisitor.hpp"
#include "tree/CloneVisitor.hpp"

char const* greet()
{
  auto rule = AST::Rule::make<AST::Regex>();
  auto b2 = rule & (rule | rule);
  auto cv = CloneVisitor< AST::Rule >( b2 );
  auto acv = AST::Rule::adaptVisitor( cv );
  b2.accept( acv );
  auto b3 = cv.result;//b2.node().clone();
  b2 = b2 & b3;
  AST::ReprVisitor printer;
  auto av = AST::Rule::adaptVisitor( printer );
  b2.accept( av );
  std::cout << printer.result << std::endl;
  return "hello, world";
}

BOOST_PYTHON_MODULE(hello_ext)
{
    using namespace boost::python;
    def("greet", greet);
}
