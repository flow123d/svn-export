

#include "iinterpolation.h"

namespace Interpolation
{
  
IInterpolation::IInterpolation()
{
  leftcond = new BCondition();
  rightcond = new BCondition();
  extrapolation_defined = false;
  x_defined = false;
  checks.resize(CH);
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

std::vector< double >* IInterpolation::GetNodes()
{
  return &x;
}

void IInterpolation::SetNodes(const std::vector< double > &x)
{
  MASSERT(x.size()>1,"vector x (nodes) contains to few nodes (at leats 2is needed)");
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
  if(cond == IInterpolation::LeftBC) leftcond->AddCond(derivate,value);
  if(cond == IInterpolation::RightBC) rightcond->AddCond(derivate,value);
}

void IInterpolation::AddCond(IInterpolation::BCkind cond, const Interpolation::BCondition& condition)
{
  if(cond == IInterpolation::LeftBC) leftcond = new BCondition(condition);
  if(cond == IInterpolation::RightBC) rightcond = new BCondition(condition);
}



}


