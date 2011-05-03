
#ifndef FUNCTORVALUEBASE_H
#define FUNCTORVALUEBASE_H

namespace Interpolation
{
///Abstract class for functors.
/** Abstract class which is used as interface 
  * for functors giving function value.
  */
class FunctorValueBase
{
  public:
    virtual double operator() ( const double &x ) = 0;
};

}

#endif 	//FUNCTORVALUEBASE_H


