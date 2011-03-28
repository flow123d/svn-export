
#include "stdafx.h"


using namespace std;
using namespace Interpolation;

#define NDIM 4 

int main(int argc, char **arg)
{
  //*
    FunctorValueBase *f = new FunctorValue<MyFunction>();
    Lagrange *lag = new Lagrange();
    lag->SetStep(1.0);
    lag->SetInterval(0.0,3.0);
    lag->AddCond(Lagrange::RightBC, 1, 12.0);
    lag->AddCond(Lagrange::RightBC, 2, 12.0);
    //lag->AddCond(Lagrange::LeftBC,1,2.0);
    //lag->AddCond(Lagrange::LeftBC,2,3.0);
    InterpolantBase *interpolant;
    interpolant = lag->Interpolate<3>(f);

    cout << "result->value(0.1)=" << interpolant->Value(0.1) << endl;
    cout << "result->value(1.0)=" << interpolant->Value(1.0) << endl;
    cout << "result->value(1.3)=" << interpolant->Value(1.3) << endl;
    cout << "result->value(2)=" << interpolant->Value(2) << endl;
    cout << "result->value(2.5)=" << interpolant->Value(2.5) << endl;
    /*
    cout << "result->value(3.0)=" << interpolant->Value(3.0) << endl;
    cout << "result->value(3.3)=" << interpolant->Value(3.3) << endl;
    cout << "result->value(3.9)=" << interpolant->Value(3.9) << endl;
    //cout << "result->value(3)=" << interpolant->Value(2.99) << endl;
    //*/
    vector<double> coefs(4);
    coefs[0] = 2;
    coefs[1] = 0;
    coefs[2] = 0;
    coefs[3] = 1;
    
    /*
    Polynomial *p = new Polynomial(0,2,coefs);
    cout << p->Value(8) << endl;
    cout << p->Integral(0,2) << endl;
    cout << p->Diff(1).dfdx << endl;
    //*/
    
    cout << "======================================" << endl;
    return 0;
}
