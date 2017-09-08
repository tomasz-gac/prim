#include "boost/python/module.hpp"
#include "boost/python/def.hpp"
#include <iostream>
#include "AST.hpp"
#include "tree/descriptor.hpp"
#include "tree/CloneVisitor.hpp"

char const* greet()
{
  auto rule = AST::Rule::make<AST::Regex>();
  auto b2 = (rule | rule) & rule;
  auto b3 = clone(b2); // b2.node().clone();
  b2 = b2 & b3;
  Descriptor descriptor;
  descriptor.visit( b2 );
  std::cout << descriptor.result << std::endl;
  return "hello, world";
}

BOOST_PYTHON_MODULE(hello_ext)
{
    using namespace boost::python;
    def("greet", greet);
}

