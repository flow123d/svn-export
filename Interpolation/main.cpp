
#include "massert.h"
#include "stdafx.h"

using namespace std;
using namespace Interpolation;

#define NDIM 4 

void test_polynomial()
{
  vector<double> coefs(4);
  coefs[0] = 2;
  coefs[1] = 0;
  coefs[2] = 0;
  coefs[3] = 1;
      
  cout << "polynomial p1:" << endl;
  Polynomial p1(0,2,coefs);
  cout << p1(8) << endl;
  cout << p1.Integral(0,2) << endl;
  cout << p1.Diff(1).dfdx << endl;
   
  cout << "polynomial p2:" << endl;
  Polynomial p2(p1);
  cout << p2(8) << endl;
  cout << p2.Integral(0,2) << endl;
  cout << p2.Diff(1).dfdx << endl;
}

void test_adaptivesimpson()
{
  double simpsontol = 1e-6;	//start with this tolerance
  
  FunctorValue<AbsFunc> f;
  double a = 0;
  double b = 10.5;
  double analytic = 7*3.5+3*3.5*3.5/2;	// -fabs(x-3.5)+7;
  
  double integral =0;
  for(int i = 0; i < 26; i++)
  {
    simpsontol /= 10;
    integral = AdaptiveSimpson::AdaptSimpson(f,a,b,simpsontol);
    cout << "tol = " << simpsontol << "\tintegral(AbsFunc) = " << integral << "\terror to analytic = " << analytic-integral;
    if(analytic-integral <= simpsontol*10) cout << "\t\tPASSED" << endl;
    else cout << "\t\tFAILED" << endl;
  }
  
  FunctorValue<SqrtFunc> g;
  a = 0;
  b = 20;
  double analytic2 = sqrt(20*20*20)*2/3;
  simpsontol = 1e-6;
  for(int i = 0; i < 20; i++)
  {
    simpsontol /= 10;
    integral = AdaptiveSimpson::AdaptSimpson(g,a,b,simpsontol);
    cout << "tol = " << simpsontol << "\tintegral(SqrtFunc) = " << integral << "\terror to analytic = " << analytic2-integral;
    if(analytic2-integral <= simpsontol*10) cout << "\t\tPASSED" << endl;
    else cout << "\t\tFAILED" << endl;
  }
  
}

void test_lagrange_sqrt()
{
  FunctorValue<SqrtFunc> sqrtf;
    Lagrange *lag_sqrtf = new Lagrange();
    double a = 0.0;
    double b = 20.0;
    lag_sqrtf->SetStep(0.5);
    lag_sqrtf->SetInterval(a,b);
    //lag_sqrtf->AddCond(IInterpolation::LeftBC,1,0.25);	//f'(4)=1/4=0.25
    //lag_sqrtf->AddCond(IInterpolation::LeftBC,2,0.03125);	//f''(4)=-1/32=-0.03125
    //lag_sqrtf->SetExtrapolation(3,3);
    InterpolantBase *interpolant_sqrtf;
    interpolant_sqrtf = lag_sqrtf->Interpolate<3>(sqrtf);
    
    double x = a;
    while(1)
    {
      if(x > b) break;
      cout << "x=" << x << "\t\tsqrt=" << sqrt(x)<< "\t\t interpolant=" << interpolant_sqrtf->operator()(x) 
	 << "\t\t err=" << sqrt(x)- interpolant_sqrtf->operator()(x) << endl;
      x+=0.1;
    }
}

void test_lagrange_absfunc()
{
  FunctorValue<AbsFunc> f;
    Lagrange *lag_f = new Lagrange();
    double a = 0.0;
    double b = 10.5;
    lag_f->SetStep(0.1);
    lag_f->SetInterval(a,b);
    lag_f->AddCond(IInterpolation::LeftBC,1,1.0);	
    lag_f->AddCond(IInterpolation::RightBC,2,-1.0);	
    lag_f->SetExtrapolation(3,3);
    InterpolantBase *interpolant;
    interpolant = lag_f->Interpolate<3>(f);
    
    double x = a;
    while(1)
    {
      if(x > b) break;
      cout << "x=" << x << "\t\tf=" << f(x)<< "\t\t interpolant=" << interpolant->operator()(x) 
	 << "\t\t err=" << f(x)- interpolant->operator()(x) << endl;
      x+=0.013;
    }
}

int main(int argc, char **arg)
{
    //test_adaptivesimpson();
    //test_lagrange_sqrt();
    //test_lagrange_absfunc();
    
    /*
    //FunctorValueBase f = new FunctorValue<MyFunction>();
    FunctorValue<MyFunction> f;   
    
    Adaptive *adaptlag_f = new Adaptive();
    
    adaptlag_f->SetTolerance(0.0001);
    adaptlag_f->SetStep(1.0);
    adaptlag_f->SetInterval(0.0,5.0);
    adaptlag_f->AddCond(IInterpolation::LeftBC,1,0.0);
    adaptlag_f->AddCond(IInterpolation::LeftBC,2,0.0);
    adaptlag_f->SetExtrapolation(3,3);
    
    InterpolantBase *adaptint_f;
    adaptint_f = adaptlag_f->Interpolate<3>(f);
    //*/
    //*
    FunctorValue<SqrtFunc> sqrtf;
    
    Adaptive *adaptlag_sqrtf = new Adaptive();
    //*
    adaptlag_sqrtf->SetTolerance(0.0001);
    adaptlag_sqrtf->SetStep(7.0);
    adaptlag_sqrtf->SetInterval(4.0,20.0);
    //adaptlag_sqrtf->AddCond(IInterpolation::LeftBC,1,0.25);	//f'=1/4=0.25
    //adaptlag_sqrtf->AddCond(IInterpolation::LeftBC,2,0.03125);	//f''=-1/32=-0.03125
    //adaptlag_sqrtf->AddCond(IInterpolation::RightBC,1,0.111803398);	//f'=1/4=0.25
    //adaptlag_sqrtf->AddCond(IInterpolation::RightBC,2,-0.002795084972);	//f''=-1/32=-0.03125
    //adaptlag_sqrtf->SetExtrapolation(3,3);
    
    InterpolantBase *adaptint_sqrtf;
    adaptint_sqrtf = adaptlag_sqrtf->Interpolate<3>(sqrtf);
    //*/
    /*
    FunctorValue<SqrtFunc> sqrtf;
    Lagrange *lag_sqrtf = new Lagrange();
    lag_sqrtf->SetStep(0.1);
    lag_sqrtf->SetInterval(0.0,4.0);
    lag_sqrtf->AddCond(IInterpolation::RightBC,1,0.25);	//f'=1/4=0.25
    lag_sqrtf->AddCond(IInterpolation::RightBC,2,0.03125);	//f''=-1/32=-0.03125
    lag_sqrtf->SetExtrapolation(3,3);
    InterpolantBase *interpolant_sqrtf;
    interpolant_sqrtf = lag_sqrtf->Interpolate<3>(sqrtf);
    
    double tot_err=0;
    for(unsigned long i = 0; i < interpolant_sqrtf->GetCount(); i++ )
	{
	  LP_Norm norm(&sqrtf,interpolant_sqrtf->GetPol(i),2);
	  double err = sqrt(AdaptiveSimpson::AdaptSimpson( norm,
						     interpolant_sqrtf->GetPol(i)->GetLower(), 
						     interpolant_sqrtf->GetPol(i)->GetUpper(),
						     1e-6) );
	  tot_err += err;
	}
    
    cout << "error = " << tot_err << endl;
    */
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
      cout << "result(-6.74)=" << interpolant(-6.74) << endl;
      cout << "result(-1.3)=" << interpolant(-1.3) << endl;
      cout << "result(0.0)=" << interpolant(0.0) << endl;
      cout << "result(0.1)=" << interpolant(0.1) << endl;
      cout << "result(1.0)=" << interpolant(1.0) << endl;
      cout << "result(1.3)=" << interpolant(1.3) << endl;
      cout << "result(2)=" << interpolant(2) << endl;
      cout << "result(2.5)=" << interpolant(2.5) << endl;
      
      cout << "result(3.0)=" << interpolant(3.0) << endl;
      cout << "result(3.3)=" << interpolant(3.3) << endl;
      cout << "result(3.9)=" << interpolant(3.9) << endl;
    }
    
    cout << "\nnew extrapolation(2,0)" << endl;
    interpolant->SetExtrapolation(2,0);
       
    if (interpolant != NULL)
    {
      cout << "result(-6.74)=" << interpolant(-6.74) << endl;
      cout << "result(-1.3)=" << interpolant(-1.3) << endl;
      cout << "result(0.0)=" << interpolant(0.0) << endl;
      cout << "result(0.1)=" << interpolant(0.1) << endl;
      cout << "result(1.0)=" << interpolant(1.0) << endl;
      cout << "result(1.3)=" << interpolant(1.3) << endl;
      cout << "result(2)=" << interpolant(2) << endl;
      cout << "result(2.5)=" << interpolant(2.5) << endl;
      
      cout << "result(3.0)=" << interpolant(3.0) << endl;
      cout << "result(3.3)=" << interpolant(3.3) << endl;
      cout << "result(3.9)=" << interpolant(3.9) << endl;
    }
    //*/
    
    cout << "==============main() END=================" << endl;
    return 0;
}
