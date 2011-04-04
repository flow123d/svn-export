
#ifndef ADAPTIVE_H
#define ADAPTIVE_H

#include <vector>
#include <iostream>

#ifndef DEB
#define DEB true
#endif
 
#include "iinterpolation.h"
#include "massert.h"
#include "functorvaluebase.h"
#include "interpolantbase.h"
#include "lagrange.h"
#include "adaptivesimpson.h"


namespace Interpolation
{
  
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
    InterpolantBase* Interpolate ( Interpolation::FunctorValueBase *func )
    {
      MASSERT(Check(),"Not all parameters has been set.");
      
      double simpsontol = 1e-6;	//tolerance for evaluating of adaptivesimpson
      double err = tolerance + 1.0; //err must be > tolerance at first
      
      InterpolantBase *result;
      AdaptiveSimpson *norm = new AdaptiveSimpson(func,a,b,simpsontol);
      
      //setting initial Lagrange interpolation:
      Lagrange *lag = new Lagrange();
      if (x_defined) lag->SetNodes(x);
      else lag->SetStep(step);
      
      //these parameters of interpolation will not change during adaption:
      lag->SetInterval(a,b);
      lag->AddCond(IInterpolation::LeftBC, *leftcond);
      lag->AddCond(IInterpolation::RightBC, *rightcond);
      lag->SetExtrapolation(left_degree, right_degree);
      
      int n=1;	//iteration cycle of adaption
      while (1)
      {
	result = lag->Interpolate<M>(func);	//interpolation with actual nodes
	norm->SetInterpolant(result);
	err = norm->AdaptSimpson(AdaptiveSimpson::L2);
	std::cout << "interation = " << n << "  err = " << err<< std::endl;
	
	if (err <= tolerance) break;	//end of adaption
	else
	{ //continue adaption
	  x = *(lag->GetNodes());
	  //changing nodes
	  
	  lag->SetNodes(x);
	}
      }

      return result;
    }
  
};


}

#endif // ADAPTIVE_H
