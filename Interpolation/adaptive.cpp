

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

  
}
