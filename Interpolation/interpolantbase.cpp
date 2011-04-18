

#include "interpolantbase.h"

namespace Interpolation
{
  
InterpolantBase::InterpolantBase(std::vector< Polynomial >& polynomials)
  : polynomials(polynomials),polynomialcount(polynomials.size()),
    a(polynomials[0].GetLower()), b(polynomials[polynomials.size()-1].GetUpper())
{}

  
double InterpolantBase::GetA()
{
  return a;
}

double InterpolantBase::GetB()
{
  return b;
}

void InterpolantBase::SetExtrapolation(const Polynomial &left, const Polynomial &right)
{
  this->left = left;
  this->right = right;
}

double InterpolantBase::Integral(const double &a, const double &b)
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

der InterpolantBase::Diff(const double &x)
{
    //Horner schema in Polynomials
    der d;
    d.f = 0;	    // Value of function
    d.dfdx = 0; 	// Value of df/dx
    return d;    	// Return function value
}

double InterpolantBase::operator() ( const double &x )
{
    if (x <= a)
      return left(x);
    if (x >= b)
      return right(x);
    
    return polynomials[FindPolynomial(x)](x); //Horner schema in Polynomials
}
  
void InterpolantBase::SetExtrapolation( const unsigned char &left_degree, const unsigned char &right_degree)
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
      l[i] = (*polynomials[0].GetCoefs())[i];
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
	r[i] += (*polynomials[polynomialcount-1].GetCoefs())[j] *
		  Power(b-polynomials[polynomialcount-1].GetLower(),j-i) *
		  Fact(j)/Fact(j-i);
      }
      r[i] /= Fact(i);
      std::cout << r[i]<< "|";
    }
    std::cout << "]"<< std::endl;
    
    left = Polynomial(a,a,l);
    right = Polynomial(b,b,r);
    
}

}