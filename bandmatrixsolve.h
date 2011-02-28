
#ifndef BANDMATRIXSOLVE_H
#define BANDMATRIXSOLVE_H


//example  of dgbtrf and dgbtrs
//http://ftp.nag.co.uk/numeric/FLOLCH/chgen22da/most_systems/c_examples/f07befe.c
      
extern "C" {
#include "CLAPACK/blaswrap.h"
#include "CLAPACK/f2c.h"
#include "CLAPACK/clapack.h"
}

#include <iostream>

class BandMatrixSolve
{
private:
  char trans;		//A*x = B type of matrix
  integer m;		//The number of rows of the matrix A.  M >= 0.
  integer n;		//the order of matrix; The number of columns of the matrix A.  N >= 0.
  integer kl;		//the number of subdiagonals;
  integer ku;		//the number of superdiagonals;
  integer nrhs;		//the number of right hand sizes (columns of B)
  
  int k;

  integer ldab;		//the leading dimension of the array AB; = KU+1 (for KL=0)
  integer *ipiv;	//the pivot indices; [1,2,3,4,...,N-1,N] for "no pivoting"	
  doublereal *ab;	//AB matrix	

  integer ldb;		//the leading dimension of the array B; output for X
  integer info;		//output; = 0 successful exit
  doublereal *b;

  bool factorization;
 
public:
	//BandMatrixSolve(long int n, long int ldab,long int ldb,long int ku,long int kl);
	BandMatrixSolve(int number_of_nodes, int degree, int number_of_leftcond, 
				 int number_of_rightcond, int number_of_functors);
	~BandMatrixSolve();
	//void SetUpperBandDimension(int ku);
	//void SetLowerBandDimension(int kl);
	//void Set_Adimension(int m,int n);
	//void Set_Bdimension(int m,int n);
	void SetA(unsigned int i,unsigned int j, double value);
	void SetB(unsigned int i, unsigned int j, double value);
	double* Solve();
};

#endif // BANDMATRIXSOLVE_H
