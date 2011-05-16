
#ifndef ADAPTIVE_H
#define ADAPTIVE_H

#include <vector>
#include <iostream>
 
#include "massert.h"
#include "iinterpolation.h"
#include "functorvaluebase.h"
#include "interpolantbase.h"
#include "lagrange.h"

#ifndef DEB
#define DEB true
#endif

///defines how many intervals will be during single iteration of adaption divided
///0.05 ... 5% of all intervals
#define P 0.05	//% of intervals that are divided during adaption

namespace Interpolation
{
 
///Class Adaptive.
/** Uses nonadaptive classes in cycles for computing interpolation.
  * In every cycle computes the interpolation error -- as
  * a distance of tunctions (function and interpolant) using L_2 norm.
  * The integral is evaluated by static class AdaptiveSimpson.
  * After getting to an acceptable tolerance returns interpolant.
  * Inherits from IInterpolation
  */
class Adaptive : public IInterpolation
{
  private:
    ///Tolerance sets the max. error of interpolation.
    double tolerance;
      
    ///Checks whether all parameters are set.
    virtual bool Check( void ); 
    
    ///Uses step to count actual nodes.
    void FillNodes();
    
    ///Gets errors from pq and changes nodes.
    void AdaptNodes();
    
  public:
    
    ///A constructor.
    Adaptive( void );
    
    ///A destructor.
    ~Adaptive( void );
    
    ///Sets tolerance.
    void SetTolerance( const double& tolerance );
    
    ///Virtual method Interpolate
    /** Interpolation by Lagrange polynomials of Mth degree on interval a,b.
      * @param func is the functor that is being interpolated
      * @return pointer at InterpolantBase
      */
    virtual InterpolantBase* Interpolate ( Interpolation::FunctorValueBase &func );
  
};


}	//namespace Interpolation

#endif // ADAPTIVE_H
