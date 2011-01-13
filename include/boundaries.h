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

#ifndef BOUNDARIES_H
#define BOUNDARIES_H

#include "mesh.h"

struct Boundary;
struct Transport_bcd;

/**
 * Setting boundary conditions should have two staps.
 * 1) Denote by numbers segments of mesh boundary. Possibly every side can be boundary.
 * 2) Assign particular type and values of BC on every boundary segment.
 *
 * So in future Boundary should keep only side and segment and there should be
 * one Boundary for every external side. Side is external either when it does not
 * neighbor with another element or when it belongs to an segment.
 */


//=============================================================================
// STRUCTURE OF THE BOUNDARY CONDITION
//=============================================================================
class Boundary
{
public:
    // Data readed from boundary conditions files (REMOVE)
    int      type;      // Type of boundary condition
    double   scalar;    // Scalar - for Dirichlet's or Newton's type
    double   flux;      // Flux - for Neumann's type or source
    double   sigma;     // Sigma koef. - for Newton's type

    int      group;     // Group of condition
    // Topology of the mesh
    struct Side *side;      // side, where prescribed
    struct Transport_bcd    *transport_bcd;  // transport boundary condition (REMOVE)

    // Misc
    int  aux;       // Auxiliary flag
    double   faux;      // Auxiliary number
};
#define DIRICHLET   1
#define NEUMANN     2
#define NEWTON      3


void read_boundary(Mesh*);
void boundary_calculation_mh(Mesh*);

#endif
//-----------------------------------------------------------------------------
// vim: set cindent:
