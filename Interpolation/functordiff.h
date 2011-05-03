#ifndef FUNCTORDIFF_H
#define FUNCTORDIFF_H

#include "functordiffbase.h"
#include "../FADBAD++/badiff.h"
using namespace fadbad;

namespace Interpolation
{
///Templated class FunctorDiff  
/** Class that templated by the mathematical functor, 
  * that returns its value and derivate. 
  * Inherits from FunctorDiffBase and implements 
  * virtual operator () and Diff(). It uses FADBAD 
  * library to obtain 1st derivate.
  */
template<class Func>
class FunctorDiff : public FunctorDiffBase
{
private:
  Func fce;	

public:
  ///constructor
  FunctorDiff(void){}
  
  ///destrcutor
  ~FunctorDiff(void){}
  
  ///returns 1st derivate using FADBAD
  virtual der Diff ( const double &i_x )
  {
    B<double> x(i_x); 	// Initialize arguments
    Func func;       	// Instantiate functor
    B<double> f(func(x)); // Evaluate function and record DAG
    f.diff(0,1);     	// Differentiate
  
    der d;
    d.f = f.x();	// Value of function
    d.dfdx = x.d(0); 	// Value of df/dx
    return d;    	// Return function value
  }

  ///overloaded operator() returns value of the function
  virtual double operator() ( const double &x )
  {
    return fce(x);
  }
  
};

}

#endif	//FUNCTORDIFF_H