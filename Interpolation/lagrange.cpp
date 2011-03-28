
#include "lagrange.h"
using namespace std;

namespace Interpolation
{
//BCondition**********************************************************************
BCondition::BCondition() 
{
  count = 0;
}
void BCondition::AddCond(unsigned int derivate, double value)
{
  //checking size of vector
  if(condition.size() < derivate)
  { 
    condition.resize(derivate);
    //initialized with zeros
  }
  condition[derivate-1].value = value;
  condition[derivate-1].defined = true;
  count++;
}
void BCondition::AutoAdd(int number_of_conditions, int highest_order_of_derivate)
{
  int n;
  if(number_of_conditions > highest_order_of_derivate) 
  { n = highest_order_of_derivate;
    cout << "Warning: Number of conditions to add was decreased according to the highest order of derivate." << endl;
  }
  else
    n = number_of_conditions;
  condition.resize(highest_order_of_derivate);
  for(int i = 1; i <= n; i++)
  {
    if(condition[condition.size()-i].defined) 
    {
      continue;
    }
    
    else 
    {
      //condition[i].value = 0 as initialized
      condition[condition.size()-i].defined = true;
      count++;
    }
  }
}

void BCondition::ClearCondition()
{
  condition.clear();
  count = 0;
}

vector<defvalue>* BCondition::GetCond()
{
  return &condition;
}

int BCondition::GetCount()
{
  return count;
}

  
//INTERPOLATION*******************************************************************
Lagrange::Lagrange(void )
{
  leftcond = new BCondition();
  rightcond = new BCondition();
}

void Lagrange::SetStep(double step)
{
  this->step = step;
}

void Lagrange::SetInterval(double a, double b)
{
  this->a = a;
  this->b = b;
}

void Lagrange::AddCond(Lagrange::BCkind cond, unsigned int derivate, double value)
{
  if(cond == Lagrange::LeftBC) leftcond->AddCond(derivate,value);
  if(cond == Lagrange::RightBC) rightcond->AddCond(derivate,value);
}

}