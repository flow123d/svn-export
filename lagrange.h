
#ifndef LAGRANGE_H
#define LAGRANGE_H

extern "C" {
#include "CLAPACK/blaswrap.h"
#include "CLAPACK/f2c.h"
#include "CLAPACK/clapack.h"
}

#include "functorvaluebase.h"
#include "interpolant.h"
#include "bandmatrixsolve.h"
#include <vector>
#include <iostream>

using namespace std;

namespace Interpolation
{

struct defvalue{
  double value;
  bool defined;
};
///class Boundary Condition
/** Defines a class that keeps boundary conditions for interpolation.
  */
class BCondition
{
  private:
  /** Conditions in the beginning of the interval.
    * value[0] = f'(a), ..., value[m] = f^{(m)}(a) */
  std::vector<defvalue> condition;
  
  ///amount of USER defined conditions
  int count;  
  
  public:
  ///constructor
  BCondition(void);
  
  /** Adds a boundary condition.
    * @param derivate sets an order of a derivate which is being added
    * @param value sets the value of derivate 
    */
  void AddCond(int derivate,double value);
  /// destroys all members in condition vectors
  void ClearCondition();
  /** fills the needed (according to the degree)
    * amount of conditions with zero
    * goes from the highest derivates */
  void AutoAdd(int number_of_conditions, int highest_order_of_derivate);
  ///returns conditions
  std::vector<defvalue> *GetCond();
  ///returns number of defined conditions
  int GetCount();
};

///class Lagrange
class Lagrange
{
  private:
    ///vector of nodes
    std::vector<double> x;
    ///vector of function values in the nodes x[i]
    std::vector<double> f;
    /** Interpolation is created for the interval \<a,b\> */
    double a,b;
    ///step that defines nodes layout
    double step;
    ///Boundary conditions - see the class BCondition
    BCondition leftcond;
    BCondition rightcond;
    
    ///class that solves band matrix using CLAPACK
    BandMatrixSolve *band;
    
  public:
    enum BCkind {LeftBC, RightBC};
    Lagrange(void);
    
    void SetInterval(double a, double b);
    void SetStep(double step);
    
    void AddCond(BCkind cond, int derivate, double value);
    
    template<char M>
    Interpolant* Interpolate(FunctorValueBase *func)
    {
      Interpolate<M>(func,a,b,step);
    }
    
    //Interpolation by Lagrange polynoms of Mth grade
    //on interval a,b with constant step
    template<char M>
    Interpolant* Interpolate(FunctorValueBase *func, double a, double b, double step)
    {
      //defining how the nodes of the interpolation will be placed
      //equidistant**********************************************
      int nodescount = (int)ceil((b-a)/step);	//number of nodes
      
      x.resize(nodescount);		//nodes
      f.resize(nodescount);		//function values in the nodes
  
      //filling the vector x and f
      for(int i = 0; i <= nodescount; i++)		
      {
	x[i] = a+step*i;
	f[i] = func->Value(x[i]);
      }
      
      //Boundary conditions**************************************
      //number of conditions to fill
      int cond_to_fill_count = M - 1 - leftcond.GetCount() - rightcond.GetCount();
      //division in halves
      leftcond.AutoAdd(ceil(cond_to_fill_count/2),M-1);
      rightcond.AutoAdd(floor(cond_to_fill_count/2),M-1);
      
      //solving band matrix**************************************
      
      //BandMatrixSolve(number of nodes,degree,number_of_leftcond,number_of_rightcond,number of functors);
      BandMatrixSolve *band = new BandMatrixSolve(x.size(),M,leftcond.GetCount(),rightcond.GetCount(),1);
  
      //boundary conditions on the left
      vector<defvalue> lc = &leftcond.GetCond();
      vector<defvalue> rc = &leftcond.GetCond();
      for(int i = 0; i < lc.size(); i++)
      {
	if(lc[i].defined)
	{
	  band->SetA(i,i+1,1.0);
	  band->SetB(i,0,lc[i].value);
	}
      }
      
      int pos = (x.size()-1)*(M+1)+2;
      for(int i = 0; i < rc.size(); i++)
      {
	if(rc[i].defined)
	{
	  
	  band->SetA(pos+i,pos+i+1,1.0);
	  band->SetB(pos+i,0,rc[i].value);
	}
      }
      
      //equations
      int ii;
      int number_of_leftcond = leftcond.GetCount();
      for(int i = 0; i < x.size()-1; i++)
      {
	ii = i*(M+1);
	band->SetA(ii+number_of_leftcond,ii,1.0);
	band->SetB(ii+number_of_leftcond,0,f[i]);
	band->SetB(ii+number_of_leftcond+1,0,f[i+1]);
	for(int k = 1; k <= M; k++)
	{
	  band->SetB(ii+number_of_leftcond+k+1,0,0.0);
	  for(int l = 0; l <= M; l++)
	  {
	    band->SetA(ii+k+number_of_leftcond,ii+l,pow(x[i],l));
	  }
	}
      }
      //2 equations for last polynomial
      ii = (x.size()-1)*(M+1);
      band->SetA(ii+number_of_leftcond,ii,1.0);
      band->SetB(ii+number_of_leftcond,0,f[f.size()-2]);
      band->SetB(ii+1+number_of_leftcond,0,f[f.size()-1]);
      for(int l = 0; l <= M; l++)
      {
	band->SetA(ii+1+number_of_leftcond,ii+l,pow(x[x.size()-1],l));
      }
  
      //boundary conditions on the right
      band->SetA(ii+2+number_of_leftcond,ii+M-1,1.0);
      band->SetB(ii+2+number_of_leftcond,0,0.0);
      
      
      
      //creation of polynoms for an interpolant******************
      vector<Polynomial* > polynomials (nodescount-1);
      for(int i = 0; i < nodescount-1; i++)
      {
	//creation, polynomial degree M
	polynomials[i] = new Polynomial(M);	
	//setting of polynomials intervals
	polynomials[i]->SetInterval(x[i],x[i+1]);
	//Gets results of dgbtrs and fills the vectors of coeficients of the polynomials
	polynomials[i]->SetCoeficients((double*)(b + i*(M+1)), M+1);
      }
      
      InterpolantBase *result; 
      result = new InterpolantEq(polynomials, step);
      
      //Extrapolation********************************************
      double *left = new double(M+1);
      double *right = new double(M+1);
      left[0] = f[0];
      right[0] = f[f.size()-1];
      for(int i = 1; i <= M; i++)
      {
	//
      }
      //result->SetExtrapolation(left, right);
      return result;
    }
    
};

}

#endif	//LAGRANGE_H
