
#include "bandmatrixsolve.h"
using namespace std;

BandMatrixSolve::BandMatrixSolve(long int n,long int ku,long int kl, long int nrhs)
: n(n), ku(ku), kl(kl), nrhs(nrhs)
{
  m = n;
  trans = 'N';
  ldab = 2*kl+ku+1;
  ldb = n*nrhs;
  k = kl+ku+1;
  ab = new double[ldab*n];
  EmptyField(ab,ldab*n);
  b = new double[ldb];
  EmptyField(b,ldb);
  //pivot indices *************************************************
  ipiv= new integer[n];
  EmptyField(ipiv,n);
  factorization = false;
}

BandMatrixSolve::~BandMatrixSolve()
{
  //std::cout << "band destructor ab" << std::endl;
  delete[] ab;
  //std::cout << "band destructor ipiv" << std::endl;
  delete[] ipiv;
  //std::cout << "band destructor b" << std::endl;
  delete[] b;
  //std::cout << "end of band destructor" << std::endl;
}

double* BandMatrixSolve::Solve()
{
  if (DEB)
  {
    WrMatrix(ab,ldab,n);
    cout << endl;
    WrMatrix(b,ldb,nrhs);
  }
  if(!factorization)
  {
    cout << "factorization dgbtrf_..." << endl;
    dgbtrf_(&m, &n, &kl, &ku, ab, &ldab, ipiv, &info);
    factorization = 1;
  }
 
  if(info < 0)
  { cout << "Error: The " << info*(-1) << 
	"-th argument had an illegal value" << endl;
    factorization = false;
    return NULL;
  }	
  
  if(info > 0)
  { cout << "Error: The factor U is singular. (info=" << info << ")" << endl;
    factorization = false;
    return NULL;
  }
  
  cout << "solving dgbtrs_..." << endl;
  dgbtrs_(&trans, &n, &kl, &ku, &nrhs, 
		ab, &ldab, ipiv, b, &ldb, &info);
  
  if (DEB) WrMatrix(b,ldb,nrhs);
  return b;
}

//fortran field [1,...,n]
//c++ field [0,...,n]
void BandMatrixSolve::SetA(integer i, integer j, doublereal value)
{
  #define AB(I,J) ab[(I) + ((J))*ldab]
  if((i < m) & (j < n))
    AB(k-1+i-j,j) = value;
  else cout << "Error: out of bounds /A" << endl;
}

void BandMatrixSolve::SetB(integer i, integer j, doublereal value)
{
  if((i < ldb) & (j < nrhs))
    b[i + j*ldb] = value;
  else cout << "Error: out of bounds /B" << endl;
}

//can write matrix to console output
void BandMatrixSolve::WrMatrix(double* a,int m, int n)
{
  for(int i = 0; i < m; i++)
  {
    for (int j = 0; j < n; j++)
    {
      cout << setw(5) << a[j*m+i] << " | ";
    }
    cout << endl;
  }
}

template<class T>
void BandMatrixSolve::EmptyField(T* p, double size)
{
  for(long i = 0; i < size; i++)
    p[i] = 0.0;
}
