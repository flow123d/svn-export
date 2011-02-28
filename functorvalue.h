
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
    FunctorValue(void) {}
    ~FunctorValue(void) {}
    
    virtual double Value(double x)
    {
      return fce(x);
    }
};

}
#endif	//FUNCTORVALUE_H