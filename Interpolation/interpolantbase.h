
#ifndef INTERPOLANTBASE_H
#define INTERPOLANTBASE_H

#include <iostream>

#include "functordiffbase.h"
#include "polynomial.h"

namespace Interpolation
{

/// abstract class InterpolantBase.
/** Defines the interpolant on the interval [a,b] that is constructed
  * by an interpolation class. Implements the interface FunctorDiffBase. 
  * Implements also return integral. Is still an abstract class for the
  * pure virtual method FindPolynomial().
  */
class InterpolantBase : public FunctorDiffBase
{
  protected:
    ///vector of polynomials      
    std::vector<Polynomial* > polynomials;
    
    /** Polynomials of the extrapolation outside the bounds a,b.
      * Left polynomial is counted when x < a.
      * Right polynomial is counted when x > b.
      */
    Polynomial* left;
    Polynomial* right;
    
    /**	"a" and "b" are lower and upper bounds of polynom
      */
    double a,b;

    ///is the same as vector.size() of polynomials
    int polynomialcount;
    
    /// A private pure virtual method.
    /** Goes through the vector of polynomials and looks for the
      * interval where "x" lies so the value, integral or the derivate 
      * could be counted.
      * @param x is dependent variable.
      * @param *pol pointer to polynomial
      * @return  vector iterator in the vector polynomials.
      */ 
    virtual unsigned int FindPolynomial(double x) = 0;
 
  public:
    /// A function value.
    /** Counts the function value of the interpolant in "x".
      * Is evaluated by a polynom using the Horner schema. 
      * @param x is dependent variable.
      * @return  value of interpolant in "x".
      */ 
    virtual double Value(double x);

    /// A Derivate.
    /** Counts the derivate of the interpolant in "x"
      * Is evaluated by a polynom using the Horner schema. 
      * @param x is dependent variable.
      * @return structure "der" which includes the function
      * value and the derivate of interpolant.
      */    
    virtual der Diff(double x);
    
    /// An integral.
    /** Counts the integral of the interpolant between limits "a" and "b".
      * Is evaluated by a integration of polynoms by Horner schema.
      * @param a a lower limit.
      * @param a an upper limit.
      * @return  value of the integral.
      */
    double Integral(double a, double b); 
    
    ///Set type of extrapolation
    /** Sets the polynomials that should be counted when x is out of bounds a,b.
      * @param left a polynomial for x < a.
      * @param right a polynomial for b > b.
      * @return  value of the integral.
      */
    void SetExtrapolation(Polynomial *left, Polynomial *right)
    {
      this->left = left;
      this->right = right;
    }
};


}
#endif	//INTERPOLANTBASE_H
