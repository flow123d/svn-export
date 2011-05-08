
#include "polynomial.h"
namespace Interpolation
{
  
Polynomial::Polynomial()
{
}
  
Polynomial::Polynomial(const unsigned char &degree)
  : degree(degree) 
{}
  
Polynomial::Polynomial(const double &a, const double &b, const std::vector< double > &coefs)
  : degree(coefs.size()-1), a(a), b(b), coefs(coefs)
{}

Polynomial::Polynomial(const Interpolation::Polynomial &pol)
  : degree(pol.degree), a(pol.a), b(pol.b), coefs(pol.coefs)
{}

Polynomial::~Polynomial(void )
{}



void Polynomial::SetInterval(const double &a, const double &b)
{
  MASSERT(a!=b,"Bounds overlap.");
  MASSERT(a<b,"a must be lower and b must be upper bound.");
  this->a = a;
  this->b = b;
}


void Polynomial::SetCoeficients(double* coeficients, const unsigned int &size)
{
  coefs.resize(size);
  for(unsigned int i = 0; i < size; i++)
  {
    coefs[i] = *(coeficients + i);
  }
}

 
double Polynomial::operator() ( const double &x )
{
  double xx = x - a;	//placing x to the interval <0,step>
  //std::cout << "xxxxxxxxxxx     " << "xx=" << xx << std::endl;
  //Horner's schema: ...(a[n]*x + a[n-1])*x + a[n-2])...
  //a[N]...coef[N], a[0]...coef[0]
  double result = coefs[degree];
  int n = degree -1;
  while(n >= 0)
  {
    result = result*xx + coefs[n];
    n--;
  }
  return result;
}

der Polynomial::Diff(const double &x)
{
  double xx = x - a;		//placing x to the interval <0,step>
  //Horner's schema: ...(a[n]*x + a[n-1])*x + a[n-2])...
  //a[N]...coef[N], a[0]...coef[0]
  der result;
  double f = coefs[degree];
  double dfdx = degree * coefs[degree];
  int n = degree - 1;
  while(n >= 1)
  {
    f = f*xx + coefs[n];
    dfdx = dfdx*xx + n*coefs[n];
    n--;
  }
  f = f*xx + coefs[0];
  result.f = f;
  result.dfdx = dfdx;
  return result;
}

double Polynomial::Integral(const double &u, const double &v)
{
  if(u > v) 
    return Integral(v,u); //replace limits (lower < upper)
  else
  {
    double aa = u-a;
    double bb = v-a;
    //Horner's schema: (a[N]*x/(N+1) + a[N-1]/N)*x + a[N-2]/(N-1)*x +...
    //...+ a[1]/2)*x + a[0]*x
    //(a[n]*x/(n+1) + a[n-1]/n)*x + a[n-2]/(n-1))...
    //a[N]...coef[N], a[0]...coef[0]
    double result_a = coefs[degree] / (degree+1);	//value in lower limit a
    double result_b = coefs[degree] / (degree+1);	//value in upper limit b
    int n = degree-1;
    while(n >= 0)
    {	
      result_a = result_a*aa + coefs[n] / (n+1);
      result_b = result_b*bb + coefs[n] / (n+1);
      n--;
    }
    result_a *= aa;
    result_b *= bb;
    return (result_b - result_a);	//result is the difference between values in limits
  }
}

double Polynomial::Integral(void )
{
  return Polynomial::Integral(a,b);
}

void Polynomial::WriteCoef(void )
{
  std::cout << "{";
  for(uint i=0; i < coefs.size()-1; i++)
  {
    std::cout << coefs[i] << " | ";
  }
  std::cout << coefs[coefs.size()-1] << "}";
}


}	//namespace Interpolation

