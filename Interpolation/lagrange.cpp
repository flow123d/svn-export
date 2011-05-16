
#include "lagrange.h"

using namespace std;

namespace Interpolation
{
Lagrange::Lagrange ( void ) : IInterpolation()
{
}

Lagrange::~Lagrange(void )
{
}


bool Lagrange::Check()
{
  /*
  0..a
  1..b
  2..step/tolerance
  3..degree M
  */
  
  bool res = true;
  for(unsigned int i = 0; i < CH; i++)
  {
    if (checks[i]) continue;
    switch(i)
    {
      case 0:  cerr << "left boundary undefined" << endl;
	break;
      case 1:  cerr << "right boundary undefined" << endl;
	break;
      case 2:  cerr << "neither vector x nor step undefined" << endl;
	break;
      case 3:  cerr << "the degree of interpolation undefined" << endl;
	break;
      default: cerr << "Lagrange::Check() found undefined parameter" << endl;
    }
    res = false;
  }
  return res;
}

void Lagrange::SetFunctionvalues ( Interpolation::FunctorValueBase& func )
{
  int nodescount;
  
  if (x_defined)	//vector x is defined from outside
  {
    MASSERT(x.size()>1,"vector x (nodes) contains to few nodes (at leats 2 is needed)");
    MASSERT(x[0] == a,"in vector of nodes x[0] should be equal to a");
    MASSERT(x[x.size()-1] == b,"in vector of nodes x[n] should be equal to b");
    
    nodescount = x.size();
    f.resize(nodescount);	//function values in the nodes
  
    //filling the vector x and f
    for(int i = 0; i < nodescount; i++)		
    {
      f[i] = func(x[i]);
    }  
  }
  else
  {
    //x_defined false -> step must be defined; is checked in Check() before
    //equidistant**********************************************
    MASSERT((b-a) >= step ,"The parameter step is larger than the lenght of the interval <a,b>.");
    nodescount = floor((double)(b-a)/step)+1;	//number of nodes
    if (DEB)
      cout << "number_of_nodes=" << floor((double)(b-a)/step)+1 << endl;
    
    x.resize(nodescount);	//nodes
    f.resize(nodescount);	//function values in the nodes
  
    //filling the vector x and f
    for(long i = 0; i < nodescount; i++)		
    {
      x[i] = a+step*i;
      f[i] = func(x[i]);
    }  
    
    //finish the interval
    if(x[x.size()-1] < b)
    {
      x.push_back(b);
      f.push_back(func(b));
      nodescount++;
    }
  }
      if (DEB)
	cout << nodescount << " nodes defined, x and f filled" << endl;
}

void Lagrange::CreateBandMatrix ()
{
  //number of conditions to fill automatically
  int n,ku,kl;
  if(M > 1)	//constant and linear has no BC
  {  
    MASSERT(leftcond->GetCount()+rightcond->GetCount() < M,"Too many boundary conditions defined.");    
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
    n = (M+1)*(x.size()-1);
    ku = M - leftcond->GetCount();
    kl = 1 + leftcond->GetCount();
  }
  else	//for M=0, meaning constant interoplation
  {
    leftcond->ClearCondition();
    rightcond->ClearCondition();
    if(DEB) cout << "Boundary conditions are not possible and all of them will not be used." << endl;
    n = (M+1)*(x.size()-1);
    ku = 0;
    kl = M;
  }
    
  band = new BandMatrixSolve(n,ku,kl,1);
}

void Lagrange::PutBC ()
{
  //boundary conditions on the left
      int number_of_leftcond = leftcond->GetCount();
      if (DEB)
	cout << "left_condcount=" << number_of_leftcond << endl;
      int condcount = 0;
      for(uint i = 0; i < (*leftcond->GetCond()).size(); i++)
      {
	if((*leftcond->GetCond())[i].defined)
	{
	  band->SetA(condcount,i+1,Fact(i+1));
	  band->SetB(condcount,0,(*leftcond->GetCond())[i].value);
	  condcount++;
	}
      }
      
      //boundary conditions on the right
      //n-(M+1) is position of last submatrix in the matrix A
      int pos = band->GetN() - (M+1);	
      if (DEB) 
	cout << "right_condcount=" << rightcond->GetCount() << endl;
      condcount = 0;
      for(uint i = 0; i < (*rightcond->GetCond()).size(); i++)
      {
	if((*rightcond->GetCond())[i].defined)
	{
	  //pos+2 is the position to start filling rc
	  band->SetA(pos+number_of_leftcond+condcount+2,pos+i+1,Fact(i+1));
	  band->SetB(pos+number_of_leftcond+condcount+2,0,(*rightcond->GetCond())[i].value);
	  condcount++;
	}
      }
}

void Lagrange::PutEquations ()
{
  //pos is always the position of submatrix in matrix A
      //0...x.size()-2
      int pos = 0;	
      int number_of_leftcond = leftcond->GetCount();
         
      //goes through submatrixes
      for(unsigned long i = 0; i < x.size()-2; i++)
      {
	pos = i*(M+1);
	band->SetA(pos+number_of_leftcond,pos,1.0);		//eqn for x = 0 
	band->SetB(pos+number_of_leftcond,0,f[i]);		//rhs for x = 0
	//goes through lines in submatrix except the first one
	for(unsigned int k = 1; k <= M; k++)
	{
	  band->SetB(pos+number_of_leftcond+k,0,0.0);		//rhs=0 for P'[i]=P'[i-1]..P(M-1)[i]=P(M-1)[i-1]
	  if (k > 1)
	    band->SetA(pos+number_of_leftcond+k,pos+k+M,-Fact(k-1));	//eqn P'[i]=P'[i-1]..P(M-1)[i]=P(M-1)[i-1]
	  //goes through values of the line
	  for(unsigned int l = k-1; l <= M; l++)
	  {
	    //eqn P'[i]=P'[i-1]..P(M-1)[i]=P(M-1)[i-1]
	    band->SetA(pos+number_of_leftcond+k,pos+l,Fact(l)/Fact(l-k+1)*Power(x[i+1]-x[i],l-k+1));
	  }
	}
	//overwrites the 0.0 which was written before in the for(k) cycle
	band->SetB(pos+number_of_leftcond+1,0,f[i+1]);
      }
      //2 equations for last polynomial in the last submatrix
      pos = (x.size()-2)*(M+1);
      band->SetA(pos+number_of_leftcond,pos,1.0);
      band->SetB(pos+number_of_leftcond,0,f[f.size()-2]);
      if(M > 0)	//constant has no other line
      {
	band->SetB(pos+number_of_leftcond+1,0,f[f.size()-1]);
	band->SetA(pos+number_of_leftcond+1,pos,1.0);
	for(unsigned int l = 1; l <= M; l++)
	{
	  band->SetA(pos+number_of_leftcond+1,pos+l,Power(x[x.size()-1]-x[x.size()-2],l));
	}
      }
}

InterpolantBase* Lagrange::CreateInterpolant ( double* bandres )
{
  //creation of polynoms for an interpolant******************
      vector<Polynomial> polynomials (x.size()-1);
      for(unsigned long i = 0; i < x.size()-1; i++)
      {
	//creation, polynomial degree M
	polynomials[i] = Polynomial(M);	
	//setting of polynomials intervals
	polynomials[i].SetInterval(x[i],x[i+1]);
	//Gets results of dgbtrs and fills the vectors of coeficients of the polynomials
	polynomials[i].SetCoeficients((double*)(bandres + i*(M+1)), M+1);
	
	if (DEB)
	{
	  cout << "p[" << i << "]=";
	  polynomials[i].WriteCoef();
	  cout << endl;
	}
      }
      if(x_defined)
	return new InterpolantAdapt(polynomials);
      else
	return new InterpolantEq(polynomials, step);
}


}	//namespace IInterpolation