#ifndef MYFUNCTION_H
#define MYFUNCTION_H

#include <math.h>

//main mathematical function
class MyFunction	
{
public:
	//templated operator (), which
	//returns the value of function in x
	template<class T>
	T operator()(const T& x)
	{
	  return x*x*x;
	}
};

class x5	
{
public:
	//templated operator (), which
	//returns the value of function in x
	template<class T>
	T operator()(const T& x)
	{
	  return x*x*x*x*x;
	}
};

class SqrtFunc	
{
public:
	//templated operator (), which
	//returns the value of function in x
	template<class T>
	T operator()(const T& x)
	{
	  //return x;
	  //return 3*x*x+2*x*x+0.43*x+2.14;
	  return sqrt(x);
	}
};

class AbsFunc	
{
public:
	//templated operator (), which
	//returns the value of function in x
	template<class T>
	T operator()(const T& x)
	{
	  return -fabs(x-3.5)+7;
	}
};

class FK            //FK - hydraulic conductivity function
{
	public:
	template <class T>
	T operator()(const T& h)
	{
        double Bpar = 0.5;
        double PPar = 2;
        double n = 1.111,
               Qr = 0.001,
               Qs = 0.436,
               Qa = 0.001,
               Qm = 0.439,
               Qk = 0.436,
               Alfa = 0.733,
               Ks = 0.0505,
               Kk = 0.0505,
               m,Kr,Hr,Hk,Hs,FFQr,FFQ,FFQk,
               C1Qee,C2Qee,Qeer,Qees,Qeek,Qee,Qe,Qek,C1Qe,C2Qe,Q;
      
       m = 1-1/n;            
       C1Qee = 1/(Qm - Qa);
       C2Qee = -Qa/(Qm - Qa);
       Qeer = fmax(C1Qee*Qr + C2Qee,1E-3);
       Qees = fmin(C1Qee*Qs + C2Qee,1 - 1E-15);
       Qeek = fmin(C1Qee*Qk + C2Qee,Qees);
       Hr = -1/Alfa*pow(pow(Qeer,-1/m)-1,1/n);
       Hs = -1/Alfa*pow(pow(Qees,-1/m)-1,1/n); 
       Hk = -1/Alfa*pow(pow(Qeek,-1/m)-1,1/n); 
       
       if (h <= Hr) return Ks*(1E-9);
       else if(h < Hk)
       {
            Q = Qa + (Qm - Qa)*pow((1 + pow(-Alfa*h,n)),-m);
            Qee = C1Qee*Q + C2Qee;
            FFQr = pow(1 - pow(Qeer,1/m),m);
            FFQ = pow(1 - pow(Qee,1/m),m);
            FFQk = pow(1 - pow(Qeek,1/m),m);
            C1Qe = 1/(Qs - Qr);
            C2Qe = -Qr/(Qs - Qr);
            Qe = C1Qe*Q + C2Qe;
            Qek = C1Qe*Qk + C2Qe;
            Kr = pow(Qe/Qek,Bpar)*pow((FFQr - FFQ)/(FFQr - FFQk),PPar) * Kk/Ks;
            return fmax(Ks*Kr,Ks*(1E-9));  
      }
      else if(h <= Hs)
      {
           Kr = (1-Kk/Ks)/(Hs-Hk)*(h-Hs) + 1;
           return Ks*Kr;
      }        
      else return Ks;
   }               
	
};



class FQ            //FQ - water saturation function
{
	public:
	template <class T>
	T operator()(const T& h)
	{
        double n = 1.111,
               Qr = 0.001,
               Qs = 0.436,
               Qa = 0.001,
               Qm = 0.439,
               Alfa = 0.733,
               m,C1Qee,C2Qee,Qeer,Qees,Qee,Hr,Hs;
               
        m = 1 - 1/n;
        C1Qee = 1/(Qm - Qa);
        C2Qee = -Qa/(Qm - Qa);
        Qeer = fmax(C1Qee*Qr + C2Qee,1E-3);
        Qees = fmin(C1Qee*Qs + C2Qee,1-1E-15);
        Hr = -1/Alfa*pow(pow(Qeer,-1/m)-1,1/n);
        Hs = -1/Alfa*pow(pow(Qees,-1/m)-1,1/n); 
        if(h <= Hr) return Qr;
        else if(h < Hs)
             {
              Qee = pow(1+pow(-Alfa*h,n),-m);
              return Qa + (Qm - Qa)*Qee;
              }
        else return Qs;
     }
};

#endif	//MYFUNCTION_H