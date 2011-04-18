#ifndef INTERPOLANT_H
#define INTERPOLANT_H

#include <vector>
#include <math.h>

#include "polynomial.h"
#include "interpolantbase.h"

namespace Interpolation 
{

/// class InterpolantAdapt, derived from InterpolantBase
/** Completes the definition of the interpolant with 
  * NONequidistant interval division.
  * Implement a virtual function FindPolynomial by the 
  * method of half-dividing interval (or hunt method can be modified).
  */
class InterpolantAdapt : public InterpolantBase
{
  private:
    /* for hunt method in FindPolynomial
    ///initial guess for FindPolynomial
    ///keeps last counted polynomial
    unsigned long *i_guess;
    */
    
    /// A private pure virtual method.
    /** Goes through the vector of polynomials and looks for the
      * interval where "x" lies so the value, integral or the derivate 
      * could be counted.
      * @param x is dependent variable.
      * @param *pol pointer to polynomial
      * @return  vector iterator in the vector polynomials.
      */ 
    virtual unsigned long FindPolynomial(const double& x);    
  
  public:
    ///A constructor of the interpolant.
    InterpolantAdapt(std::vector<Polynomial> &polynomials);
};


/// class InterpolantEq, derived from InterpolantBase
/** Completes the definition of the EQUIDISTANT interpolant.
  * Implement a virtual function FindPolynomial.
  */
class InterpolantEq : public InterpolantBase
{
  private:
    double step;
    
    /// A private pure virtual method.
    /** Goes through the vector of polynomials and looks for the
      * interval where "x" lies so the value, integral or the derivate 
      * could be counted.
      * @param x is dependent variable.
      * @param *pol pointer to polynomial
      * @return  vector iterator in the vector polynomials.
      */ 
    virtual unsigned long FindPolynomial(const double& x);
  
  public:
    ///A constructor of the interpolant.
    InterpolantEq(std::vector<Polynomial> &polynomials, const double &step);
    
};

}

#endif	//INTERPOLANT_H
