
#ifndef ADAPTIVE_H
#define ADAPTIVE_H

#include <vector>
#include <iostream>
#include <math.h>
#include <queue>

#ifndef DEB
#define DEB true
#endif
 
#include "iinterpolation.h"
#include "massert.h"
#include "functorvaluebase.h"
#include "interpolantbase.h"
#include "lagrange.h"
#include "adaptivesimpson.h"
#include "norm.h"


namespace Interpolation
{

struct ErrorNum {
    double err;
    unsigned long i;
};

class CompareErrorNum {
    public:
    bool operator()(ErrorNum& err1, ErrorNum& err2) // Returns true if t1 is earlier than t2
    {
       if (err1.err < err2.err) return true;
       return false;
    }
};
  
class Adaptive : public IInterpolation
{
  private:
    ///tolerance sets the max. error of interpolation
    double tolerance;
      
     ///Checks whether all parameters are set
    bool Check(); 
    
  public:
    
    ///constructor
    Adaptive(void );
    
    ///Set tolerance
    void SetTolerance(double tolerance);
    
    /** Interpolation by Lagrange polynomials of Mth degree on interval a,b.
	* Template parameter is the degree of polynomials
	* @param func is the functor that is being interpolated
	*/
    template<unsigned char M>
    InterpolantBase* Interpolate ( Interpolation::FunctorValueBase &func )
    {
      MASSERT(Check(),"Not all parameters has been set.");
      
      InterpolantBase *result;
      
      //setting initial Lagrange interpolation:
      Lagrange *lag = new Lagrange();
      if (x_defined) lag->SetNodes(x);
      else lag->SetStep(step);
      
      //these parameters of interpolation will not change during adaption:
      lag->SetInterval(a,b);
      lag->AddCond(IInterpolation::LeftBC, *leftcond);
      lag->AddCond(IInterpolation::RightBC, *rightcond);
      lag->SetExtrapolation(left_degree, right_degree);
      
      double simpsontol = 1e-6;	//tolerance for evaluating of adaptivesimpson
      double max_rel_err = tolerance / (b-a); //relative error
      double tot_err;	// absolute error on <a,b>
      ErrorNum p_err;	// relative polynomial error on its interval
      
      int n=2;	//iteration cycle of adaption
      while (n)
      {
	result = lag->Interpolate<M>(func);	//interpolation with actual nodes
	
	tot_err = 0;
	p_err.i = 0;
	p_err.err = 0;
	std::priority_queue<ErrorNum, std::vector<ErrorNum>, CompareErrorNum> pq;
	
	for(unsigned long i = 0; i < result->GetCount(); i++ )
	{
	  LP_Norm norm(&func,result->GetPol(i),2);
	  // p_err = e/(b-a)
	  p_err.i = i;
	  p_err.err = sqrt(AdaptiveSimpson::AdaptSimpson( norm,
						     result->GetPol(i)->GetLower(), 
						     result->GetPol(i)->GetUpper(),
						     simpsontol) )
		 / (result->GetPol(i)->GetUpper()-result->GetPol(i)->GetLower());
	  std::cout << "\t p_err=" << p_err.err << std::endl;
	  if(p_err.err > max_rel_err) pq.push(p_err);
	  tot_err += p_err.err;
	}
	
	std::cout << "interation = " << n << "  err = " << tot_err << std::endl;
	
	
	if (tot_err <= tolerance) break;	//end of adaption
	else
	{ //continue adaption
	  x = *(lag->GetNodes());
	  //changing nodes
	  
	  for(int k = 0; k < 1; k++)
	  {
	    unsigned long i = pq.top().i;
	    std::cout << "getting top of priority _queued: i=" << i << std::endl;
	    pq.pop();
	    x.insert(x.begin()+i+1, (x[i+1]+x[i])*0.5);
	  }
	  
	  lag->SetNodes(x);
	}
	n--;
	//*/
      }

      return result;
    }
  
};


}

#endif // ADAPTIVE_H
