
#ifndef ADAPTIVE_H
#define ADAPTIVE_H

#ifndef DEB
#define DEB true
#endif

#include <vector>
#include <iostream>
 
#include "iinterpolation.h"
#include "massert.h"
#include "functorvaluebase.h"
#include "interpolantbase.h"
#include "lagrange.h"


namespace Interpolation
{
 
///Class Adaptive.
/** Uses nonadaptive classes in cycles for computing interpolation.
  * In every cycle computes the interpolation error -- as
  * a distance of tunctions (function and interpolant) using L_2 norm.
  * The integral is evaluated by static class AdaptiveSimpson.
  * After getting to an acceptable tolerance returns interpolant.
  * Inherits from IInterpolation
  */
class Adaptive : public IInterpolation
{
  private:
    ///Tolerance sets the max. error of interpolation.
    double tolerance;
      
    ///Checks whether all parameters are set.
    bool Check( void ); 
    
  public:
    
    ///A constructor.
    Adaptive( void );
    
    ///A destructor.
    ~Adaptive( void );
    
    ///Sets tolerance.
    void SetTolerance( const double& tolerance );
    
    ///Virtual method Interpolate
    /** Interpolation by Lagrange polynomials of Mth degree on interval a,b.
      * @param func is the functor that is being interpolated
      */
    virtual InterpolantBase* Interpolate ( Interpolation::FunctorValueBase &func )
    {
      MASSERT(Check(),"Not all parameters has been set.");
      
      InterpolantBase *result;
      
      
      //setting initial Lagrange interpolation:
      Lagrange *lag = new Lagrange();
      if (x_defined) lag->SetNodes(x);
      else 
      {
	//x_defined false -> step must be defined; is checked in Check() before
	//equidistant**********************************************
	MASSERT((b-a) >= step ,"The parameter step is larger than the lenght of the interval <a,b>.");
	int nodescount = floor((double)(b-a)/step)+1;	//number of nodes
	if (DEB)
	  std::cout << "number_of_nodes=" << floor((double)(b-a)/step)+1 << std::endl;
    
	x.resize(nodescount);	//nodes
  
	//filling the vector x and f
	for(long i = 0; i < nodescount; i++)		
	{
	  x[i] = a+step*i;
	}  
    
	//finish the interval
	if(x[x.size()-1] < b)
	{
	  x.push_back(b);
	  //nodescount++;
	}
	lag->SetNodes(x);
      }
      
      
      //these parameters of interpolation will not change during adaption:
      lag->SetDegree(M);
      lag->SetInterval(a,b);
      if(leftcond_defined) lag->AddCond(IInterpolation::LeftBC, *leftcond);
      if(rightcond_defined) lag->AddCond(IInterpolation::RightBC, *rightcond);
      if(extrapolation_defined) lag->SetExtrapolation(left_degree, right_degree);
      
      
      //double max_rel_err = tolerance / (b-a); //relative error
      double tot_err = 0;	// absolute error on <a,b>
      
      int n=5;	//iteration cycle of adaption
      while (n)
      {
	result = lag->Interpolate(func);	//interpolation with actual nodes
	
	tot_err = ComputeError(&func,result);	//computes error of interpolation
	
	//writes the relative and absolute total error in each iteration
	std::cout << "interation = " << n << "\trel. err = " << tot_err/(b-a) << "\tabs. err = " << tot_err << std::endl;
	
	if (tot_err <= tolerance) break;	//end of adaption
	else
	{ //continue adaption
	  //changing nodes
	  
	  for(int k = 0; k < 1; k++)	//divides k=1 intervals
	  {
	    unsigned long i = pq.top().i;
	    std::cout << "getting top of priority _queued: i=" << i << std::endl;
	    pq.pop();
	    x.insert(x.begin()+i+1, (x[i+1]+x[i])*0.5);
	  }
	  
	  lag->SetNodes(x);
	}
	n--;
      }
      
      delete lag;

      return result;   
    }
  
};


}	//namespace Interpolation

#endif // ADAPTIVE_H
