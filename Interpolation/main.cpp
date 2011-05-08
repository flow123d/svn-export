
#include "massert.h"
#include "stdafx.h"

#include <time.h>

using namespace std;
using namespace Interpolation;

#define N_E 10000000	//number of evaluations for testing
#define CLK_TCK		CLOCKS_PER_SEC

InterpolantBase* FindStep( FunctorValueBase& f, IInterpolation* interpolation, double tolerance )
{
  double tot_err;	// absolute error on <a,b>
  double tot_err_prev = 9.99999999999999e+15;	// absolute error on <a,b> in previous iteration
      
  InterpolantBase* result;
  
  //*
  int n = 1;
  while (1)
      {
	result = interpolation->Interpolate(f);	//interpolation with actual nodes
	
	tot_err = interpolation->ComputeError(&f,result);
	
	//writes relative and absolute total error
	cout << "Interation = " << n << "\trel. err = " << 
	  tot_err/(interpolation->GetUpper()-interpolation->GetLower()) 
	  << "\tabs. err = " << tot_err << endl;
	cout << "with step = " << interpolation->GetStep() << endl;
	  
	if(tot_err > tot_err_prev) 
	{	
	  cout << "error has increased -> end of iteration"<< endl;
	  break;
	}
	tot_err_prev = tot_err;
	
	if(tot_err > tolerance)
	{
	  interpolation->SetStep(0.5*interpolation->GetStep());
	}
	else
	  break;
	
	n++;
      }
      return result;
}

void test_interpolant()
{
  cout.precision(15);
  FunctorValue<MyFunction> f;
  Lagrange *lag = new Lagrange();
  double a = 0.0;
  double b = 30.0;
  lag->SetDegree(3);
  lag->SetStep(1.0);
  lag->SetInterval(a,b);
  //lag->AddCond(IInterpolation::LeftBC,1,0.0);
  //lag->AddCond(IInterpolation::LeftBC,2,0.0);
  lag->AddCond(IInterpolation::RightBC,1,2700.0);
  lag->AddCond(IInterpolation::RightBC,2,1800.0);
  lag->SetExtrapolation(3,3);
 
  InterpolantBase *interpolant;
  /*
  interpolant = lag->Interpolate(f);
  
  cout << "interpolant x^3: " << endl;
  cout << "d(1) = " << interpolant->Diff(1).dfdx << endl;
  cout << "d(1.2) = " << interpolant->Diff(1.2).dfdx << endl;
  cout << "d(3) = " << interpolant->Diff(3).dfdx << endl;
  cout << "d(6) = " << interpolant->Diff(6).dfdx << endl;
  cout << "I(4;9) = " << interpolant->Integral(4,9) << endl;
  //*/
  /*
  FunctorValue<x5> g;
  lag->SetDegree(5);
  lag->AddCond(IInterpolation::LeftBC,3,0.0);
  lag->AddCond(IInterpolation::LeftBC,4,0.0);
  
  /* for b=10
  lag->AddCond(IInterpolation::RightBC,1,5.0*10000);
  lag->AddCond(IInterpolation::RightBC,2,20.0*1000);
  lag->AddCond(IInterpolation::RightBC,3,60.0*100);
  lag->AddCond(IInterpolation::RightBC,4,120.0*10);
  //*/
  
  interpolant = lag->Interpolate(f);
  
  cout << "interpolant x^5: " << endl;
  cout << "d(1) = " << interpolant->Diff(1.0).dfdx << endl;
  cout << "d(1.2) = " << interpolant->Diff(1.2).dfdx << endl;
  cout << "d(3) = " << interpolant->Diff(3.0).dfdx << endl;
  cout << "d(6) = " << interpolant->Diff(6.0).dfdx << endl;
  cout << "I(4;9) = " << interpolant->Integral(4.0,9.0) << endl;
  //*/
  
  cout << "error = " << lag->ComputeError(&f,interpolant) << endl;
  
  delete lag;
  delete interpolant;
  
}

void test_polynomial()
{
  vector<double> coefs1(4);
  coefs1[0] = 2;
  coefs1[1] = 0;
  coefs1[2] = 0;
  coefs1[3] = 1;
  
  cout << "polynomial p1=x^3 + 2" << endl;
  Polynomial p1(0,2,coefs1);
  cout << p1(8) << endl;
  cout << p1.Integral(0,5) << endl;
  cout << p1.Diff(1).dfdx << endl;
  
   
  cout << "polynomial p2:" << endl;
  Polynomial p2(p1);
  cout << p2(8) << endl;
  cout << p2.Integral(0,2) << endl;
  cout << p2.Diff(1).dfdx << endl;
  
  cout << "polynomial p3=3x+2 test of linear polynomial" << endl;
  vector<double> coefs2(2);
  coefs2[0] = 2;
  coefs2[1] = 3;
  Polynomial p3(0,2,coefs2);
  cout << p3(8) << endl;
  cout << p3.Integral(0,2) << endl;
  cout << p3.Diff(1).dfdx << endl;
  
  cout << "polynomial p4=2 test of constant" << endl;
  vector<double> coefs3(2);
  coefs3[0] = 2;
  Polynomial p4(0,2,coefs3);
  cout << p4(8) << endl;
  cout << p4.Integral(0,2) << endl;
  cout << p4.Diff(1).dfdx << endl;
  
}

void test_adaptivesimpson()
{
  double simpsontol = 1e-6;	//start with this tolerance
  
  FunctorValue<AbsFunc> f;	//functor - in myfunction.h
  double a = 0;
  double b = 10.5;
  double analytic = 7*3.5+3*3.5*3.5/2;	// -fabs(x-3.5)+7;
  
  double integral =0;
  for(int i = 0; i < 26; i++)
  {
    simpsontol /= 10;			//changes the tolerance
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
  
  FunctorValue<SqrtFunc> h;
  a = 0;
  b = 1e-10;
  double analytic3 = sqrt(b*b*b)*2/3;
  simpsontol = 1e-5;
  for(int i = 0; i < 20; i++)
  {
    simpsontol /= 10;
    integral = AdaptiveSimpson::AdaptSimpson(h,a,b,simpsontol);
    cout << "tol = " << simpsontol << "\tintegral(SqrtFunc) = " << integral << "\terror to analytic = " << analytic3-integral;
    if(analytic3-integral <= simpsontol*10) cout << "\t\tPASSED" << endl;
    else cout << "\t\tFAILED" << endl;
  }
  
}

void test_lagrange_sqrt()
{
  FunctorValue<SqrtFunc> sqrtf;
  Lagrange *lag_sqrtf = new Lagrange();
  double a = 0.0;
  double b = 20.0;
  lag_sqrtf->SetDegree(3);
  lag_sqrtf->SetStep(0.5);
  lag_sqrtf->SetInterval(a,b);
  //lag_sqrtf->AddCond(IInterpolation::LeftBC,1,0.25);	//f'(4)=1/4=0.25
  //lag_sqrtf->AddCond(IInterpolation::LeftBC,2,0.03125);	//f''(4)=-1/32=-0.03125
  //lag_sqrtf->SetExtrapolation(3,3);
  InterpolantBase *interpolant_sqrtf;
  interpolant_sqrtf = lag_sqrtf->Interpolate(sqrtf);
    
  double x = a;
  while(1)
  {
    if(x > b) break;
    cout << "x=" << x << "\t\tsqrt=" << sqrt(x)<< "\t\t interpolant=" << interpolant_sqrtf->operator()(x) 
      << "\t\t err=" << sqrt(x)- interpolant_sqrtf->operator()(x) << endl;
    x+=0.1;
  }
    
  delete lag_sqrtf;
  delete interpolant_sqrtf;
}

void test_constant()
{
  FunctorValue<SqrtFunc> sqrtf;
  Lagrange *lag_sqrtf = new Lagrange();
  double a = 0.0;
  double b = 20.0;
  lag_sqrtf->SetDegree(0);
  lag_sqrtf->SetStep(0.5);
  lag_sqrtf->SetInterval(a,b);
  //lag_sqrtf->AddCond(IInterpolation::LeftBC,1,0.25);	//f'(4)=1/4=0.25
  //lag_sqrtf->AddCond(IInterpolation::LeftBC,2,0.03125);	//f''(4)=-1/32=-0.03125
  //lag_sqrtf->SetExtrapolation(3,3);
  InterpolantBase *interpolant_sqrtf;
  interpolant_sqrtf = lag_sqrtf->Interpolate(sqrtf);
    
  double x = a;
  
  while(1)
  {
    if(x >= b) break;
    cout << "x=" << x << "\t\tsqrt=" << sqrt(x)<< "\t\t interpolant=" << interpolant_sqrtf->operator()(x) 
      << "\t\t err=" << sqrt(x)- interpolant_sqrtf->operator()(x) << endl;
    x+=0.1;
  }
   
  delete lag_sqrtf;
  delete interpolant_sqrtf;
}

void test_lagrange_absfunc()
{
  FunctorValue<AbsFunc> f;
    Lagrange *lag_f = new Lagrange();
    double a = 0.0;
    double b = 10.5;
    lag_f->SetDegree(3);
    lag_f->SetStep(0.1);
    lag_f->SetInterval(a,b);
    lag_f->AddCond(IInterpolation::LeftBC,1,1.0);	
    lag_f->AddCond(IInterpolation::RightBC,2,-1.0);	
    lag_f->SetExtrapolation(3,3);
    InterpolantBase *interpolant;
    interpolant = lag_f->Interpolate(f);
    
    double x = a;
    while(1)
    {
      if(x > b) break;
      cout << "x=" << x << "\t\tf=" << f(x)<< "\t\t interpolant=" << interpolant->operator()(x) 
	 << "\t\t err=" << f(x)- interpolant->operator()(x) << endl;
      x+=0.013;
    }
    
  delete lag_f;
  delete interpolant;
}

void test_FK_FQ() 
{
  FunctorValue<FK> fk;
  FunctorValue<FQ> fq;

  double a = -10.0;
  double b = 0.012;
  double step = 0.05;
  //double tolerance = 0.0005;
  //double toleranceadapt = 0.0005;

  Lagrange *lag_fk = new Lagrange();
  lag_fk->SetDegree(3);
  lag_fk->SetStep(step);
  lag_fk->SetInterval(a,b);
  //lag_fk->AddCond(IInterpolation::LeftBC,1,1.0);	
  //lag_fk->AddCond(IInterpolation::RightBC,2,-1.0);	
  lag_fk->SetExtrapolation(0,0);
  InterpolantBase *ifk;
  ifk = lag_fk->Interpolate(fk);

  /*
  Lagrange *lag_fq = new Lagrange();
  lag_fk.SetDegree(3);
  lag_fq->SetStep(step);
  lag_fq->SetInterval(a,b);
  //lag_fq->AddCond(IInterpolation::LeftBC,1,1.0);	
  //lag_fq->AddCond(IInterpolation::RightBC,2,-1.0);	
  lag_fq->SetExtrapolation(0,0);
  InterpolantBase *ifq;
  ifq = lag_fq->Interpolate(fq);
  */
  
  double x = a;
    while(1)
    {
      if(x > b) break;
      cout << "x=" << x << "\t\tf=" << fk(x)<< "\t\t interpolant=" << ifk->operator()(x) 
	 << "\t\t err=" << fk(x)- ifk->operator()(x) << endl;
      x+=0.02;
    }
    
    double tot_err;	// absolute error on <a,b
    ErrorNum p_err;	// relative polynomial error on its interval
    for(unsigned long i = 0; i < ifk->GetCount(); i++ )
	{
	  LP_Norm norm(&fk,ifk->GetPol(i),2);
	  p_err.i = i;
	  //absolute polynomial error
	  p_err.err = sqrt(AdaptiveSimpson::AdaptSimpson( norm,
						     ifk->GetPol(i)->GetLower(), 
						     ifk->GetPol(i)->GetUpper(),
						     1e-10) );
	  //increase the absolute total error
	  tot_err += p_err.err;
	  //p_err convertion absolute -> relative (p_err/(xi+1 - xi))
	  p_err.err /= (ifk->GetPol(i)->GetUpper()-ifk->GetPol(i)->GetLower());
	}
    cout << "\trel. err = " << tot_err/(b-a) << "\tabs. err = " << tot_err << endl;
    
  delete lag_fk;
  delete ifk;
}


int main(int argc, char **arg)
{
    cout.precision(15);
    cout << "Output precision has been set to 15 digits." << endl;
    
    //test_interpolant();
    //test_polynomial();
    //test_adaptivesimpson();
    //test_lagrange_sqrt();
    //test_lagrange_absfunc();
    //test_FK_FQ();
    //test_constant();
    
    //*
    FunctorValueBase* f = new FunctorValue<SqrtFunc>();
    Lagrange *lag = new Lagrange();
    
    lag->SetDegree(3);
    lag->SetStep(1.0);
    lag->SetInterval(0.0,2.0);
    
    //BC 2nd derivates
    lag->AddCond(IInterpolation::LeftBC,2,0.0);
    lag->AddCond(IInterpolation::RightBC,2,-sqrt(2.0)/16.0);
    
    InterpolantBase* i;
    i = FindStep(*f,lag,0.01);	//finishes in 6th iteration succesfully
    //for tolerance = 0.01
    //Interation = 6  rel. err = 0.00277933154486984  abs. err = 0.00555866308973968
    //with step = 0.03125
    
    //for tolerance = 0.001
    //Interation = 9  rel. err = 0.000347412520764468 abs. err = 0.000694825041528936
    //with step = 0.00390625
    
    //for tolerance = 0.0001
    //Interation = 12 rel. err = 4.34265624209219e-05 abs. err = 8.68531248418439e-05
    //with step = 0.00048828125


    delete lag;
    
    cout << "------------------------------------------------------------------------------------" << endl;
    lag = new Lagrange();
    lag->SetDegree(3);
    lag->SetStep(1.0);
    lag->SetInterval(0.0,2.0);
    
    //changing BC - on the right side only
    lag->AddCond(IInterpolation::RightBC,1,sqrt(2.0)/4.0);
    lag->AddCond(IInterpolation::RightBC,2,-sqrt(2.0)/16.0);

    i = FindStep(*f,lag,0.01); //finishes in 3th iteration unsuccefully
    delete lag;
    
    cout << "------------------------------------------------------------------------------------" << endl;
    lag = new Lagrange();
    lag->SetDegree(3);
    lag->SetStep(1.0);
    lag->SetInterval(0.0,2.0);
    
    //changing BC - on the left side only
    lag->AddCond(IInterpolation::LeftBC,1,0.0);
    lag->AddCond(IInterpolation::LeftBC,2,0.0);
    
    i = FindStep(*f,lag,0.01); //finishes in 2th iteration unsuccefully
    delete i;
    
    //*/
    
    /*
    FunctorValue<SqrtFunc> sqrtf;
    
    Lagrange *lag_sqrtf = new Lagrange();
    
    lag_sqrtf->SetDegree(3);
    lag_sqrtf->SetStep(0.5);
    lag_sqrtf->SetInterval(0.0,2.0);
    //lag_sqrtf->AddCond(IInterpolation::RightBC,1,sqrt(2.0)/4.0);
    lag_sqrtf->AddCond(IInterpolation::LeftBC,2,0.0);
    lag_sqrtf->AddCond(IInterpolation::RightBC,2,-sqrt(2.0)/16.0);
    
    InterpolantBase *int_sqrtf;
    int_sqrtf = lag_sqrtf->Interpolate(sqrtf);
    
    cout << "error = " << lag_sqrtf->ComputeError(&sqrtf, int_sqrtf) << endl;
    
    delete lag_sqrtf;
    delete int_sqrtf;
    //*/
    
    
    //==========================================================================
    /*	test precision of cout
    double x = 1.0;
    cout << x << endl;
    cout << fixed << x << endl;
    cout.precision(10);
    cout << "10:\t" << fixed << x << endl;
    cout.precision(15);
    cout << "16:\t" << fixed << x << endl;
    cout << x << endl;
    //*/
    
    /*
    //FunctorValueBase f = new FunctorValue<MyFunction>();
    FunctorValue<MyFunction> f;   
    
    Adaptive *adaptlag_f = new Adaptive();
    
    adaptlag_f->SetDegree(3);
    adaptlag_f->SetTolerance(0.01);
    adaptlag_f->SetStep(1.0);
    adaptlag_f->SetInterval(0.0,5.0);
    adaptlag_f->AddCond(IInterpolation::LeftBC,1,0.0);
    adaptlag_f->AddCond(IInterpolation::LeftBC,2,0.0);
    adaptlag_f->SetExtrapolation(3,3);
    
    InterpolantBase *adaptint_f;
    adaptint_f = adaptlag_f->Interpolate(f);
    //*/
    
    /*
    Adaptive *adaptlag_sqrtf = new Adaptive();
    
    adaptlag_sqrtf->SetDegree(3);
    adaptlag_sqrtf->SetTolerance(0.1);
    adaptlag_sqrtf->SetStep(0.5);
    adaptlag_sqrtf->SetInterval(0.0,2.0);
    adaptlag_sqrtf->AddCond(IInterpolation::RightBC,1,sqrt(2.0)/4.0);
    adaptlag_sqrtf->AddCond(IInterpolation::RightBC,2,-sqrt(2.0)/16.0);
    
    InterpolantBase *adaptint_sqrtf;
    adaptint_sqrtf = adaptlag_sqrtf->Interpolate(sqrtf);
    
    delete adaptlag_sqrtf;
    delete adaptint_sqrtf;
    //*/
    
    /*
    FunctorValue<SqrtFunc> sqrtf;
    
    Adaptive *adaptlag_sqrtf = new Adaptive();
    
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
