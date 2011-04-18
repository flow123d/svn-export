#ifndef ADAPTIVESIMPSON_H
#define ADAPTIVESIMPSON_H

#include <math.h>
#include <iostream>

#include "functorvaluebase.h"

namespace Interpolation
{

class AdaptiveSimpson
{
  private:

    static double Simpson ( const double& h, const double &fa, const double &fc, const double &fb );

    static double SimpsonAd( FunctorValueBase &func, 
		      const double& h, const double &a, const double &c, const double &b,
		      const double &fa, const double &fc, const double &fb, 
		      const double &sx, const double &tol, long &recursion );
      
  public:
	       
    static double AdaptSimpson( FunctorValueBase &func,
		     const double& a, const double& b, 
		     const double& tol );	    		
};
  
}

#endif	//ADAPTIVESIMPSON_H