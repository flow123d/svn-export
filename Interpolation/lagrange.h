
#ifndef LAGRANGE_H
#define LAGRANGE_H

#include <vector>
#include <iostream>

#ifndef DEB
#define DEB true
#endif

#include "massert.h"

#include "iinterpolation.h"
#include "functorvaluebase.h"
#include "interpolantbase.h"
#include "interpolant.h"
#include "bandmatrixsolve.h"

namespace Interpolation
{
///class Lagrange
class Lagrange	: public IInterpolation
{
  private:
    
    ///vector of function values in the nodes x[i]
    std::vector<double> f;
    
    ///class that solves band matrix using CLAPACK
    BandMatrixSolve *band;
    
    ///Checks whether all parameters are set
    bool Check(); 
    
    /** Evaluates function in nodes.
      * @param func is the functor that isbeing interpolated
      */
    void SetFunctionvalues ( Interpolation::FunctorValueBase *func );
    
    ///Fills the BC, computes dimensions a then create the BandMatrixSolve object
    /** @param M is a degree of polynomials
      */
    void CreateBandMatrix ( const unsigned char &M );
    
    ///Puts equations of boundary conditions into the matrix A
    /** @param M is a degree of polynomials
      */
    void PutBC ( const unsigned char &M );
    
    ///Puts equations into the matrix A
    void PutEquations ( const unsigned char &M );
    
    ///Creates the interpolant from previous results
    InterpolantBase* CreateInterpolant ( double *bandres, const unsigned char &M );
    
    ///Math Power x^n
    template<class T>
    T Power ( T x, unsigned int n )
    {
      T result = 1;
      for(unsigned int i = 0; i < n; i++)
      {
	result = result*x;
      }
      return result;
    }
    
    ///Factorial
    long Fact ( long x )
    {     
      if (x > 1)
	return x*Fact(x-1);
      else
	return 1;
    }
    
  public:
    
    ///constructor
    Lagrange ( void );
    
    /** Interpolation by Lagrange polynomials of Mth degree on interval a,b.
      * Template parameter is the degree of polynomials
      * @param func is the functor that is being interpolated
      */   
    template<unsigned char M>
    InterpolantBase* Interpolate ( Interpolation::FunctorValueBase *func )
    {
      MASSERT(Check(),"Not all parameters has been set.");
      
      SetFunctionvalues(func);  
      CreateBandMatrix(M);
      if (DEB)
      {
	std::cout << "x:" << std::endl;
	band->WrMatrix(x.data(),x.size(),1);
	std::cout << "f:" << std::endl;
	band->WrMatrix(f.data(),f.size(),1);
      }
      
      PutBC(M);  
      PutEquations(M);
      
      if (DEB)
	std::cout << "Matrix A filled" << std::endl;
      
      //result of the band matrix
      double *bandres;
      bandres = band->Solve();
      
      InterpolantBase *result = CreateInterpolant(bandres, M);
      
      //Extrapolation********************************************
      if(!extrapolation_defined)
      {
	left_degree = leftcond->GetCount();
	right_degree = rightcond->GetCount();
      }
      MASSERT((left_degree <= M),"The degree of extrapolation > the degree of the interpolation.");
      MASSERT((right_degree <= M),"The degree of extrapolation > the degree of the interpolation.");
      
      result->SetExtrapolation(left_degree,right_degree);
      
      return result;
      
    }
    
};

}	//namespace IInterpolation

#endif	//LAGRANGE_H
