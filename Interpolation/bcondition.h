
#ifndef BCONDITION_H
#define BCONDITION_H

#include <vector>
#include <iostream>

#include "massert.h"

#ifndef DEB
#define DEB true
#endif

namespace Interpolation
{
/** Contains value and boolean information
  * whether the value is defined by user.
  */
struct defvalue
{
  double value;
  bool defined;
};
  
///Class BCondition -- Boundary Condition
/** Defines a class that keeps boundary conditions for interpolation.
  * Has methods to control the conditions (Add, AutoAdd,GetCond,GetCount).
  */
class BCondition
{    
  private:
    /** Conditions in the beginning of the interval.
      * value[0] = f'(a), ..., value[m] = f^{(m)}(a) 
      */
    std::vector<defvalue> condition;
  
  ///Amount of USER defined conditions.
    unsigned int count;  
  
  public:
    ///A constructor.
    BCondition( void );
    
    ///A copy constructor.
    BCondition( const BCondition& source );
    
    ///A destructor.
    ~BCondition( void );
  
    /** Adds a boundary condition.
      * @param derivate sets an order of a derivate 
      *			which is being added
      * @param value sets the value of derivate 
      */
    void AddCond( const unsigned int &derivate, const double &value );
  
    ///Destroys all members in condition vectors.
    void ClearCondition();
    
    /** Fills the needed (according to the degree)
      * amount of conditions with zeros.
      * Goes from the highest derivates. 
      * @param number_of_conditions sets the amount of conditions to be added
      * @param highest_order_of_derivates sets the highest order of derivate 
      */
    void AutoAdd( const unsigned int &number_of_conditions, 
		  const unsigned int &highest_order_of_derivate );
    
    ///returns pointer to vector of conditions
    std::vector<defvalue> *GetCond();
  
    ///returns number of defined conditions
    unsigned int GetCount();
};

}	//namespace Interpolation

#endif // BCONDITION_H