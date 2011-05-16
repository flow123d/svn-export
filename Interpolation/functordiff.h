#ifndef FUNCTORDIFF_H
#define FUNCTORDIFF_H

#include "massert.h"
#include "functordiffbase.h"
#include "../FADBAD++/fadbad.h"
#include "../FADBAD++/badiff.h"
#include "../FADBAD++/tadiff.h"


///Defines how many derivates can be returned from Taylor's coeficients.
#define N 10

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
  virtual der Diff ( const double& i_x )
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
  
  /** Returns n-th derivate of function in i_x.
    * Uses coeficients in Taylor's row.
    */
  double Diffn(double i_x, unsigned int n)
  {
    MASSERT(n <= N,"Cannot obtain nth derivate. Check header file.");
    T<double> x,f;

    x = i_x;
    x[1] = 1;
    f = fce(x);
    f.eval(N);
	
    for(int i = 0; i<N; i++)	//goes through the field of Taylor's coeficients
    {				//and divide them by the factorial to get derivates
      f[i] = f[i] * this->Fact(i);
    }
    
    return f[n];		//returns n-th derivate
  }

  
  ///Factorial
  long Fact ( long x )
  {     
    if (x > 1)
      return x*Fact(x-1);
    else
      return 1;
  }
  
};

}

#endif	//FUNCTORDIFF_H