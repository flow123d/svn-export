
#include "norm.h"

namespace Interpolation
{
 
//LP Norm============================================
LP_Norm::LP_Norm( FunctorValueBase* func1, 
		  FunctorValueBase *func2,
		  unsigned int p)
  : func1(func1), func2(func2), p(p)
{}

double LP_Norm::operator() (const double &x)
{
  double res = 1;
  for(unsigned int i = 0; i < p; i++)
  {
    res *= (func1->operator()(x) - func2->operator()(x));
  }
  return res;
}


//W1P Norm============================================
W1P_Norm::W1P_Norm( FunctorDiffBase* func1, 
		    FunctorDiffBase *func2,
		    unsigned int p)
  : func1(func1), func2(func2), p(p)
{}

double W1P_Norm::operator() (const double &x)
{
  double res1 = 1;
  double res2 = 1;
  for(unsigned int i = 0; i < p; i++)
  {
    res1 *= (func1->operator()(x) - func2->operator()(x));
    res2 *= (func1->Diff(x).dfdx - func2->Diff(x).dfdx);
  }
  return res1+res2;
}
  
  
}	//Interpolation