#include "boost/python/module.hpp"
#include "boost/python/def.hpp"

#include "AST.hpp"
#include "ReprVisitor.hpp"

char const* greet()
{
  auto rule = AST::Rule::make<AST::Regex>();
  auto b2 = rule & rule | rule;
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
