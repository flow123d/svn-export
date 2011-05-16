
#ifndef INTERPOLANTBASE_H
#define INTERPOLANTBASE_H

#include <iostream>

#include "massert.h"
#include "functordiffbase.h"
#include "polynomial.h"

#ifndef DEB
#define DEB true
#endif

namespace Interpolation
{

/// Abstract class InterpolantBase.
/** Defines the interpolant on the interval [a,b] that is constructed
  * by an interpolation class. Implements the interface FunctorDiffBase. 
  * Implements also return integral. Is still an abstract class for the
  * pure virtual method FindPolynomial().
  */
class InterpolantBase : public FunctorDiffBase
{
  protected:
    ///vector of polynomials      
    std::vector<Polynomial> polynomials;
    
    /** Polynomials of the extrapolation outside the bounds a,b.
      * Left polynomial is counted when x < a.
      * Right polynomial is counted when x > b.
      */
    Polynomial left;
    Polynomial right;
    
    ///is the same as vector.size() of polynomials
    unsigned long polynomialcount;
    
    ///	"a" and "b" are lower and upper bounds of polynom
    double a,b;

    
    /// A private pure virtual method.
    /** Goes through the vector of polynomials and looks for the
      * interval where "x" lies so the value, integral or the derivate 
      * could be counted.
      * @param x is dependent variable.
      * @param *pol pointer to polynomial
      * @return  vector iterator in the vector polynomials.
      */ 
    virtual unsigned long FindPolynomial(const double &x) = 0;
    
    ///Math Power x^n
    template<class T>
    T Power(T x, unsigned int n)
    {
      T result = 1;
      for(unsigned int i = 0; i < n; i++)
      {
	result = result*x;
      }
      return result;
    }
    
    ///Factorial
    unsigned long Fact(unsigned int x)
    {     
      if (x > 1)
	return x*Fact(x-1);
      else
	return 1;
    }
 
  public:
    ///constructor
    InterpolantBase( std::vector<Polynomial> &polynomials );
    
    ///destructor
    ~InterpolantBase( void );
    
    ///returns left bound of the interval
    inline double GetA()
    {  return a; }
    
    ///returns right bound of the interval
    inline double GetB()
    {  return b; }
    
    ///returns the polynomialcount
    inline unsigned long GetCount()
    { return polynomialcount; }
    
    ///returns the pointer to the vector of polynomials
    inline Polynomial* GetPol( unsigned long i )
    { return &(polynomials[i]); }
    
    /// A function value.
    /** Counts the function value of the interpolant in "x".
      * Is evaluated by a polynom using the Horner schema. 
      * @param x is dependent variable.
      * @return  value of interpolant in "x".
      */ 
    virtual double operator() ( const double &x );

    /// A Derivate.
    /** Counts the derivate of the interpolant in "x"
      * Is evaluated by a polynom using the Horner schema. 
      * @param x is dependent variable.
      * @return structure "der" which includes the function
      * value and the derivate of interpolant.
      */    
    virtual der Diff( const double &x );
    
    /// An integral.
    /** Counts the integral of the interpolant between limits "u" and "v".
      * Is evaluated by a integration of polynoms by Horner schema.
      * @param u a lower limit.
      * @param v an upper limit.
      * @return  value of the integral.
      */
    double Integral( const double &u, const double &v ); 
    
    ///Sets type of extrapolation
    /** Sets the polynomials that should be counted when x is out of bounds a,b.
      * @param left a polynomial for x < a.
      * @param right a polynomial for b > b.
      */
    void SetExtrapolation( const Polynomial &left, const Polynomial &right );
    
    ///Sets type of extrapolation
    /** Creates the polynomials that should be counted when x is out of bounds a,b.
      * Extrapolation depends on the degree of BC by default (during interpolation)
      * or can be set explicitly.
      * @param left_degree degree of extrapolation for x <= a.
      * @param right_degree degree of extrapolation for b >= b.
      */
    void SetExtrapolation ( const unsigned char &left_degree, const unsigned char &right_degree );
};


}
#endif	//INTERPOLANTBASE_H
