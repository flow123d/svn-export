
#ifndef BANDMATRIXSOLVE_H
#define BANDMATRIXSOLVE_H


//example  of dgbtrf and dgbtrs
//http://ftp.nag.co.uk/numeric/FLOLCH/chgen22da/most_systems/c_examples/f07befe.c
     
//#define __cplusplus 
extern "C" {
#include "../CLAPACK-3.2.1/INCLUDE/blaswrap.h"
#include "../CLAPACK-3.2.1/INCLUDE/f2c.h"
#include "../CLAPACK-3.2.1/INCLUDE/clapack.h"
}

//types from CLAPACK/INCLUDE/f2c.h
#ifndef F2C_INCLUDE
#define F2C_INCLUDE
typedef long int integer;
typedef unsigned long int uinteger;
typedef char *address;
typedef short int shortint;
typedef float real;
typedef double doublereal;
#endif 

#include <iostream>

class BandMatrixSolve
{
private:
  char trans;		//A*x = B type of matrix
  integer m;		//The number of rows of the matrix A.  M >= 0.
  integer n;		//the order of matrix; The number of columns of the matrix A.  N >= 0.
  integer ku;		//the number of superdiagonals;
  integer kl;		//the number of subdiagonals;
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
	BandMatrixSolve(integer n, integer ku, integer kl, integer nrhs);
	//BandMatrixSolve(int number_of_nodes, int degree, int number_of_leftcond, 
	//			 int number_of_rightcond, int number_of_functors);
	~BandMatrixSolve();
	//void SetUpperBandDimension(int ku);
	//void SetLowerBandDimension(int kl);
	//void Set_Adimension(int m,int n);
	//void Set_Bdimension(int m,int n);
	void SetA(integer i, integer j, doublereal value);
	void SetB(integer i, integer j, doublereal value);
	double* Solve();
};

#endif // BANDMATRIXSOLVE_H
