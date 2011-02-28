
#include "polynomial.h"
namespace Interpolation
{
  
double Polynomial::Value(double x)
{
  double xx = x - a;	//placing x to the interval <0,step>
  
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

der Polynomial::Diff(double x)
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

double Polynomial::Integral(double a, double b)
{
  if(a > b) 
    return Integral(b,a); //replace limits (lower < upper)
  else
  {
    //Horner's schema: (a[N]*x/(N+1) + a[N-1]/N)*x + a[N-2]/(N-1)*x +...
    //...+ a[1]/2)*x + a[0]*x
    //(a[n]*x/(n+1) + a[n-1]/n)*x + a[n-2]/(n-1))...
    //a[N]...coef[N], a[0]...coef[0]
    double result_a = coefs[degree] / (degree+1);	//value in lower limit a
    double result_b = coefs[degree] / (degree+1);	//value in upper limit b
    int n = degree-1;
    while(n >= 0)
    {	
      result_a = result_a*a + coefs[n] / (n+1);
      result_b = result_b*b + coefs[n] / (n+1);
      n--;
    }
    result_a *= a;
    result_b *= b;
    return (result_b - result_a);	//result is the difference between values in limits
  }
}

double Polynomial::Integral(void )
{
  return Polynomial::Integral(a,b);
}


}

