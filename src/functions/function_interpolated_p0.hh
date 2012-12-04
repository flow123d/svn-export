/*!
 *
 * Copyright (C) 2007 Technical University of Liberec.  All rights reserved.
 *
 * Please make a following refer to Flow123d on your project site if you use the program for any purpose,
 * especially for academic research:
 * Flow123d, Research Centre: Advanced Remedial Technologies, Technical University of Liberec, Czech Republic
 *
 * This program is free software; you can redistribute it and/or modify it under the terms
 * of the GNU General Public License version 3 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program; if not,
 * write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 021110-1307, USA.
 *
 *
 * $Id: function_interpolated_p0.hh 1567 2012-02-28 13:24:58Z jan.brezina $
 * $Revision: 1567 $
 * $LastChangedBy: jan.brezina $
 * $LastChangedDate: 2012-02-28 14:24:58 +0100 (Tue, 28 Feb 2012) $
 *
 *
 */

#include "functions/functions_all.hh"

#ifndef FUNCTION_INTERPOLATED_P0_HH_
#define FUNCTION_INTERPOLATED_P0_HH_

#include "function_base.hh"
#include "mesh/mesh.h"
#include "mesh/mesh_types.hh"
#include "system/system.hh"
#include "mesh/msh_gmshreader.h"
#include "new_mesh/bih_tree.hh"
#include "new_mesh/ngh/include/abscissa.h"
#include "new_mesh/ngh/include/triangle.h"
#include "new_mesh/ngh/include/tetrahedron.h"


template <int dim>
class FunctionInterpolatedP0: public FunctionBase<dim> {
public:

	/**
	 * Constructor
	 */
	FunctionInterpolatedP0(const unsigned int n_components=1, const double init_time=0.0);


	/**
	 * Declare Input type.
	 */
	static Input::Type::Record &get_input_type();

	/**
	 * Initialization from the input interface.
	 */
	void init_from_input(Input::Record rec);

    /**
     * Set element for interpolation
     */
	void set_element(Element *element);

	/**
	 * Set sources files of interpolation
	 *
	 * @param mesh_file file contained data of mesh
	 * @param raw_output file contained output
	 *
	 * TODO: use streams instead of filenames (better testing)
	 */
	void set_source_of_interpolation(const FilePath & mesh_file,
									 const FilePath & raw_output);

    /**
     * Returns one scalar value in one given point.
     */
    virtual double value(const Point<dim> &p, const unsigned int  component = 0) const;
    /**
     * Returns one vector value in one given point.
     */
    virtual void   vector_value(const Point<dim> &p, std::vector<double>     &value) const;

    /**
     * Returns std::vector of scalar values in several points at once.
     */
    virtual void   value_list (const std::vector< Point<dim> >  &point_list,
                       std::vector<double>         &value_list,
                       const unsigned int  component = 0) const;

    /**
     * Returns std::vector of scalar values in several points at once.
     */
    virtual void   vector_value_list (const std::vector< Point<dim> >    &point_list,
                              std::vector< std::vector<double> >      &value_list) const;

protected:
	/// element for interpolation
	ElementIter element_;

	/// mesh
	Mesh* mesh_;

	/// value of pressure in element_
	double pressure_;

	/// vector of pressures in nodes
	std::vector<double> pressures_;

	/// vector stored suspect elements in calculating the intersection
	std::vector<unsigned int> searchedElements_;

	/// tree of mesh elements
	BIHTree* bihTree_;

	/**
	 * Read pressures from file and put them to vector pressures_
	 *
	 * @param raw_output file contained output
	 */
	void read_pressures(FILE* raw_output);

	/**
	 * Read scalar element data with name @p field_name using tokenizer @p tok initialized
	 * over a GMSH file or stream.
	 *
	 * This needs lot of work to be general enough to be outside of this class
	 * TODO:
	 * - we need concept of Fields so that we can fill corresponding vectors
	 * - then we should support scalar as well as vector or even tensor data
	 * - support for time dependent data
	 * - selective reading on submesh (parallelism - subdomains, or boundary data)
	 *
	 */
	void read_element_data_from_gmsh(Tokenizer &tok, const  string &field_name);


	/**
	 * Calculate pressures in triangle element
	 */
	void calculate_triangle_pressure(TTriangle &element);

	/**
	 * Calculate pressures in abscissa element
	 */
	void calculate_abscissa_pressure(TAbscissa &element);

	/**
	 * Create tetrahedron from element
	 */
	void createTetrahedron(Element *ele, TTetrahedron &te);

	/**
	 * Create triangle from element
	 */
	void createTriangle(Element *ele, TTriangle &tr);

	/**
	 * Create abscissa from element
	 */
	void createAbscissa(Element *ele, TAbscissa &ab);
};



#endif /* FUNCTION_INTERPOLATED_P0_HH_ */
