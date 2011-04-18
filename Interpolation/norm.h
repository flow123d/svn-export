

#ifndef NORM_H
#define NORM_H

#include "functorvaluebase.h"
#include "functorvalue.h"
#include "functordiffbase.h"
#include "interpolantbase.h"

namespace Interpolation
{

class LP_Norm	: public FunctorValueBase
{
  private:
   FunctorValueBase *func1;
   FunctorValueBase *func2;
    unsigned int p;
  
  public:
    
    LP_Norm( FunctorValueBase* func1, 
	     FunctorValueBase *func2,
	     unsigned int p);
    
    virtual double operator() ( const double &x );
  
};


class W1P_Norm	: public FunctorValueBase
{
  private:
    FunctorDiffBase *func1;
    FunctorDiffBase *func2;
    unsigned int p;
  
  public:
    
    W1P_Norm ( FunctorDiffBase *func1,
	       FunctorDiffBase *func2,
	       unsigned int p);
	       
    virtual double operator() ( const double &x );
};


}	//namespace Interpolation

#endif // NORM_H
