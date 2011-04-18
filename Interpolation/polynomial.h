
#ifndef POLYNOMIAL_H
#define POLYNOMIAL_H

#include <iostream>
#include <vector>

#include "massert.h"
#include "functordiffbase.h"

namespace Interpolation
{
  
//struct der;
 
/// class Polynomial.
/** Defines a polynomial of n-th degree on an interval \<a,b\>.
  * It has several methods, that counts value, derivate and integral
  * in chosen point. Can be constructed by several types of constructors
  * with different parameters.
  */
class Polynomial : public FunctorDiffBase
{
  private:
    ///degree of the polynomial
    unsigned char degree;
    
    ///"a" and "b" are lower and upper bounds of polynomial
    double a,b;
    
    /** Vector of coeficients of the polynomial
      * in the form a_n*x^n + a_{n-1}*x^{n-1}...a_1*x + a_0
      * where: a_N...coef[N], a_0...coef[0].
      */
    std::vector<double> coefs;
    
  public:   
    Polynomial();
    
    /** Constructor.
      * @param degree is the degree of the polynomial
      */
    Polynomial(const unsigned char &degree);
        
    /** Constructor.
      * @param a is the lower bound of the polynomial
      * @param b is the upperbound of the polynomial
      * @param coefs is the vector of coeficients of the polynomial
      */
    Polynomial(const double &a, const double &b, const std::vector<double> &coefs);
    
    ///copy constructor
    Polynomial(const Polynomial &pol);   
    
    ///sets the interval for the polynomial
    void SetInterval(const double &a, const double &b);
    
    ///returns lower limit of the polynomial - "a"
    double GetLower();
    
    ///returns upper limit of the polynomial - "b"
    double GetUpper();
      
    
    ///sets the coeficients "coef"
    void SetCoeficients(double *coeficients, const unsigned int &size);
    
    ///Returns potiner to the vector of coeficients
    std::vector<double> *GetCoefs();
    
    ///returns value of the polynomial P(x), uses Horner schema
    virtual double operator() ( const double &x );
    //virtual double operator() ( const double &x );
    
    ///returns derivate of the polynomial P'(x), uses Horner schema
    der Diff(const double &x);
    
    ///returns intergral of the polynomial P(x) in explicit limits a,b, uses Horner schema
    double Integral(const double &a, const double &b);
    
    ///returns intergral of the polynomial P(x) in default limits a,b, uses Horner schema
    double Integral(void);	//int. between a and b 
};

}

#endif	//POLYNOMIAL_H