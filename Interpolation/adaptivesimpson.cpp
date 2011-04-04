
#include "adaptivesimpson.h"

namespace Interpolation
{
  
AdaptiveSimpson::AdaptiveSimpson( FunctorValueBase* func, 
				  InterpolantBase* interpolant, 
				  const double& a, const double& b, 
				  const double& tol)
  : a(a), b(b), tol(tol), func(func), interpolant(interpolant)
{
}

AdaptiveSimpson::AdaptiveSimpson( FunctorValueBase* func, 
				  const double& a, const double& b, 
				  const double& tol)
  : a(a), b(b), tol(tol), func(func)
{
  interpolant = NULL;
}

void AdaptiveSimpson::SetInterpolant(InterpolantBase* interpolant)
{
  this->interpolant = interpolant;
}

  
double AdaptiveSimpson::Simpson(const double& h, double &fa, double &fc, double &fb)
{
  return (fa + fb + 4*fc)*h/6;
}

double AdaptiveSimpson::SimpsonAd(double (AdaptiveSimpson::*fce)(double&), 
				  const double& h, double &a, double &c, double &b,
				  double &fa, double &fc, double &fb, 
				  double &sx, double &tol)
{
  double ca = 0.5*(a+c);
  double cb = 0.5*(c+b);
  double fca = (this->*fce)(ca);
  double fcb = (this->*fce)(cb);
   
  double h2 = 0.5*h;
  double sa = Simpson(h2,fa,fca,fc);
  double sb = Simpson(h2,fc,fcb,fb);
  
  double ErrEst = (sa+sb-sx)/15;
  if (abs(ErrEst) <= tol)
    return sa+sb+ErrEst;
  else
    return SimpsonAd(fce,h2,a,ca,c,fa,fca,fc,sa,tol) 
	   + SimpsonAd(fce,h2,c,cb,b,fc,fcb,fb,sb,tol);
}

double AdaptiveSimpson::L2norm(double& x)
{
  double f = func->Value(x);
  double g = interpolant->Value(x);
  double res = (f-g)*(f-g);
  std::cout << "x=" << x << "  ->  " << res << std::endl;
  return res;
}

double AdaptiveSimpson::W1_1norm(double& x)
{
  double f = func->Value(x);
  double g = interpolant->Value(x);
  return (f-g)*(f-g);
}

double AdaptiveSimpson::AdaptSimpson(AdaptiveSimpson::Norm type)
{
  MASSERT(type < W1_1,"This norm needs derivate of the function, cannot use FunctorValueBase.");
  std::cout << "AdaptiveSimpson: a(" << a << ") b(" << b << ")" << std::endl;
  double c = 0.5*(b-a);
  double fa,fb,fc;
  double sx,res;

  switch(type)
  {
    case 0:	
      fa = L2norm(a);
      fb = L2norm(b);
      fc = L2norm(c);
      sx = Simpson(b-a, fa, fc, fb);
      res = SimpsonAd(&AdaptiveSimpson::L2norm,b-a,a,c,b,fa,fc,fb,sx,tol);
      return sqrt(res);
      break;
	
    default:
      fa = L2norm(a);
      fb = L2norm(b);
      fc = L2norm(c);
      sx = Simpson(b-a, fa, fc, fb);
      res = SimpsonAd(&AdaptiveSimpson::L2norm,b-a,a,c,b,fa,fc,fb,sx,tol);
      return sqrt(res);
  }
}




}
