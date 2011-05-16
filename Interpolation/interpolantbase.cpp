

#include "interpolantbase.h"

namespace Interpolation
{
  
InterpolantBase::InterpolantBase(std::vector< Polynomial >& polynomials)
  : polynomials(polynomials),polynomialcount(polynomials.size()),
    a(polynomials[0].GetA()), b(polynomials[polynomials.size()-1].GetB())
{}

InterpolantBase::~InterpolantBase(void )
{}


void InterpolantBase::SetExtrapolation(const Polynomial &left, const Polynomial &right)
{
  this->left = left;
  this->right = right;
}

double InterpolantBase::Integral(const double &u, const double &v)
{
    if (a > b) return Integral(b,a);	//integral must be from lower to upper limit
    
    if ( v <= a )			//limits are outside the interval of interpolation
      return left.Integral(u,v);
    if ( u >= b )			//limits are outside the interval of interpolation
      return right.Integral(u,v);	
    
    double result = 0;
    unsigned long j = 0;		//limit u is in j-th interval
    unsigned long k = 0;		//limit u is in k-th interval
    if ( u < a)			
    {
      result += left.Integral(u,a);	//limit u is outside the interval of interpolation
      j = 0;
    }
    else
      j = FindPolynomial(u);
    
    if ( v > b)
    {
      result += right.Integral(b,v);	//limit u is outside the interval of interpolation
      k = GetCount()-1;
    }
    else
      k = FindPolynomial(v);

    //first incomplete polynomial
    result = polynomials[j].Integral(u,polynomials[j].GetB());

    //complete polynomials in the middle
    for(unsigned long i=j+1; i < k; i++)
    {
	result += polynomials[i].Integral();
    }
    
    //last incomplete polynomial
    result += polynomials[k].Integral(polynomials[k].GetA(),v);

    return result;
}

der InterpolantBase::Diff(const double &x)
{
    if (x <= a)		//left extrapolation
      return left.Diff(x);
    if (x >= b)		//right extrapolation
      return right.Diff(x);
    
    return polynomials[FindPolynomial(x)].Diff(x); //Horner schema in Polynomials
}

double InterpolantBase::operator() ( const double &x )
{
    if (x <= a)		//left extrapolation
      return left(x);
    if (x >= b)		//right extrapolation
      return right(x);
    
    return polynomials[FindPolynomial(x)](x); //Horner schema in Polynomials
}
  
void InterpolantBase::SetExtrapolation( const unsigned char &left_degree, const unsigned char &right_degree)
{
    std::vector<double> l(left_degree+1);
    std::vector<double> r(right_degree+1);
    /*
    on the left (polynomial is the same as polynomials[0]):
    P[0] = left
    */
    for(int i = 0; i <= left_degree; i++)
    {
      l[i] = (*polynomials[0].GetCoefs())[i];
    }
    
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
		  Power(b-polynomials[polynomialcount-1].GetA(),j-i) *
		  Fact(j)/Fact(j-i);
      }
      r[i] /= Fact(i);
    }
    
    left = Polynomial(a,a,l);
    right = Polynomial(b,b,r);
    
    if(DEB) 
    {
      std::cout << "extrapolate_left=";
      left.WriteCoef();
      std::cout << "\nextrapolate_right=";
      right.WriteCoef();
      std::cout << std::endl;
    }
    
}

}