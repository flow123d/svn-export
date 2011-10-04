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
 * $Id: darcy_flow_mh.hh 877 2011-02-04 13:13:25Z jakub.sistek $
 * $Revision: 877 $
 * $LastChangedBy: jakub.sistek $
 * $LastChangedDate: 2011-02-04 14:13:25 +0100 (Fri, 04 Feb 2011) $
 *
 * @file
 * @brief Output class for darcy_flow_mh model.
 *
 *  @author Jan Brezina
 *
 */

#ifndef DARCY_FLOW_MH_OUTPUT_HH_
#define DARCY_FLOW_MH_OUTPUT_HH_

#include "mesh/mesh.h"

class DarcyFlowMH;
class OutputTime;


/**
 * Actually this class only collect former code from postprocess.*
 * This code depends on values stored in mesh and has to be changed to use fields or other data provided by
 * interface to darcy_flow. We have to relay on the interface in order to allow different implementation of darcy_flow.
 *
 * Principal functionalities of current postprocess are:
 * - move computed values into mesh - this can be removed after we get new output classes,
 *   other dependent code can use directly field interface
 * - compute interpolation to nodes - this can be external and operate directly on fields
 * - compute water balances over materials and boundary parts - is feasible through field interface
 *
 * Further functionality of this class should be:
 * - prepare data for general output classes
 *
 */
class DarcyFlowMHOutput {
public:
    DarcyFlowMHOutput(DarcyFlowMH *flow) ;
    ~DarcyFlowMHOutput();

    void postprocess();

    /** \brief Calculate values for output.  **/
    void output();

private:
    void make_side_flux();
    void make_element_scalar();
    void make_element_scalar(double* scalars);
    void make_element_vector();

    void make_element_vector_line(ElementFullIter);
    void make_element_vector_triangle(ElementFullIter);
    void make_element_vector_tetrahedron(ElementFullIter);

    void make_sides_scalar();
    /**
     * \brief Calculate nodes scalar,
     * store it in double* node_scalars instead of node->scalar
     *  */
    void make_node_scalar_param(double* scalars);
    void make_node_scalar();
    void make_neighbour_flux();
    //void make_previous_scalar();

    /**
     * Calculate and output water balance over material subdomains and boudary fluxes.
     * Works only for steady flow.
     *
     * TODO:
     * - fix it also for unsteady flow
     * - create separate class for this caculations and output
     * - create class for output of tables with support to output into various file formats
     *   like GNUplot of excel/open calc
     **/
    void water_balance();
    double calc_water_balance();

    DarcyFlowMH *darcy_flow;
    Mesh *mesh_;
    OutputTime *output_writer;
    TimeMark::Type output_mark_type;

    /** \brief Array for storing nodes scalar, node index is used as index of array */
    double* node_scalars;
    /** \brief Array for storing elements scalar, element index is used as index of array */
    double* ele_scalars;

    /** \brief Structure for storing elements vectors, used in register_elem_data  */
    typedef std::vector< vector<double> > VectorFloatVector;
    typedef struct OutVector {
        VectorFloatVector   *vectors;
        string              name;
        string              unit;
    }_OutVector;

    /** \brief Vector for storing elements vectors */
    struct OutVector *element_vectors;

    /// Temporary solution for writing balance into separate file.
    FILE *balance_output_file;
};


#endif /* DARCY_FLOW_MH_OUTPUT_HH_ */
