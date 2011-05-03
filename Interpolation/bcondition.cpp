
#include "bcondition.h"

using namespace std;

namespace Interpolation
{
BCondition::BCondition() 
{
  count = 0;
}

BCondition::BCondition(const BCondition& source)
  : condition(source.condition), count(source.count)
{
}

BCondition::~BCondition(void )
{
}


void BCondition::AddCond(const unsigned int &derivate, const double &value)
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
void BCondition::AutoAdd(const unsigned int &number_of_conditions, 
			 const unsigned int &highest_order_of_derivate)
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

}	//namespace Interpolation