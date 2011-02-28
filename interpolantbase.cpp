

#include "interpolantbase.h"

namespace Interpolation
{
  double InterpolantBase::Integral(double a, double b)
  {
    std::vector<Polynomial* >::iterator iterator;
    Polynomial *pol; 
    iterator = FindPolynomial(a, pol);
    double result = pol->Integral(a,pol->GetUpper());
    for(iterator; iterator != FindPolynomial(b, pol); iterator++)
    {
	result += pol->Integral();
    }
    result += pol->Integral(pol->GetLower(),b);
    return result;
  }

  der InterpolantBase::Diff(double x)
  {
    //Horner schema in Polynomials
    der d;
    d.f = 0;	    // Value of function
    d.dfdx = 0; 	// Value of df/dx
    return d;    	// Return function value
  }

  double InterpolantBase::Value(double x)
  {
    Polynomial *pol;
    if (x < a)
      return left->Value(x);
    if (x > b)
      return right->Value(x);
    FindPolynomial(x,pol);
    return pol->Value(x); //Horner schema in Polynomials
  }
}