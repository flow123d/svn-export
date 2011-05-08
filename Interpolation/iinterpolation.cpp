

#include "iinterpolation.h"

namespace Interpolation
{
  
IInterpolation::IInterpolation()
{
  leftcond = new BCondition();
  rightcond = new BCondition();
  extrapolation_defined = false;
  x_defined = false;
  leftcond_defined = false;
  rightcond_defined = false;
  checks.resize(CH);
}

IInterpolation::~IInterpolation(void )
{
  delete leftcond;
  delete rightcond;
}

void IInterpolation::SetDegree(const unsigned int& M)
{
  this->M = M;
  checks[3] = true;
}


void IInterpolation::SetExtrapolation(const unsigned char& left_degree, 
				      const unsigned char& right_degree)
{  
  this->left_degree = left_degree;
  this->right_degree = right_degree;
  extrapolation_defined = true;
}

void IInterpolation::SetInterval(const double& a, 
				 const double& b)
{
  MASSERT(a!=b,"Bounds overlap.");
  MASSERT(a<b,"a must be lower and b must be upper bound.");
  this->a = a;
  checks[0] = true;
  this->b = b;
  checks[1] = true;
}

void IInterpolation::SetNodes(const std::vector< double > &x)
{
  MASSERT(x.size()>1,"vector x (nodes) contains to few nodes (at leats 2 is needed)");
  this->x = x;
  x_defined = true;
  checks[2] = true;
}

void IInterpolation::SetStep(const double& step)
{
  MASSERT(step>0,"step cannot be negative or zero.");
  this->step = step;
  checks[2] = true;
}

void IInterpolation::AddCond(IInterpolation::BCkind cond, 
			     const unsigned int& derivate, 
			     const double& value)
{
  if(cond == IInterpolation::LeftBC) 
  {	
    leftcond->AddCond(derivate,value);
    leftcond_defined = true;
  }
  if(cond == IInterpolation::RightBC) 
  {
    rightcond->AddCond(derivate,value);
    rightcond_defined = true;
  }
}

void IInterpolation::AddCond(IInterpolation::BCkind cond, const Interpolation::BCondition& condition)
{
  if(cond == IInterpolation::LeftBC) 
  {
    leftcond = new BCondition(condition);
    leftcond_defined = true;
  }
  if(cond == IInterpolation::RightBC) 
  {
    rightcond = new BCondition(condition);
    rightcond_defined = true;
  }
}

void IInterpolation::ClearCondition(IInterpolation::BCkind cond)
{
  if(cond == IInterpolation::LeftBC) 
  {	
    leftcond->ClearCondition();
    leftcond_defined = false;
  }
  if(cond == IInterpolation::RightBC) 
  {
    rightcond->ClearCondition();;
    rightcond_defined = false;
  }
}


double IInterpolation::ComputeError( FunctorValueBase* f, InterpolantBase* g )
{
  double tot_err = 0;	// absolute error on <a,b>
  ErrorNum p_err;	// absolute/relative polynomial error on its interval
      
  p_err.i = 0;
  p_err.err = 0;
	
  for(unsigned long i = 0; i < g->GetCount(); i++ )
  {
    LP_Norm norm(f,g->GetPol(i),2);
    p_err.i = i;

    //absolute polynomial error
    p_err.err = sqrt(AdaptiveSimpson::AdaptSimpson( norm,
					      g->GetPol(i)->GetLower(), 
					      g->GetPol(i)->GetUpper(),
					      SIMPSON_TOLERANCE) );
    //increase the absolute total error
    tot_err += p_err.err;
    
    //writes absolute error on a single polynomial
    std::cout << "\t abs. p_err=" << p_err.err;
    
    //p_err convertion absolute -> relative (p_err/(xi+1 - xi))
    p_err.err /= (g->GetPol(i)->GetUpper()-g->GetPol(i)->GetLower());
    
    //writes relative error on a single polynomial
    std::cout << "\t rel. p_err=" << p_err.err << std::endl;
    
    pq.push(p_err); //puts in priority queue
  }
  
  /* writes relative and absolute total error
  std::cout << "\trelative err = "
    << tot_err/(g->GetA()-g->GetB()) 
    << "\tabsolute err = " << tot_err << std::endl;
  //*/
  
  return tot_err;	//returns absolute total error
}



}


