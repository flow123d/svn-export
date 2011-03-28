
#include "interpolant.h"

namespace Interpolation
{
  

Interpolant::Interpolant(void)
{
}


unsigned int Interpolant::FindPolynomial(double x)
{
   return 0;
}

//InterpolantEq - EQUIDISTANT
InterpolantEq::InterpolantEq(std::vector< Polynomial* > &polynomials, double step)
  : step(step)
{
  this->polynomials = polynomials;
  this->polynomialcount = polynomials.size();
  this->a = polynomials[0]->GetLower();
  this->b = polynomials[polynomialcount-1]->GetUpper();
}


unsigned int InterpolantEq::FindPolynomial(double x)
{
  //counts in which interval x is (the last node before x) and saves that to i
  int i = floor((x - a) / step);	
  //std::cout << "double=" << ((x - a) / step) << "    int i=" << i << std::endl; 
  return i;
}


}



