

#ifndef IINTERPOLATION_H
#define IINTERPOLATION_H

#include <vector>
#include <iostream>

#include "massert.h"
#include "functorvaluebase.h"
#include "interpolantbase.h"
#include "bcondition.h"

#define CH 3

namespace Interpolation
{
  
///class IInterpolation
/** Defines basic methods to control the interpolation.
  * meaning to set orget the parameters (interval,nodes
  * step, boundary conditions, extrapolation).
  * In case of adaptive interpolation are nodes/step
  * taken as initial input for interation.
  */
class IInterpolation
{
  protected:
    
    ///vector of nodes
    std::vector<double> x;
    
    ///vector x is or is not defined by user (step is not or is needed)
    ///nodes if defined are used instead of step !!!
    bool x_defined;
    
    /** Interpolation is created for the interval \<a,b\> */
    double a,b;
    
    ///step that defines nodes layout
    double step;
    
    ///Boundary conditions - see the class BCondition
    BCondition *leftcond;
    BCondition *rightcond;
    
    ///degree of extrapolation at each of bounds
    bool extrapolation_defined;
    unsigned char left_degree;
    unsigned char right_degree;
    
    ///checking
    /** In Lagrange example:
      * 0..a
      * 1..b  
      * 2..x vector is defined or step is defined
      */
    std::vector<bool> checks;
  
  public:
    ///type that specifies kind of boundary condition - left or right
    enum BCkind {LeftBC, RightBC};
    
    ///constructor
    IInterpolation( void );
    
    /** Sets the interval of interpolation.
      * @param a is left boundary
      * @param b is right bound
      */
    void SetInterval ( const double &a, const double &b );
    
    ///returns the vector x -nodes
    std::vector<double> *GetNodes();
    
    /** Sets the nodes
      * Nodes (vector x) if defined are used instead of step !!!
      * In case of adaptive intepolation these are initial 
      * abscissas for first iteration.
      * @param x is vector of nodes
      */
    void SetNodes ( const std::vector<double> &x );
    
    /** Adds the boundary condition
      * @param cond is type of condition meaning left or 
      *		right side of the interval
      * @param derivate
      * @param value is the value of the derivate
      */
    void AddCond ( BCkind cond, const unsigned int& derivate, 
		   const double& value );
    
    /** Adds the boundary condition
      * @param cond is type of condition meaning left or 
      *		right side of the interval
      * @param condition is an object of condition
      */
    void AddCond ( BCkind cond, const BCondition& condition );
    
    /** Sets the step of interpolation - the placement of nodes
      * Nodes (vector x) if defined are used instead of step !!!
      * In case of adaptive intepolation it is initial 
      * step for first iteration.
      * @param step is the step between nodes
      */
    void SetStep ( const double& step );
    
    /** Extrapolation depends on the degree of BC by default
      * or can be entered explicitly.
      * @param left_degree is a degree of extrapolation on the left 
      * @param right_degree is a degree of extrapolation on the right 
      */
    void SetExtrapolation ( const unsigned char& left_degree, 
			    const unsigned char& right_degree );
};

}
#endif // IINTERPOLATION_H
