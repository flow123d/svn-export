
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
    /** Evaluates the derivate.
      * @param x is the point in which the derivate is being evaluated.
      * @return der is structure of value of the function and derivate
      */
    virtual der Diff(const double &x) = 0;	//returns df(x)/dx and f(x)
    
    ///Overloaded operator() returning the value of the function.
    virtual double operator() ( const double &x ) = 0;
};

}

#endif 	//FUNCTORDIFFBASE_H