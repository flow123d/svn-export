
#include "massert.h"
#include "stdafx.h"

#include <time.h>

using namespace std;
using namespace Interpolation;

#define CLK_TCK		CLOCKS_PER_SEC

InterpolantBase* FindStep( FunctorValueBase& f, IInterpolation* interpolation, double tolerance )
{
  double tot_err;	// absolute error on <a,b>
  double tot_err_prev = 9.99999999999999e+99;	// absolute error on <a,b> in previous iteration
      
  InterpolantBase* result;
  
  //*
  int n = 1;
  while (1)
      {
	result = interpolation->Interpolate(f);	//interpolation with actual nodes
	
	tot_err = interpolation->ComputeError(&f,result);
	
	//writes relative and absolute total error
	cout << "Interation = " << n << "\trel.err = " << 
	  tot_err/(interpolation->GetB()-interpolation->GetA()) 
	  << "\t\tabs.err = " << tot_err;
	cout << "\t\twith step = " << interpolation->GetStep() << endl;
	  
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

double MeasureTime(FunctorValueBase* func, const double& a, const double& b, const long& n)
{
	double step = abs(b - a)/n;
	
	clock_t starttime, endtime;
	starttime = clock();
	for(int i = 0; i < n; i++)
	{
		func->operator()(a + i*step);
	}
	endtime = clock();

	return ((double)endtime-starttime)/CLK_TCK;
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
  //lag->AddCond(IInterpolation::RightBC,1,2700.0);
  //lag->AddCond(IInterpolation::RightBC,2,180.0);
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
  //*
  FunctorValue<x5> g;
  lag->SetDegree(5);
  
  lag->AddCond(IInterpolation::LeftBC,3,0.0);
  lag->AddCond(IInterpolation::LeftBC,4,0.0);
  
  // for b=10
  //lag->AddCond(IInterpolation::RightBC,1,5.0*10000);
  //lag->AddCond(IInterpolation::RightBC,2,20.0*1000);
  lag->AddCond(IInterpolation::RightBC,3,60.0*b*b);
  lag->AddCond(IInterpolation::RightBC,4,120.0*b);
  //*/
  
  interpolant = lag->Interpolate(g);
  /*
  cout << "interpolant x^5: " << endl;
  cout << "d(1) = " << interpolant->Diff(1.0).dfdx << endl;
  cout << "d(1.2) = " << interpolant->Diff(1.2).dfdx << endl;
  cout << "d(3) = " << interpolant->Diff(3.0).dfdx << endl;
  cout << "d(6) = " << interpolant->Diff(6.0).dfdx << endl;
  cout << "I(4;9) = " << interpolant->Integral(4.0,9.0) << endl;
  //*/
  
  cout << "error = " << lag->ComputeError(&g,interpolant) << endl;
  
  delete lag;
  delete interpolant;
  
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

void test_FK_FQ()	//const double& degree)
/*const double& a, const double& b, 
		 const double& step, const double& tolerance, 
		 const int& degree ) 
		 */
{
  FK ffk;
  FQ ffq;
  FunctorDiff<FK> fk;
  FunctorDiff<FQ> fq;
  
  double a = -10.0;
  double b = ffk.Get_Hs()-1e-10;	//-0.127;
  double step = 1.0;//0.05;
  double tolerance = 1.0e-7;
  double tolerance_0 = 5.0e-4; //for constant

  cout << "fk'(a) = "<< fk.Diff(a).dfdx << endl;
  cout << "fk'(b) = "<< fk.Diff(b).dfdx << endl;
  cout << "fk''(a) = "<< fk.Diffn(a,2) << endl;
  cout << "fk''(b) = "<< fk.Diffn(b,2) << endl;
  
  cout << "fq'(a) = "<< fq.Diff(a).dfdx << endl;
  cout << "fq'(b) = "<< fq.Diff(b).dfdx << endl;
  cout << "fq''(a) = "<< fq.Diffn(a,2) << endl;
  cout << "fq''(b) = "<< fq.Diffn(b,2) << endl;
  
  cout << "fk_Hr = "<< ffk.Get_Hr() << endl;
  cout << "fk_Hs = "<< ffk.Get_Hs() << endl;
  cout << "fq_Hr = "<< ffq.Get_Hr() << endl;
  cout << "fq_Hs = "<< ffq.Get_Hs() << endl;
  
  
  const int n_count = 8;
  long n[] = {1e+7,3e+7,5e+7,7e+7,9e+7,1e+8,3e+8,5e+8};
  
  //*
  Lagrange *lag = new Lagrange();
  lag->SetStep(step);
  lag->SetInterval(a,b);
  lag->SetExtrapolation(0,0);
  
  lag->SetDegree(0);
  cout << "tolerance = " << tolerance_0 << endl;
  cout << "==================  TEST - FK <0> ==================" << endl;
  InterpolantBase *ifk0;
  ifk0 = FindStep(fk,lag,tolerance_0);
  
  lag->SetStep(step);
  cout << "==================  TEST - FQ <0> ==================" << endl;
  InterpolantBase *ifq0;
  ifq0 = FindStep(fq,lag,tolerance_0);
  
  lag->SetDegree(1);
  lag->SetStep(step);
  cout << "tolerance = " << tolerance << endl;
  cout << "==================  TEST - FK <1> ==================" << endl;
  InterpolantBase *ifk1;
  ifk1 = FindStep(fk,lag,tolerance);
  
  lag->SetStep(step);
  cout << "==================  TEST - FQ <1> ==================" << endl;
  InterpolantBase *ifq1;
  ifq1 = FindStep(fq,lag,tolerance);
  
  
  lag->SetDegree(3);
  lag->SetStep(step);
  lag->AddCond(IInterpolation::LeftBC,2,fk.Diffn(a,2));	
  lag->AddCond(IInterpolation::RightBC,2,fk.Diffn(b,2));	
  cout << "==================  TEST - FK <3> ==================" << endl;
  InterpolantBase *ifk3;
  ifk3 = FindStep(fk,lag,tolerance);
  
  
  lag->SetStep(step);
  lag->ClearCondition(IInterpolation::LeftBC);
  lag->ClearCondition(IInterpolation::RightBC);
  lag->AddCond(IInterpolation::LeftBC,2,fq.Diffn(a,2));	
  lag->AddCond(IInterpolation::RightBC,2,fk.Diffn(b,2));
  cout << "==================  TEST - FQ <3> ==================" << endl;
  InterpolantBase *ifq3; 
  ifq3 = FindStep(fq,lag,tolerance);
  
  for(int i = 0; i < n_count; i++)
  {
    cout << "Measuring time: " << n[i] << " evaluations" <<  endl;
  
    cout << "fk: " << MeasureTime(&fk,a,b,n[i]) << endl;
    cout << "fk_int<0>: " << MeasureTime(ifk0,a,b,n[i]) << "\t" << ifk0->GetCount() << endl;
    cout << "fk_int<1>: " << MeasureTime(ifk1,a,b,n[i]) << "\t" << ifk1->GetCount() << endl;
    cout << "fk_int<3>: " << MeasureTime(ifk3,a,b,n[i]) << "\t" << ifk3->GetCount() << endl;
  
    cout << endl;
    cout << "fq: " << MeasureTime(&fq,a,b,n[i]) << endl;
    cout << "fq_int<0>: " << MeasureTime(ifq0,a,b,n[i]) << "\t" << ifq0->GetCount() << endl;
    cout << "fq_int<1>: " << MeasureTime(ifq1,a,b,n[i]) << "\t" << ifq1->GetCount() << endl;
    cout << "fq_int<3>: " << MeasureTime(ifq3,a,b,n[i]) << "\t" << ifq3->GetCount() << endl;
  }
  
  delete ifq3; 
  delete ifk3;
  delete ifq1; 
  delete ifk1;
  delete ifq0; 
  delete ifk0;
  delete lag;
  //*/
  //*
  Adaptive *adaptlag = new Adaptive();
    
  adaptlag->SetTolerance(tolerance_0);
  adaptlag->SetStep(step);
  adaptlag->SetInterval(a,b);  
  adaptlag->SetExtrapolation(0,0);
    
  //*
  adaptlag->SetDegree(0);
  cout << "tolerance = " << tolerance_0 << endl;
  cout << "==================  TEST - FK <0> adapt ==================" << endl;
  InterpolantBase *ifka0 = adaptlag->Interpolate(fk);
  
  adaptlag->SetStep(step);
  cout << "==================  TEST - FQ <0> adapt ==================" << endl;
  InterpolantBase *ifqa0 = adaptlag->Interpolate(fq);
  //*/
  
  adaptlag->SetTolerance(tolerance);
  adaptlag->SetDegree(1);
  adaptlag->SetStep(step);
  cout << "tolerance = " << tolerance << endl;
  cout << "==================  TEST - FK <1> adapt ==================" << endl;
  InterpolantBase *ifka1 = adaptlag->Interpolate(fk);
  
  
  adaptlag->SetStep(step);
  cout << "==================  TEST - FQ <1> adapt ==================" << endl;
  InterpolantBase *ifqa1 = adaptlag->Interpolate(fq);
  
  adaptlag->SetDegree(3);
  adaptlag->SetStep(step);
  adaptlag->AddCond(IInterpolation::LeftBC,2,fk.Diffn(a,2));	
  adaptlag->AddCond(IInterpolation::RightBC,2,fk.Diffn(b,2));
  cout << "==================  TEST - FK <3> adapt ==================" << endl;
  InterpolantBase *ifka3 = adaptlag->Interpolate(fk);
  
  
  adaptlag->SetStep(step);
  adaptlag->ClearCondition(IInterpolation::LeftBC);
  adaptlag->ClearCondition(IInterpolation::RightBC);
  
  adaptlag->AddCond(IInterpolation::LeftBC,2,fq.Diffn(a,2));	
  adaptlag->AddCond(IInterpolation::RightBC,2,fq.Diffn(b,2));
  cout << "==================  TEST - FQ <3> adapt ==================" << endl;
  InterpolantBase *ifqa3 = adaptlag->Interpolate(fq);

  for(int i = 0; i < n_count; i++)
  {
    cout << "Measuring time: " << n[i] << " evaluations" <<  endl;
  
    cout << "fk: " << MeasureTime(&fk,a,b,n[i]) << endl;
    cout << "fk_inta<0>: " << MeasureTime(ifka0,a,b,n[i]) << "\t" << ifka0->GetCount() << endl;
    cout << "fk_inta<1>: " << MeasureTime(ifka1,a,b,n[i]) << "\t" << ifka1->GetCount() << endl;
    cout << "fk_inta<3>: " << MeasureTime(ifka3,a,b,n[i]) << "\t" << ifka3->GetCount() << endl;
  
    cout << endl;
    cout << "fq: " << MeasureTime(&fq,a,b,n[i]) << endl;
    cout << "fq_inta<0>: " << MeasureTime(ifqa0,a,b,n[i]) << "\t" << ifqa0->GetCount() << endl;
    cout << "fq_inta<1>: " << MeasureTime(ifqa1,a,b,n[i]) << "\t" << ifqa1->GetCount() << endl;
    cout << "fq_inta<3>: " << MeasureTime(ifqa3,a,b,n[i]) << "\t" << ifqa3->GetCount() << endl;
  }

  delete ifqa3; 
  delete ifka3;
  delete ifqa1; 
  delete ifka1;
  delete ifqa0; 
  delete ifka0;
  delete adaptlag;
  //*/
}

void test_BC()	//test different BC on sqrt(x) and finds step for tolerance
{
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
    
    //for tolerance 0.00001
    //Interation = 16 rel. err = 2.71416015886692e-06 abs. err = 5.42832031773384e-06
    //with step = 3.0517578125e-05
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
    delete lag;
    delete f;
}

int main(int argc, char **arg)
{
    cout.precision(15);
    cout << "Output precision has been set to 15 digits." << endl;
    
    //test_BC();
    //test_interpolant();
    //test_polynomial();
    //test_adaptivesimpson();
    //test_lagrange_sqrt();
    //test_lagrange_absfunc();
    test_FK_FQ();
    //test_constant();
    
    cout << "==============main() END=================" << endl;
    return 0;
}
