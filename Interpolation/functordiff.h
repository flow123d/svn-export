#ifndef FUNCTORDIFF_H
#define FUNCTORDIFF_H

#include "functordiffbase.h"
#include "../FADBAD++/badiff.h"
using namespace fadbad;

namespace Interpolation
{
//class that is derived and expanded from FunctorDiffBase
//and templated by the mathematical function
//it uses class DFunc, that uses FADBAD library and 
//is able to evaluate derivates
template<class Func>
class FunctorDiff : public FunctorDiffBase
{
private:
  Func fce;	

public:
  FunctorDiff(void){}
  ~FunctorDiff(void){}
  
  //returns 1st grade derivate
  //virtual double Diff(double x);
  virtual der Diff ( const double &i_x )
  {
    B<double> x(i_x); 	// Initialize arguments
    Func func;       	// Instantiate functor
    B<double> f(func(x)); // Evaluate function and record DAG
    f.diff(0,1);     	// Differentiate
  
    der d;
    d.f = f.x();	 	// Value of function
    d.dfdx = x.d(0); 	// Value of df/dx
    return d;    		// Return function value
  }

  virtual double operator() ( const double &x )
  {
    return fce(x);
  }
  
};

}

#endif	//FUNCTORDIFF_H