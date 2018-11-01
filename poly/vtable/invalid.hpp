#ifndef __INVALID_HPP__
#define __INVALID_HPP__

#include <exception>

namespace poly{

class Invalid{
private:
  Invalid() = default;
public:
  static Invalid& get(){
    static Invalid instance{};
    return instance;
  }

  Invalid( const Invalid&  ) = delete;  
  Invalid(       Invalid&& ) = delete;
};

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
