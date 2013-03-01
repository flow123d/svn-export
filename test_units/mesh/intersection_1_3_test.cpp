/*
 * muj_test.cpp
 *
 *  Created on: 6.2.2013
 *      Author: viktor
 */
#include <gtest/gtest.h>
#include "system/system.hh"
#include "system/file_path.hh"
#include "mesh/ngh/include/point.h"
#include "mesh/ngh/include/intersection.h"
#include "mesh/mesh.h"
#include "mesh/msh_gmshreader.h"
#include "mesh/bih_tree.hh"
#include "fields/field_interpolated_p0.hh"

//#include "new_mesh/ngh/include/point.h"

#define DEBUG

// muj test
TEST(intersections, 1d_3d){
	unsigned int elementLimit = 20;
	FilePath mesh_file("line_cube.msh", FilePath::input_file); // krychle 1x1x1 param = 0.2; sít úseček param = 0.1
	Mesh mesh_krychle;
	GmshMeshReader reader(mesh_file);
	BoundingBox bb;
	std::vector<unsigned int> searchedElements;

	reader.read_mesh(&mesh_krychle);

	BIHTree bt(&mesh_krychle, elementLimit);

	//bb.set_bounds(arma::vec3("0 0 0"), arma::vec3("1 1 1"));
	//bt.find_elements(bb, searchedElements);

	//START_TIMER("vypocet_intersection_1_3");

	FOR_ELEMENTS(&mesh_krychle, elm) {
	         if (elm->dim() == 1) {
	        	TAbscissa ta;
	        	FieldInterpolatedP0<3,FieldValue<3>::Scalar>::createAbscissa(elm, ta);
	        	BoundingBox elementBoundingBox = ta.get_bounding_box();
	        	bt.find_bounding_box(elementBoundingBox, searchedElements);
	        	TTetrahedron tt;
	        	TIntersectionType iType;
	        	double measure;

	        	for (std::vector<unsigned int>::iterator it = searchedElements.begin(); it!=searchedElements.end(); it++)
	        		{
	        			int idx = *it;
	        			ElementFullIter ele = mesh_krychle.element( idx );
	        			if (ele->dim() == 3) {
	        				FieldInterpolatedP0<3,FieldValue<3>::Scalar>::createTetrahedron(ele, tt);
	        				GetIntersection(ta, tt, iType, measure);
	        				/*if (iType == line) {
	        					xprintf(Msg, "%d %d \n",elm.id(),ele.id());
	        				              }*/

	        			}
	        		}

	         }
	       }

	//END_TIMER("vypocet_intersection_1_3");

	xprintf(Msg, "Test - intersection is hexagon\n");

}



