
#ifndef LAGRANGE_H
#define LAGRANGE_H

#include <vector>
#include <iostream>

#ifndef DEB
#define DEB true
#endif

#include "functorvaluebase.h"
#include "interpolant.h"
#include "bandmatrixsolve.h"

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
  void AddCond(unsigned int derivate,double value);
  /// destroys all members in condition vectors
  void ClearCondition();
  /** fills the needed (according to the degree)
    * amount of conditions with zeros.
    * Goes from the highest derivates. 
    */
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
    BCondition *leftcond;
    BCondition *rightcond;
    
    ///class that solves band matrix using CLAPACK
    BandMatrixSolve *band;
    
  public:
    ///type that specifies kind of boundary condition - left or right
    enum BCkind {LeftBC, RightBC};
    
    ///constructor
    Lagrange(void);
    
    /** Sets the interval of interpolation.
      * @param a is left boundary
      * @param b i right bound
      */
    void SetInterval(double a, double b);
    
    ///Sets the step of interpolation - theplacement of nodes
    void SetStep(double step);
    
    ///Adds the boundary condition
    void AddCond(BCkind cond, unsigned int derivate, double value);
    
    template<char M>
    InterpolantBase* Interpolate(FunctorValueBase *func)
    {
      return Interpolate<M>(func,a,b,step);
    }
    
    //Interpolation by Lagrange polynoms of Mth grade
    //on interval a,b with constant step
    template<char M>
    InterpolantBase* Interpolate(FunctorValueBase *func, double a, double b, double step)
    {
      //defining how the nodes of the interpolation will be placed
      //equidistant**********************************************
      int nodescount = floor((double)(b-a)/step)+1;	//number of nodes
      if (DEB)
	cout << "number_of_nodes=" << ceil((double)(b-a)/step) << endl;
      
      x.resize(nodescount);		//nodes
      f.resize(nodescount);		//function values in the nodes
  
      //filling the vector x and f
      for(int i = 0; i <= nodescount; i++)		
      {
	x[i] = a+step*i;
	f[i] = func->Value(x[i]);
      }
      
      if (DEB)
	cout << nodescount << " nodes defined, x and f filled" << endl;
      
      //Boundary conditions**************************************
      //number of conditions to fill automatically
      int cond_to_fill_count = M - 1 - leftcond->GetCount() - rightcond->GetCount();
      if (DEB)
      {
	cout << "cond_to_fill_count=" << cond_to_fill_count << endl;
	cout << "cond_to_fill_LEFT=" << ceil(cond_to_fill_count/2.0) << endl;
	cout << "cond_to_fill_RIGHT=" << floor(cond_to_fill_count/2.0) << endl;
      }
      //division in halve
      leftcond->AutoAdd(ceil(cond_to_fill_count/2.0),M-1);
      rightcond->AutoAdd(floor(cond_to_fill_count/2.0),M-1);
      
      if (DEB)
	cout << "boundary conditions are checked" << endl;
      //solving band matrix**************************************
      //creation of the solver
      //BandMatrixSolve(integer n, integer ku, integer kl, integer nrhs);
      int n = (M+1)*(x.size()-1);
      int ku = M - leftcond->GetCount();
      int kl = 1 + leftcond->GetCount();
      BandMatrixSolve *band = new BandMatrixSolve(n,ku,kl,1);
      
      if (DEB)
      {
	cout << "x:" << endl;
	band->WrMatrix(x.data(),x.size(),1);
	cout << "f:" << endl;
	band->WrMatrix(f.data(),f.size(),1);
      }
      
      //filling the A matrix
      //boundary conditions on the left
      int number_of_leftcond = leftcond->GetCount();
      if (DEB)
	cout << "left_condcount=" << number_of_leftcond << endl;
      int condcount = 0;
      for(unsigned int i = 0; i < (*leftcond->GetCond()).size(); i++)
      {
	if((*leftcond->GetCond())[i].defined)
	{
	  band->SetA(condcount,i+1,Fact(i+1));
	  band->SetB(condcount,0,(*leftcond->GetCond())[i].value);
	  condcount++;
	}
      }
      
      //equations
      //pos is always the position of submatrix in matrix A
      //0...x.size()-2
      int pos = 0;	
      //goes through submatrixes
      for(unsigned int i = 0; i < x.size()-2; i++)
      {
	pos = i*(M+1);
	band->SetA(pos+number_of_leftcond,pos,1.0);		//eqn for x = 0 
	band->SetB(pos+number_of_leftcond,0,f[i]);		//rhs for x = 0
	//goes through lines in submatrix except the first one
	for(int k = 1; k <= M; k++)
	{
	  band->SetB(pos+number_of_leftcond+k,0,0.0);		//rhs=0 for P'[i]=P'[i-1]..P(M-1)[i]=P(M-1)[i-1]
	  if (k > 1)
	    band->SetA(pos+number_of_leftcond+k,pos+k+M,-Fact(k-1));	//eqn P'[i]=P'[i-1]..P(M-1)[i]=P(M-1)[i-1]
	  //goes through values of the line
	  for(int l = k-1; l <= M; l++)
	  {
	    //eqn P'[i]=P'[i-1]..P(M-1)[i]=P(M-1)[i-1]
	    band->SetA(pos+number_of_leftcond+k,pos+l,Fact(l)/Fact(l-k+1)*Power(x[i+1],l-k+1));
	  }
	}
	//overwrites the 0.0 which was written before in the for(k) cycle
	band->SetB(pos+number_of_leftcond+1,0,f[i+1]);
      }
     
      //2 equations for last polynomial
      pos = (x.size()-2)*(M+1);
      band->SetA(pos+number_of_leftcond,pos,1.0);
      band->SetB(pos+number_of_leftcond,0,f[f.size()-2]);
      band->SetB(pos+number_of_leftcond+1,0,f[f.size()-1]);
      band->SetA(pos+number_of_leftcond+1,pos,1.0);
      for(int l = 1; l <= M; l++)
      {
	band->SetA(pos+number_of_leftcond+1,pos+l,Power(x[x.size()-1],l));
      }
      
      //boundary conditions on the right
      condcount = 0;
      pos = n-(M+1);	
      if (DEB) 
	cout << "right_condcount=" << rightcond->GetCount() << endl;
      //n-(M+1) is position of last submatrix in the matrix A
      for(unsigned int i = 0; i < (*rightcond->GetCond()).size(); i++)
      {
	if((*rightcond->GetCond())[i].defined)
	{
	  //pos+2 is the position to start filling rc
	  band->SetA(pos+number_of_leftcond+condcount+2,pos+i+1,Fact(i+1));
	  band->SetB(pos+number_of_leftcond+condcount+2,0,(*rightcond->GetCond())[i].value);
	  condcount++;
	}
      }
      
      if (DEB)
	cout << "Matrix A filled" << endl;
      
      double *res;
      res = band->Solve();
      
      //creation of polynoms for an interpolant******************
      vector<Polynomial* > polynomials (nodescount-1);
      for(int i = 0; i < nodescount-1; i++)
      {
	//creation, polynomial degree M
	polynomials[i] = new Polynomial(M);	
	//setting of polynomials intervals
	polynomials[i]->SetInterval(x[i],x[i+1]);
	//Gets results of dgbtrs and fills the vectors of coeficients of the polynomials
	polynomials[i]->SetCoeficients((double*)(res + i*(M+1)), M+1);
	
	if (DEB)
	{
	  cout << "p[" << i << "]={";
	  for(int j = 0; j < M; j++)
	    cout <<  *(res + i*(M+1) + j) << "|";
	  cout <<  *(res + i*(M+1) + M) << "}" << endl;
	}
      }
      
      InterpolantBase *result; 
      result = new InterpolantEq(polynomials, step);
      
      /*
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
      //*/
      return result;
      
    }
    
    ///Math Power x^n
    template<class T>
    T Power(T x, unsigned int n)
    {
      T result = 1;
      for(unsigned int i = 0; i < n; i++)
	result = result*x;
      
      return result;
    }
    
    ///Factorial
    int Fact(int x)
    {     
      if (x > 1)
	return x*Fact(x-1);
      else
	return 1;
    }
    
};

}

#endif	//LAGRANGE_H
