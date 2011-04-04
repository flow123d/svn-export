
#include "interpolant.h"

using namespace std;

namespace Interpolation
{
  
//InterpolantAdapt - ADAPTIVE
InterpolantAdapt::InterpolantAdapt(const vector< Polynomial* > &polynomials)
{
  this->polynomials = polynomials;
  this->polynomialcount = polynomials.size();
  this->a = polynomials[0]->GetLower();
  this->b = polynomials[polynomialcount-1]->GetUpper();
}

unsigned long InterpolantAdapt::FindPolynomial(double& x)
{
//Numerical Recipes in C, 3.4 How to Search an Ordered Table, hunt method
/*
void hunt(float xx[], unsigned long n, float x, unsigned long *jlo)
Given an array xx[1..n], and given a value x, returns a value jlo such that x is between
xx[jlo] and xx[jlo+1]. xx[1..n] must be monotonic, either increasing or decreasing.
jlo=0 or jlo=n is returned to indicate that x is out of range. jlo on input is taken as the
initial guess for jlo on output.
jlo -> i_guess

unsigned long jm,jhi,inc;

//We suppose ascending order of table.

//int ascnd;
//ascnd=(xx[n] >= xx[1]);
//True if ascending order of table, false otherwise.


// may be skipped, we could guarantee 0 < jlo < n elsewhere
//if (*i_guess <= 0 || *i_guess >= polynomialcount) {
  //Input guess not useful. Go immediately to bisection.
  // *i_guess=0;
  //jhi = polynomialcount+1;
//}


//else 
{
  inc=1; //Set the hunting increment.
  if (x >= polynomials[*i_guess]->GetLower()) 
  { //Hunt up:
    if (*i_guess == polynomialcount-1) return *i_guess;
    
    jhi = (*i_guess)+1;
    while (x >= polynomials[jhi]->GetLower()) 
    { //Not done hunting,
      *i_guess = jhi;
      inc += inc;	//so double the increment
      jhi = (*i_guess) + inc;
      
      if (jhi > polynomialcount-1) 
      {	//Done hunting, since oﬀ end of table.
	jhi = polynomialcount;
	break;
      }
      //Try again.
    }
  }
  //Done hunting, value bracketed.
  else 
  { //Hunt down:
    if (*i_guess == 0) 
    {
      return *i_guess;
    }
    
    jhi = (*i_guess)--;
    while (x < polynomials[*i_guess]->GetLower()) 
    { //Not done hunting,
      jhi = (*i_guess);
      inc <<= 1; //so double the increment
      if (inc >= jhi) 
      { //Done hunting, since oﬀ end of table.
	*i_guess = 0;
	break;
      }
      else *i_guess = jhi-inc;
    }
    //and try again.
  }
  //Done hunting, value bracketed.
}

//Hunt is done, so begin the ﬁnal bisection phase:
while (jhi-(*i_guess) != 1) 
{
  jm = (jhi+(*i_guess)) >> 1;
  if (x >= polynomials[jm]->GetLower())
    *i_guess = jm;
  else
    jhi = jm;
}
if (x == polynomials[polynomialcount-1]->GetLower()) *i_guess = polynomialcount-1;
if (x == polynomials[0]->GetLower()) *i_guess = 0;

return *i_guess;
*/

//Numerical Recipes in C, 3.4 How to Search an Ordered Table, hunt method
/*
void locate(float xx[], unsigned long n, float x, unsigned long *j)
Given an array xx[1..n], and given a value x, returns a value j such that x is between xx[j]
and xx[j+1]. xx must be monotonic, either increasing or decreasing. j=0 or j=n is returned
to indicate that x is out of range.
*/

  unsigned long ju,jm,jl;

  jl = 0;			//Initialize lower
  ju = polynomialcount;		//and upper limits.

  while (ju-jl > 1) 
  { //If we are not yet done,
    jm = (ju+jl) >> 1;		//compute a midpoint,
    if (x >= polynomials[jm]->GetLower())
      jl = jm;		//and replace either the lower limit
    else
      ju = jm;		//or the upper limit, as appropriate.
  } //Repeat until the test condition is satisﬁed.

  //Then set the output
  if (x == polynomials[0]->GetLower()) return 0;
  else if(x == polynomials[polynomialcount-1]->GetLower()) return polynomialcount-1;
  else return jl;
}


//InterpolantEq - EQUIDISTANT
InterpolantEq::InterpolantEq(vector< Polynomial* > &polynomials, const double &step)
  : step(step)
{
  this->polynomials = polynomials;
  this->polynomialcount = polynomials.size();
  this->a = polynomials[0]->GetLower();
  this->b = polynomials[polynomialcount-1]->GetUpper();
}


unsigned long InterpolantEq::FindPolynomial(double& x)
{
  //counts in which interval x is (the last node before x)
  return  floor((x - a) / step);
}


}



