#ifndef ADAPTIVESIMPSON_H
#define ADAPTIVESIMPSON_H

#include <math.h>

#include "functorvaluebase.h"
#include "interpolantbase.h"

namespace Interpolation
{

class AdaptiveSimpson
{
  private:
    
    double a,b;
    double tol;
    
    FunctorValueBase *func;
    InterpolantBase *interpolant;
    
    double Simpson ( const double& h, double &fa, double &fc, double &fb );

  
    double SimpsonAd( double (AdaptiveSimpson::*fce)(double&), 
		      const double& h, double &a, double &c, double &b,
		      double &fa, double &fc, double &fb, 
		      double &sx, double &tol );
  
    double L2norm ( double& x );
  
    double W1_1norm ( double& x );
    
  public:
  
    enum Norm{ L2 = 0, 
	       W1_1 = 1, 
	       W1_2 = 2};
	       
    AdaptiveSimpson( FunctorValueBase *func,
		     const double& a, const double& b, 
		     const double& tol );
		     
    void SetInterpolant( InterpolantBase *interpolant );
		    
    AdaptiveSimpson( FunctorValueBase *func, 	
		     InterpolantBase *interpolant, 
		     const double& a, const double& b, 
		     const double& tol );
	       
    double AdaptSimpson( AdaptiveSimpson::Norm type);
			
  
};
  
}

#endif	//ADAPTIVESIMPSON_H