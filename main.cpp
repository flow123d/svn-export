
#include <iostream>
#include "stdafx.h"
#include <math.h>

using namespace std;
using namespace Interpolation;

#define NDIM 4 

int main(int argc, char **arg)
{
    
  vector<double*> left;
  left.resize(3);
  cout << left[0] << endl;
  cout << &left[0] << endl;
  /*
    MyFunction func;
    FunctorValueBase *f = new FunctorValue<MyFunction>();
    InterpolantBase *pol = new Interpolant();
    InterpolantBase *pol_eq = new InterpolantEq();
    
    
    vector<double> coefs(4);
    coefs[0] = 2;
    coefs[1] = 0;
    coefs[2] = 0;
    coefs[3] = 1;

    Polynomial *p = new Polynomial(0,2,coefs);
    cout << p->Value(8) << endl;
    cout << p->Integral(0,2) << endl;
    cout << p->Diff(1).dfdx << endl;
    */
    //*
    
    cout << "======================================" << endl;
    
    /*
    int i, j, info2;
int N, NRHS, LDA, LDB;
double A[NDIM*NDIM];
double B[NDIM];
static int IPIV[NDIM], INFO;


N=NDIM; NRHS=1; LDA=NDIM; LDB=NDIM;

A[0] = 1.0;
A[4] = -1.0;
A[8] = 2.0;
A[12] = -1.0;
A[1] = 2.0;
A[5] = -2.0;
A[9] = 3.0;
A[13] = -3.0;
A[2] = 1.0;
A[6] = 1.0;
A[10] = 1.0;
A[14] = 0.0;
A[3] = 1.0;
A[7] = -1.0;
A[11] = 4.0;
A[15] = 3.0;
    
    for (i=0;i<N;i++){
       for (j=0;j<N;j++) {
	 cout << A[i+N*j] << endl;
       }
    }

   B[0] = -8.0; 
   B[1] = -20.0; 
   B[2] = -2.0;
   B[3] = 4.0;

///*
   dgesv_(&N, &NRHS, A, &LDA, &IPIV, B, &LDB, &INFO);

cout << "info " << INFO << endl; 
 
   for (i=0;i<N;i++)
     cout << B[i] << endl;

//*/
    //system("PAUSE");
    return 0;
}
