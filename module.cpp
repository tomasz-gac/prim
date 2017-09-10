// #include "boost/python/module.hpp"
// #include "boost/python/def.hpp"
#include <iostream>
#include "AST.hpp"
#include "tree/descriptor.hpp"
#include "tree/CloneVisitor.hpp"

void print( const AST::Rule& rule ){
  Descriptor descriptor;
  descriptor.visit( rule );
  std::cout << descriptor.result << std::endl;
}

//char const* greet()
int main()
{
  std::cout << "begin" << std::endl;
  {
    auto never = AST::Rule::make< AST::Never >();
    std::cout << "Never" << std::endl;
    auto b2 = AST::Rule::make<AST::Not>( never );
    std::cout << "Not" << std::endl;
    static_cast< AST::Not& >(b2.node()).children[0] = b2;
    std::cout << "Ref" << std::endl;
  }
  std::cout << "end" << std::endl;
  return 0; //"hello, world";
}

// BOOST_PYTHON_MODULE(hello_ext)
// {
//     using namespace boost::python;
//     def("greet", greet);
// }

