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
 * $Id$
 * $Revision$
 * $LastChangedBy$
 * $LastChangedDate$
 *
 * @file
 * @brief ???
 *
 */

#ifndef MAKE_MESH_H
#define MAKE_MESH_H

#include <vector>
#include <mesh_types.hh>

#include "nodes.h"
#include "elements.h"
#include "sides.h"
#include "edges.h"
#include "neighbours.h"
#include "boundaries.h"


#define ELM  0
#define BC  1
#define NODE  2

/**
 *  This parameter limits volume of elements from below.
 */
#define MESH_CRITICAL_VOLUME 1.0E-12

/**
 * Provides for statement to iterate over the Nodes of the Mesh.
 * The parameter is FullIter local variable of the cycle, so it need not be declared before.
 * Macro assume that variable Mesh *mesh; is declared and points to a valid Mesh structure.
 */
#define FOR_NODES(i) \
    for( NodeFullIter i( mesh->node_vector.begin() ); \
        i != mesh->node_vector.end(); \
        ++i)

/**
 * Macro for conversion form Iter to FullIter for nodes.
 */
#define NODE_FULL_ITER(i) \
    mesh->node_vector.full_iter(i)

/**
 * Macro to get "NULL" ElementFullIter.
 */
#define NODE_FULL_ITER_NULL \
    NodeFullIter(mesh->node_vector)


#define FOR_NODE_ELEMENTS(i,j)   for((j)=0;(j)<(i)->n_elements();(j)++)
#define FOR_NODE_SIDES(i,j)      for((j)=0;(j)<(i)->n_sides;(j)++)

//=============================================================================
// STRUCTURE OF THE MESH
//=============================================================================

class Mesh {
private:

public:
    Mesh();

    inline unsigned int n_elements() const {
        return element.size();
    }

    inline unsigned int n_boundaries() const {
        return boundary.size();
    }

    // Files
    // DF - Move to ConstantDB
    // char *geometry_fname; // Name of file of nodes and elems
    // char *concentration_fname;//Name of file of concentration
    // char *transport_bcd_fname;//Name of file of transport BCD

    NodeVector node_vector;  //
    ElementVector element;   //
    BoundaryVector boundary; //

    flow::VectorId<int> bcd_group_id; // gives a index of group for an id

    int n_materials; // # of materials
    int n_concentrations; // # of concentrations
    int n_substances; // # of substances transported by water
    struct Concentration *concentration; // First concentration
    struct Concentration *l_concentration; // Last concentration
    int n_transport_bcd; // # of transport boundary conditions
    struct Transport_bcd *transport_bcd; // First transport boundary condition
    struct Transport_bcd *l_transport_bcd; // Last transport boundary condition
    int n_sides; // # of sides
    int n_insides; // # of internal sides
    int n_exsides; // # of external sides
    struct Side *side; // First side
    struct Side *l_side; // Last side
    int n_edges; // # of edges
    struct Edge *edge; // First edge
    struct Edge *l_edge; // Last edge
    int n_neighs; // # of neighbours
    struct Neighbour *neighbour; // First neighbour
    struct Neighbour *l_neighbour; // Last neighbour
    // Hashes
    int max_edg_id;
    int max_side_id;
    int max_bou_id; // Highest id number of boundary
    int max_con_id; // Highest id number of concentration
    int max_tbc_id; // Highest id number of transport boundary
    int max_ngh_id; // Highest id number of neighbouring

    int n_lines; // Number of line elements
    int n_triangles; // Number of triangle elements
    int n_tetrahedras; // Number of tetrahedra elements

//    int *epos_id; // Element position -> ID list
//    int *spos_id; // Side position -> ID list
//   int *npos_id; // Node position -> ID list

    struct Edge **edge_hash;
    struct Side **side_hash;
    //struct Boundary **boundary_hash; // Boundary id # -> ptr to boundary
    struct Concentration **concentration_hash;
    struct Transport_bcd **transport_bcd_hash;
    struct Neighbour **neighbour_hash; // Neighbour id # -> neighbour index
};

/**
 * Provides for statement to iterate over the Elements of the Mesh.
 * The parameter is FullIter local variable of the cycle, so it need not be declared before.
 * Macro assume that variable Mesh *mesh; is declared and points to a valid Mesh structure.
 */
#define FOR_ELEMENTS(__i) \
    for( ElementFullIter __i( mesh->element.begin() ); \
        __i != mesh->element.end(); \
        ++__i)

/**
 * Macro for conversion form Iter to FullIter for elements.
 */
#define ELEMENT_FULL_ITER(i) \
    mesh->element.full_iter(i)

/**
 * Macro to get "NULL" ElementFullIter.
 */
#define ELEMENT_FULL_ITER_NULL \
    ElementFullIter(mesh->element)


#define FOR_BOUNDARIES(i) \
for( BoundaryFullIter i( mesh->boundary.begin() ); \
    i != mesh->boundary.end(); \
    ++i)

/**
 * Macro for conversion form Iter to FullIter for boundaries.
 */
#define BOUNDARY_FULL_ITER(i) \
    mesh->boundary.full_iter(i)

/**
 * Macro to get "NULL" BoundaryFullIter.
 */
#define BOUNDARY_NULL \
    BoundaryFullIter(mesh->boundary)


void make_mesh(struct Problem*);
int *max_entry();


#endif
//-----------------------------------------------------------------------------
// vim: set cindent:
