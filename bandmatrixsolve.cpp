
#include "bandmatrixsolve.h"
using namespace std;

//*
double* BandMatrixSolve::Solve()
{
  if(!factorization)
  {
    dgbtrf_(&m, &n, &kl, &ku, ab, &ldab, ipiv, &info);
    factorization = 1;
  }
  if(info < 0)
  { cout << "Error: The " << info*(-1) << 
	"-th argument had an illegal value" << endl;
    factorization = 0;
    return 0;
  }	
  if(info > 0)
  { cout << "Error: The factor U is singular" << endl;
    factorization = 0;
    return 0;
  }
  
  dgbtrs_(&trans, &n, &kl, &ku, &nrhs, 
		ab, &ldab, ipiv, b, &ldb, &info);
  return b;
}
//*/
/*
BandMatrixSolve::BandMatrixSolve(long int n,long int ldab,long int ldb,long int ku,long int kl)
  : n(n), m(n), ldab(ldab), ldb(ldb), ku(ku), kl(kl)
{
  trans = 'N';
  k = kl+ku+1;
  ab = new double[ldab*n];
  b = new double[ldb];
  //pivot indices *************************************************
  ipiv= new long int[n];
}
//*/
BandMatrixSolve::BandMatrixSolve(int number_of_nodes, int degree, int number_of_leftcond, 
				 int number_of_rightcond, int number_of_functors)
  : //n((degree+1)*number_of_nodes), 
    //m((degree+1)*number_of_nodes), 	//a is square matrix
    kl(1 + number_of_leftcond), 
    ku(degree - number_of_rightcond),
    nrhs(number_of_functors)
    
{
  n = (degree+1)*number_of_nodes; 
  m = (degree+1)*number_of_nodes; 	//a is square matrix
  trans = 'N';
  k = kl+ku+1;
  ldab = 2*kl+ku+1;
  ldb = n;
  ab = new double[ldab*n];
  b = new double[ldb*nrhs];
  //pivot indices *************************************************
  ipiv= new long int[(degree+1)*n];
}

BandMatrixSolve::~BandMatrixSolve()
{
}


//fortran field [1,...,n]
//c++ field [0,...,n]
void BandMatrixSolve::SetA(unsigned int i,unsigned int j, double value)
{
  #define AB(I,J) ab[(I) + ((J))*ldab]
  if(i < m & j < n)
    AB(k+i-j,j) = value;
  else cout << "Error: out of bounds" << endl;
}

void BandMatrixSolve::SetB(unsigned int i,unsigned int j, double value)
{
  if(i < ldb & j < nrhs)
    b[i + j*ldb] = value;
  else cout << "Error: out of bounds" << endl;
}