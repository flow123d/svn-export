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

#endif	//MYFUNCTION_H