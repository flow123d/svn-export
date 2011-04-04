
#include "massert.h"
#include "stdafx.h"

using namespace std;
using namespace Interpolation;

#define NDIM 4 

int main(int argc, char **arg)
{
    FunctorValueBase *f = new FunctorValue<MyFunction>();
    
    
    Adaptive *adaptlag = new Adaptive();
    //*
    adaptlag->SetTolerance(0.001);
    adaptlag->SetStep(1.0);
    adaptlag->SetInterval(0.0,6.0);
    adaptlag->AddCond(IInterpolation::LeftBC,1,0.0);
    adaptlag->AddCond(IInterpolation::LeftBC,2,0.0);
    adaptlag->SetExtrapolation(3,3);
    
    InterpolantBase *adaptint;
    adaptint = adaptlag->Interpolate<3>(f);
    //*/
    /*
    Lagrange *lag = new Lagrange();
    lag->SetStep(1.0);
    lag->SetInterval(0.0,5.0);
    //lag->AddCond(Lagrange::RightBC, 1, 12.0);
    //lag->AddCond(Lagrange::RightBC, 2, 12.0);
    lag->AddCond(IInterpolation::LeftBC,1,0.0);
    lag->AddCond(IInterpolation::LeftBC,2,0.0);
    lag->SetExtrapolation(3,3);
    InterpolantBase *interpolant;
    interpolant = lag->Interpolate<3>(f);
       
    if (interpolant != NULL)
    {
      cout << "result->value(-6.74)=" << interpolant->Value(-6.74) << endl;
      cout << "result->value(-1.3)=" << interpolant->Value(-1.3) << endl;
      cout << "result->value(0.0)=" << interpolant->Value(0.0) << endl;
      cout << "result->value(0.1)=" << interpolant->Value(0.1) << endl;
      cout << "result->value(1.0)=" << interpolant->Value(1.0) << endl;
      cout << "result->value(1.3)=" << interpolant->Value(1.3) << endl;
      cout << "result->value(2)=" << interpolant->Value(2) << endl;
      cout << "result->value(2.5)=" << interpolant->Value(2.5) << endl;
      
      cout << "result->value(3.0)=" << interpolant->Value(3.0) << endl;
      cout << "result->value(3.3)=" << interpolant->Value(3.3) << endl;
      cout << "result->value(3.9)=" << interpolant->Value(3.9) << endl;
    }
    
    cout << "\nnew extrapolation(2,0)" << endl;
    interpolant->SetExtrapolation(2,0);
       
    if (interpolant != NULL)
    {
      cout << "result->value(-6.74)=" << interpolant->Value(-6.74) << endl;
      cout << "result->value(-1.3)=" << interpolant->Value(-1.3) << endl;
      cout << "result->value(0.0)=" << interpolant->Value(0.0) << endl;
      cout << "result->value(0.1)=" << interpolant->Value(0.1) << endl;
      cout << "result->value(1.0)=" << interpolant->Value(1.0) << endl;
      cout << "result->value(1.3)=" << interpolant->Value(1.3) << endl;
      cout << "result->value(2)=" << interpolant->Value(2) << endl;
      cout << "result->value(2.5)=" << interpolant->Value(2.5) << endl;
      
      cout << "result->value(3.0)=" << interpolant->Value(3.0) << endl;
      cout << "result->value(3.3)=" << interpolant->Value(3.3) << endl;
      cout << "result->value(3.9)=" << interpolant->Value(3.9) << endl;
    }
    //*/
    
    //*
    vector<double> coefs(4);
    coefs[0] = 2;
    coefs[1] = 0;
    coefs[2] = 0;
    coefs[3] = 1;
      
    cout << "polynomial p1:" << endl;
    Polynomial *p1 = new Polynomial(0,2,coefs);
    cout << p1->Value(8) << endl;
    cout << p1->Integral(0,2) << endl;
    cout << p1->Diff(1).dfdx << endl;
    
    cout << "polynomial p2:" << endl;
    Polynomial *p2 = new Polynomial(*p1);
    cout << p2->Value(8) << endl;
    cout << p2->Integral(0,2) << endl;
    cout << p2->Diff(1).dfdx << endl;
    
    //*/
    cout << "==============main() END=================" << endl;
    return 0;
}
