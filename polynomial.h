
#ifndef POLYNOMIAL_H
#define POLYNOMIAL_H

#include <vector>
#include "functordiffbase.h"

namespace Interpolation
{
  
struct der;
 
/// class Polynomial.
/** Defines a polynomial of n-th degree on an interval \<a,b\>.
  * It has several methods, that counts value, derivate and integral
  * in chosen point. Can be constructed by several types of constructors
  * with different parameters.
  */
class Polynomial
{
  private:
    /** Vector of coeficients of the polynomial
      * in the form a_n*x^n + a_{n-1}*x^{n-1}...a_1*x + a_0
      * where: a_N...coef[N], a_0...coef[0].
      */
    std::vector<double> coefs;
    
    ///"a" and "b" are lower and upper bounds of polynom
    double a, b;
    ///degree of the polynomial
    char degree;
  public:   
    Polynomial(char degree)
      : degree(degree) {}
      
    Polynomial(char degree, double a, double b, std::vector<double> coefs)
      : a(a), b(b), coefs(coefs)
    {
      degree = coefs.size() - 1;
    }
    
    ///sets the interval for the polynomial
    void SetInterval(double a, double b)
    {
      this->a = a;
      this->b = b;
    }
    
    ///sets the coeficients "coef"
    void SetCoeficients(double *coeficients, int size)
    {
      coefs.resize(size);
      for(int i = 0; i < size; i++)
	coefs[i] = *(coeficients + i);
    }
    
    ///returns lower limit of the polynomial - "a"
    double GetLower()
    {return a;}
    
    ///returns upper limit of the polynomial - "b"
    double GetUpper()
    {return b;}
    
    ///returns value of the polynomial P(x), uses Horner schema
    double Value(double x);

    
    ///returns derivate of the polynomial P'(x), uses Horner schema
    der Diff(double x);

    
    ///returns intergral of the polynomial P(x) in explicit limits a,b, uses Horner schema
    double Integral(double a, double b);
    
    ///returns intergral of the polynomial P(x) in default limits a,b, uses Horner schema
    double Integral(void);	//int. between a and b
    
};

}

#endif	//POLYNOMIAL_H