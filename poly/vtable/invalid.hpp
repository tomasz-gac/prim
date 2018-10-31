#ifndef __INVALID_HPP__
#define __INVALID_HPP__

#include <exception>

namespace poly{

struct Invalid{  };

class invalid_vtable_call
  : public std::exception
{
public:
  virtual const char* what() const noexcept override {
    return "Function call in purposefully invalid vtable ";
  }
};

}

#endif //__INVALID_HPP__
