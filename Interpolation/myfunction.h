#ifndef MYFUNCTION_H
#define MYFUNCTION_H

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

#endif	//MYFUNCTION_H