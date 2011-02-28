
#include "interpolant.h"

namespace Interpolation
{
  

Interpolant::Interpolant(void)
{
}


std::vector<Polynomial* >::iterator Interpolant::FindPolynomial(double x, Polynomial* pol)
{ 
  pol = polynomials[0];
  return polynomials.begin(); 
  
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


std::vector<Polynomial* >::iterator InterpolantEq::FindPolynomial(double x, Polynomial* pol)
{
  //counts in which interval x is (the last node before x) and saves that to i
  int i = floor((x - a) / step);	
  pol = polynomials[i];
  return polynomials.begin() + i;
}


}



