
#ifndef BANDMATRIXSOLVE_H
#define BANDMATRIXSOLVE_H

#include <iomanip>
#include <iostream>

#ifndef DEB
#define DEB true
#endif

//example  of dgbtrf and dgbtrs
//http://ftp.nag.co.uk/numeric/FLOLCH/chgen22da/most_systems/c_examples/f07befe.c
     
//#define __cplusplus 
extern "C" {
#include "../CLAPACK-3.2.1/INCLUDE/blaswrap.h"
#include "../CLAPACK-3.2.1/INCLUDE/f2c.h"
#include "../CLAPACK-3.2.1/INCLUDE/clapack.h"
}

//types from CLAPACK/INCLUDE/f2c.h that are used here
#ifndef F2C_INCLUDE
#define F2C_INCLUDE
typedef long int integer;
typedef double doublereal;
#endif 

/// A class BandMatrixSolve.
/** Provides interface for solving band matrix by DGBTRS_ and DGBTRF_ in CLAPACK.
  * Closer specification for these methods can be found on 
  * http://www.netlib.org/lapack/double/dgbtrf.f
  * http://www.netlib.org/lapack/double/dgbtrs.f
  */
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
  /// A constructor.
  /** Counts the function value of the interpolant in "x".
    * Is evaluated by a polynom using the Horner schema. 
    * @param n the order of (square) matrix.
    * @param ku is the number of superdiagonals.
    * @param kl is the number of subdiagonals.
    * @param nhrs is the number of right hand vectors.
    */
  BandMatrixSolve( integer n, integer ku, integer kl, integer nrhs );
  
  ///A destructor.
  ~BandMatrixSolve( void );
  
  ///Returns the m dimension of the matrix A.
  inline int GetM()
  { return m; }
  
  ///Returns the n dimension of the matrix A.
  inline int GetN()
  { return n; }
  
  ///Returns the number of superdiagonals of the matrix A.
  inline int GetKU()
  { return ku; }
  
  ///Returns the number of subdiagonals of the matrix A.
  inline int GetKL()
  { return kl; }
  
  ///Returns the number of right hand sizes of the matrix A.
  inline int GetNRHS()
  { return nrhs; }
  
  /// A method for filling the matrix A.
  /** @param i is the row
    * @param j is the column
    */
  void SetA( integer i, integer j, doublereal value );
  
  /// A method for filling the matrix B.
  /** @param i is the row
    * @param j is the column
    */
  void SetB( integer i, integer j, doublereal value );
  
  ///Solves the band matrix.
  /** First uses dgbtrf_ to do LU factorization
    * and if there isno error uses dgbtrs_ 
    * to solve the band matrix.
    */
  double* Solve();

  ///Writes the matrix to output.
  void WrMatrix( double* a, int m, int n );
  
  ///Put zeros in field
  template<class T>
  void EmptyField( T* p, double size );
};

#endif // BANDMATRIXSOLVE_H
