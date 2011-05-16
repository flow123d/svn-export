

#include "adaptive.h"

namespace Interpolation
{

Adaptive::Adaptive(void ) : IInterpolation()
{
  //adds boolean information if tolerance has been set
  checks.push_back(false);
}

Adaptive::~Adaptive(void )
{
}


bool Adaptive::Check()
{
  /*
  0..a
  1..b
  2..step/tolerance
  3..degree M
  4..tolerance
  */
  bool res = true;
  for(unsigned int i = 0; i < CH+1; i++)
  {
    if (checks[i]) continue;
    switch(i)
    {
      case 0:  std::cerr << "left boundary undefined" << std::endl;
	break;
      case 1:  std::cerr << "right boundary undefined" << std::endl;
	break;
      case 2:  std::cerr << "neither vector x nor step undefined" << std::endl;
	break;
      case 3:  std::cerr << "the degree of interpolation undefined" << std::endl;
	break;
      case 4:  std::cerr << "tolerance undefined" << std::endl;
	break;
      default: std::cerr << "Lagrange::Check() found undefined parameter" << std::endl;
    }
    res = false;
  }
  return res;
}

void Adaptive::SetTolerance(const double& tolerance)
{
  MASSERT(tolerance>0,"tolerance cannot be negative or zero.");
  this->tolerance = tolerance;
  checks[4] = true;
}

void Adaptive::AdaptNodes()
{
  //changing nodes
  long int k_max;
  k_max = ceil(P*pq.size());	//P % of intervals will be divided
  for(int k = 0; k < k_max; k++)	//divides k=1 intervals
  {
    unsigned long i = pq.top().i;
    if(DEB)
      std::cout << "getting top of priority _queued: i=" << i << std::endl;
    pq.pop();
    x.insert(x.begin()+i+1, (x[i+1]+x[i])*0.5);
  }
}

void Adaptive::FillNodes()
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
}


InterpolantBase* Adaptive::Interpolate(FunctorValueBase& func)
{
  MASSERT(Check(),"Not all parameters has been set.");
      
  InterpolantBase *result;
      
  ClearPolynomialErrors();
      
  //setting initial Lagrange interpolation:
  Lagrange *lag = new Lagrange();
  if (x_defined) lag->SetNodes(x);
  else 
  {
    FillNodes();
    lag->SetNodes(x);
  }      
      
  //these parameters of interpolation will not change during adaption:
  lag->SetDegree(M);
  lag->SetInterval(a,b);
  if(leftcond_defined) lag->AddCond(IInterpolation::LeftBC, *leftcond);
  if(rightcond_defined) lag->AddCond(IInterpolation::RightBC, *rightcond);
  if(extrapolation_defined) lag->SetExtrapolation(left_degree, right_degree);
      
  double tot_err = 0;	//absolute error on <a,b>
  //absolute error on <a,b> in previous iteration
  double tot_err_prev = 9.99999999999999e+99;	
      
  int n=0;	//iteration cycle of adaption
  while (1)
  {
    result = lag->Interpolate(func);	//interpolation with actual nodes
    
    ClearPolynomialErrors();	//deleting priority queue
      
    tot_err = ComputeError(&func,result);	//computes error of interpolation
	
    //writes the relative and absolute total error in each iteration
    if(DEB)
      std::cout << "interation = " << n << "\trel. err = " << tot_err/(b-a) 
		<< "\tabs. err = " << tot_err << std::endl;
	
    if(tot_err > tot_err_prev) 
    {	
      std::cout << "FAILED" << std::endl;
      std::cout << "error has increased -> end of iteration"<< std::endl;
      break;
    }
    
    tot_err_prev = tot_err;
	
    if (tot_err <= tolerance) 
    {
      std::cout << "interation = " << n << "\trel. err = " << tot_err/(b-a) 
		<< "\tabs. err = " << tot_err << std::endl;
      std::cout << "SUCCESS" << std::endl;
      break;	//end of adaption
    }
    else
    { //continue adaption 
      AdaptNodes();
      lag->SetNodes(x);	  
    }
    n++;
  }
      
  delete lag;
  return result;   
}


  
}
