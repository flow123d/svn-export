
#ifndef FUNCTORVALUEBASE_H
#define FUNCTORVALUEBASE_H

namespace Interpolation
{
//abstract class which is used as interface 
//for functors giving function value
class FunctorValueBase
{
  public:
    virtual double Value(double x) = 0;
};

}

#endif 	//FUNCTORVALUEBASE_H


