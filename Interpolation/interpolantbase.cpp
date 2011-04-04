

#include "interpolantbase.h"

namespace Interpolation
{
  
double InterpolantBase::GetA()
{
  return a;
}

double InterpolantBase::GetB()
{
  return b;
}

void InterpolantBase::SetExtrapolation(Polynomial* left, Polynomial* right)
{
  this->left = left;
  this->right = right;
}

double InterpolantBase::Integral(double a, double b)
{
    /*
    std::vector<Polynomial* >::iterator iterator;
    Polynomial* pol; 
    iterator = FindPolynomial(a, pol);
    double result = pol->Integral(a,pol->GetUpper());
    for(iterator; (iterator != FindPolynomial(b, pol)); iterator++)
    {
	result += pol->Integral();
    }
    result += pol->Integral(pol->GetLower(),b);
    return result;
    //*/
    return 0.0;
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
    if (x <= a)
      return left->Value(x);
    if (x >= b)
      return right->Value(x);
    
    return polynomials[FindPolynomial(x)]->Value(x); //Horner schema in Polynomials
}
  
void InterpolantBase::SetExtrapolation( unsigned char left_degree, unsigned char right_degree)
{
    std::vector<double> l(left_degree+1);
    std::vector<double> r(right_degree+1);
    std::cout << "extrapolate_left=[ ";
    /*
    on the left (polynomial is the same as polynomials[0]):
    P[0] = left
    */
    for(int i = 0; i <= left_degree; i++)
    {
      l[i] = (*polynomials[0]->GetCoefs())[i];
      std::cout << l[i]<< "|";
    }
    std::cout << "]"<< std::endl;

    std::cout << "extrapolate_right=[ ";
    
    /*
    on the right:
    a[0] + a[1]x + a[2]x^2 + a[3]x^3= left[0]	//value P[n](b) = right(b)
          a[1] + 2*a[2]x + 3*a[3]x^2= left[1]	//(1) derivate P'[n](b) = right'(b)
                    2*a[2] + 6*a[3]x= 2*left[2]	//(2) derivate P''[n](b) = right''(b)
                              6*a[3]= 6*left[3] //(3) derivate P'''[n](b) = right'''(b)
    where x = b - x[n-1]
    */
    for(int i = 0; i <= right_degree; i++)
    {
      for(int j = i; j <= right_degree; j++)
      {
	r[i] += (*polynomials[polynomialcount-1]->GetCoefs())[j] *
		  Power(b-polynomials[polynomialcount-1]->GetLower(),j-i) *
		  Fact(j)/Fact(j-i);
      }
      r[i] /= Fact(i);
      std::cout << r[i]<< "|";
    }
    std::cout << "]"<< std::endl;
    
    left = new Polynomial(a,a,l);
    right = new Polynomial(b,b,r);
    
}

}