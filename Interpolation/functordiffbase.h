
#ifndef FUNCTORDIFFBASE_H
#define FUNCTORDIFFBASE_H

#include "functorvaluebase.h"

namespace Interpolation
{
/// Structure der
/** Structure with derivate and value, that is return by Diff()
  */
struct der
{
  double f;
  double dfdx;
};

///Abstract class for functors.
/** Abstract class which is used as interface 
  * for functors giving function value and 1st derivate.
  */
class FunctorDiffBase : public FunctorValueBase
{
  public:
    virtual der Diff(const double &x) = 0;	//returns df(x)/dx and f(x)
    virtual double operator() ( const double &x ) = 0;
};

}

#endif 	//FUNCTORDIFFBASE_H