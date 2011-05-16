

#ifndef IINTERPOLATION_H
#define IINTERPOLATION_H

#include <vector>
#include <iostream>
#include <queue>
#include <math.h>

#include "massert.h"
#include "functorvaluebase.h"
#include "interpolantbase.h"
#include "bcondition.h"
#include "norm.h"
#include "adaptivesimpson.h"

#ifndef DEB
#define DEB true
#endif

#define CH 4
#define SIMPSON_TOLERANCE 1e-6	//toletance for adaptive simpson

namespace Interpolation
{
  
///A structure for priority queue.
struct ErrorNum {
    double err;
    unsigned long i;
};

///A Condition function for priority queue.
class CompareErrorNum {
    public:
    bool operator()(ErrorNum& err1, ErrorNum& err2) // Returns true if t1 is earlier than t2
    {
       if (err1.err < err2.err) return true;
       return false;
    }
};
  
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
    bool x_defined;	//flag true if defined
    
    /** Interpolation is created for the interval a,b */
    double a,b;
    
    ///Step that defines nodes layout.
    double step;
    
    ///Degree of spline.
    unsigned int M;
    
    ///Boundary conditions - see the class BCondition
    bool leftcond_defined, rightcond_defined;	//flag true if defined
    BCondition *leftcond;
    BCondition *rightcond;
    
    ///degree of extrapolation at each of bounds
    bool extrapolation_defined;		//flag true if defined
    unsigned char left_degree;
    unsigned char right_degree;
    
    ///Checking
    /** In Lagrange example:
      * 0..a
      * 1..b  
      * 2..x vector is defined or step is defined
      * 3..degree M
      */
    std::vector<bool> checks;
    
    ///Keeps polynoms indexes in descending order 
    ///according to their distance from function.
    std::priority_queue< ErrorNum, 
			 std::vector<ErrorNum>, 
			 CompareErrorNum> 
			 pq;
  
  public:
    ///Type that specifies kind of boundary condition - left or right.
    enum BCkind {LeftBC, RightBC};
    
    ///A constructor.
    IInterpolation( void );
    
    ///A destructor.
    ~IInterpolation( void );
    
    ///Sets the degree of the interpolation.
    void SetDegree( const unsigned int& M);

    /** Sets the nodes.
      * Nodes (vector x) if defined are used instead of step !!!
      * In case of adaptive intepolation these are initial 
      * abscissas for first iteration.
      * @param x is vector of nodes
      */
    void SetNodes ( const std::vector<double> &x );

    /** Sets the interval of interpolation.
      * @param a is left boundary
      * @param b is right bound
      */
    void SetInterval ( const double &a, const double &b );

    /** Sets the step of interpolation - the placement of nodes.
      * Nodes (vector x) if defined are used instead of step !!!
      * In case of adaptive intepolation it is initial 
      * step for first iteration.
      * @param step is the step between nodes
      */
    void SetStep ( const double& step );
    
    ///Returns the degree of interpolation.
    inline unsigned int GetDegree()
    { return M; }

    ///Returns the vector x -- nodes.
    inline std::vector<double> *GetNodes()
    { return &x; }
    
    ///Returns A -- left bound of the interval.
    inline double GetA()
    { return a; }
    
    ///Returns b -- right bound of the interval.
    inline double GetB()
    { return b; }
    
    ///Returns step.
    inline double GetStep()
    { return step; }
      
    ///Returns the priority_queue with polynomial indexes
    ///sorted descendently according to their distance from function.
    inline std::priority_queue< ErrorNum, 
				std::vector<ErrorNum>, 
				CompareErrorNum> 
				*GetPolynomialErrors()
    { return &pq; }
    
    ///Deletes the priority queue pq.
    void ClearPolynomialErrors();
    
    /** Adds the boundary condition.
      * @param cond is type of condition meaning left or 
      *		right side of the interval
      * @param derivate
      * @param value is the value of the derivate
      */
    void AddCond ( BCkind cond, const unsigned int& derivate, 
		   const double& value );
    
    /** Adds the boundary condition.
      * @param cond is type of condition meaning left or 
      *		right side of the interval
      * @param condition is an object of condition
      */
    void AddCond ( BCkind cond, const BCondition& condition );
    
    ///clears left or right hand side boundary conditions
    void ClearCondition( BCkind cond );

	/** Extrapolation depends on the degree of BC by default
      * or can be entered explicitly.
      * @param left_degree is a degree of extrapolation on the left 
      * @param right_degree is a degree of extrapolation on the right 
      */
    void SetExtrapolation( const unsigned char& left_degree, 
			   const unsigned char& right_degree );

	/** Computes the distance of two functions.
	  * Especially to get the difference between function 
	  * and interpolantion.
	  */
    double ComputeError( FunctorValueBase* f, InterpolantBase* g);

    ///Virtual method checks if all needed parameters are defined.
    virtual bool Check() = 0;
    
    ///Virtual method that computes interpolation and returns interpolant.
    virtual InterpolantBase* Interpolate( FunctorValueBase& f) = 0;

};

}
#endif // IINTERPOLATION_H
