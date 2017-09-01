#include "boost/python/module.hpp"
#include "boost/python/def.hpp"

#include "AST.hpp"

char const* greet()
{
  auto rule = AST::Rule::make<AST::Regex>();
  auto b2 = rule | rule;
  AST::NodePrinter printer;
  b2.accept( printer );
  return "hello, world";
}

BOOST_PYTHON_MODULE(hello_ext)
{
    using namespace boost::python;
    def("greet", greet);
}
