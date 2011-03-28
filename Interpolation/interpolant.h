#ifndef INTERPOLANT_H
#define INTERPOLANT_H

#include <vector>
#include <math.h>

#include "polynomial.h"
#include "interpolantbase.h"

namespace Interpolation 
{

/// class Interpolant, derived from InterpolantBase
/** Completes the definition of the interpolant with 
  * NONequidistant interval division.
  * Implement a virtual function FindPolynomial by the 
  * method of half-dividing interval.
  */
class Interpolant : public InterpolantBase
{
  private:
    /// A private pure virtual method.
    /** Goes through the vector of polynomials and looks for the
      * interval where "x" lies so the value, integral or the derivate 
      * could be counted.
      * @param x is dependent variable.
      * @param *pol pointer to polynomial
      * @return  vector iterator in the vector polynomials.
      */ 
    virtual unsigned int FindPolynomial(double x);    
  
  public:
    ///A constructor of the interpolant.
    Interpolant(void);
};

/// class InterpolantEq, derived from InterpolantBase
/** Completes the definition of the EQUIDISTANT interpolant.
  * Implement a virtual function FindPolynomial.
  */
class InterpolantEq : public InterpolantBase
{
  private:
    int step;
    
    /// A private pure virtual method.
    /** Goes through the vector of polynomials and looks for the
      * interval where "x" lies so the value, integral or the derivate 
      * could be counted.
      * @param x is dependent variable.
      * @param *pol pointer to polynomial
      * @return  vector iterator in the vector polynomials.
      */ 
    virtual unsigned int FindPolynomial(double x);
  
  public:
    ///A constructor of the interpolant.
    InterpolantEq(std::vector<Polynomial* > &polynomials, double step);
    
};

}

#endif	//INTERPOLANT_H
