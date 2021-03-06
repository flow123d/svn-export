#ifndef IntersectionLocalH
#define IntersectionLocalH

#include <iostream>
#include <vector>
#include <stdio.h>
//#include "element.h"

using namespace std;
class TPoint;
//class TElement;

//class IntersectionPoint - dva vektory lokalnich souradnic
class IntersectionPoint {
public:
	IntersectionPoint(const std::vector<double> &c1, const std::vector<double> &c2)
		: coord1(c1), coord2(c2) {}
	IntersectionPoint(const IntersectionPoint &LC)
		: coord1(LC.coord1), coord2(LC.coord2) {}
	inline std::vector<double> &el1_coord()
    		{return coord1;}
	inline std::vector<double> &el2_coord()
			{return coord2;}
	bool operator ==(const IntersectionPoint&);
protected:
	std::vector<double> coord1;
	std::vector<double> coord2;

	friend class IntersectionLocal;
	friend IntersectionPoint *interpolate(IntersectionPoint &A1, IntersectionPoint &A2, double t);
};
IntersectionPoint *interpolate(IntersectionPoint &A1, IntersectionPoint &A2, double t);


class IntersectionLocal {
public:
    typedef enum {
        point,
        line,
        area
    } IntersectionType;

    IntersectionLocal(IntersectionType i_type);
    IntersectionLocal(IntersectionLocal*);
    ~IntersectionLocal();

    //void set_elements(TElement *elem1, TElement *elem2); //metoda na naplneni ele1, ele2
    void add_local_coord(const std::vector<double> &coordin1, const std::vector<double> &coordin2); //metoda na pridani souradnic do i_points
    void add_local_point(IntersectionPoint *InPoint);
    void print(FILE *out_file);

    //void AddNewLocalcoord(); //doplnit predavany parametr, pridat novou i_points

    static int getNumInstances() {
		return IntersectionLocal::numberInstance;
	}

    inline IntersectionType get_type()
        {return type; }
    inline IntersectionPoint * get_point(const unsigned int index)
    {
          if (index >= i_points.size() ) return NULL;
          else return i_points[index];
    }

private:
    static int numberInstance;
    int id;

    std::vector<IntersectionPoint *> i_points; //vektor ukazatelu na dvojice lokal. souradnic
    IntersectionType type;
    //TElement *ele1;
    //TElement *ele2;

    int generateId();
};

#endif /* IntersectionLocalH */
