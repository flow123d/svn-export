
#ifndef FUNCTORVALUE_H
#define FUNCTORVALUE_H

#include "functorvaluebase.h"

namespace Interpolation
{
  
//class templated by the mathematical functor, that returns its value
template<class Func>
class FunctorValue : public FunctorValueBase
{
  private:
    Func fce;
    
  public:
    ///constructor
    FunctorValue(void) {}
    
    ///destructor
    ~FunctorValue(void) {}
    
    ///overloaded operator() returns value of the function
    virtual double operator() ( const double &x )
    {
      return fce(x);
    }
};

}
#endif	//FUNCTORVALUE_H