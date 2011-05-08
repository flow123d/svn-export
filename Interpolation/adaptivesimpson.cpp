
#include "adaptivesimpson.h"

namespace Interpolation
{

double AdaptiveSimpson::Simpson(const double& h, const double &fa, 
				const double &fc, const double &fb)
{
  return (fa + fb + 4.0*fc)*(h/6.0);
}

double AdaptiveSimpson::SimpsonAd(FunctorValueBase &func, 
				  const double& h, const double &a, 
				  const double &c, const double &b,
				  const double &fa, const double &fc, 
				  const double &fb, const double &sx, 
				  const double &tol, long &recursion)
{
  recursion++;
  double ca = 0.5*(a+c);
  double cb = 0.5*(c+b);
  double fca = func(ca);
  double fcb = func(cb);
   
  double h2 = 0.5*h;
  double sa = Simpson(h2,fa,fca,fc);
  double sb = Simpson(h2,fc,fcb,fb);
  
  double err_est = (sa+sb-sx)/15.0;
  //std::cout << "sa=" << sa << " sb=" << sb << " sx=" << sx << " sa+sb=" << sa+sb <<" err_est=" << err_est << std::endl;
  if (fabs(err_est) <= tol || recursion >= MAX_RECURSION)
  {
    //std::cout << recursion << "\tsa+sb+err = " << sa+sb+err_est << std::endl;
    return sa+sb+err_est;
  }
  else
  {
    return SimpsonAd(func,h2,a,ca,c,fa,fca,fc,sa,tol,recursion) 
	   + SimpsonAd(func,h2,c,cb,b,fc,fcb,fb,sb,tol,recursion);
  }
}

double AdaptiveSimpson::AdaptSimpson( FunctorValueBase &func,
				      const double& a, const double& b, 
				      const double& tol )
{
  std::cout << "AdaptiveSimpson: a(" << a << ") b(" << b << ")";
  double c = 0.5*(b+a);
  double fa,fb,fc;
  double sx,res;

  fa = func(a);
  fb = func(b);
  fc = func(c);
  long recursion = 0;
  std::cout << "fa=" << fa << "\tfb=" << fb << "\tfc=" << fc;
  sx = Simpson(b-a, fa, fc, fb);
  res = SimpsonAd(func,b-a,a,c,b,fa,fc,fb,sx,tol,recursion);
  std::cout << "\trecursions: " << recursion << std::endl;
  return res;
}




}
