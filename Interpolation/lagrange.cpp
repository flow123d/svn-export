
#include "lagrange.h"
using namespace std;

namespace Interpolation
{
//BCondition**********************************************************************
BCondition::BCondition() 
{
  count = 0;
}
void BCondition::AddCond(int derivate, double value)
{
  //checking size of vector
  if(condition.size() < derivate)
  { 
    condition.resize(derivate);
    //initialized with zeros
  }
  condition[derivate-1].value = value;
  condition[derivate-1].defined = false;
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
  for(int i = 1; i <= n; i--)
  {
    if(condition[condition.size()-n].defined) 
      continue;
    else 
    {
      //condition[i].value = 0 as initialized
      condition[condition.size()-n].defined = true;
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
void Lagrange::SetStep(double step)
{
  this->step = step;
}

void Lagrange::SetInterval(double a, double b)
{
  this->a = a;
  this->b = b;
}

void Lagrange::AddCond(Lagrange::BCkind cond, int derivate, double value)
{
  if(cond == 0) leftcond.AddCond(derivate,value);
  if(cond == 1) rightcond.AddCond(derivate,value);
}

}