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
 * @ingroup mesh
 * @brief  Mesh construction
 *
 */

#include <unistd.h>

#include "mesh/ini_constants_mesh.hh"
#include "constantdb.h"

#include "system/system.hh"
#include "xio.h"

#include<boost/tokenizer.hpp>

#include "boost/lexical_cast.hpp"

#include "problem.h"
#include "mesh/mesh.h"

// think about following dependencies
#include "mesh/boundaries.h"

#include "transport.h"

//TODO: sources, concentrations, initial condition  and similarly boundary conditions should be
// instances of a Element valued field
// concentrations is in fact reimplemented in transport REMOVE it HERE

// After removing non-geometrical things from mesh, this should be part of mash initializing.
#include "topology.cc"
#include "msh_reader.h"
#include "msh_gmshreader.h"

void count_element_types(Mesh*);
void read_node_list(Mesh*);

Mesh::Mesh() {
    xprintf(Msg, " - Mesh()     - version with node_vector\n");

    n_materials = NDEF;
//    concentration = NULL;
//    l_concentration = NULL;
//    transport_bcd = NULL;
//    l_transport_bcd = NULL;
    //	n_sources        = NDEF;
    //	source           = NULL;
    //	l_source         = NULL;
    n_sides = NDEF;
    side = NULL;
    l_side = NULL;
    n_insides = NDEF;
    n_exsides = NDEF;
    n_neighs = NDEF;
    neighbour = NULL;
    l_neighbour = NULL;

    // Hashes
    n_lines = 0;
    n_triangles = 0;
    n_tetrahedras = 0;

//    concentration_hash = NULL;
//    transport_bcd_hash = NULL;
//    neighbour_hash = NULL;
}

//=============================================================================
// MAKE AND FILL ALL LISTS IN STRUCT MESH
//
// DF - method make_mesh() will be removed outside from this file
//=============================================================================

void make_mesh(struct Problem *problem) {
    F_ENTRY;

    ASSERT(!(problem == NULL), "NULL pointer as argument of function make_mesh()\n");
    const string& mesh_file_name = IONameHandler::get_instance()->get_input_file_name(OptGetStr("Input", "Mesh", NULL));

    Mesh* mesh = new Mesh();

    /* Test of object storage */
    ConstantDB::getInstance()->setObject(MESH::MAIN_INSTANCE, mesh);


    // read all mesh files - this is work for MeshReader
    // DF - elements are read by MeshReader
    // --------------------- MeshReader testing - Begin
    MeshReader* meshReader = new GmshMeshReader();
    meshReader->read(mesh_file_name, mesh);
    // --------------------- MeshReader testing - End

    read_neighbour_list(mesh);
    mesh->read_intersections( OptGetStr( "Input", "Neighbouring", "\\" ) );

    make_side_list(mesh);
    make_edge_list(mesh);

//    make_hashes(problem);
    count_element_types(mesh);

    // topology
    element_to_material(mesh, *(problem->material_database));
    node_to_element(mesh);
    element_to_side_both(mesh);
    neigh_vv_to_element(mesh);
    element_to_neigh_vv(mesh);
    neigh_vb_to_element_and_side(mesh);
    neigh_bv_to_side(mesh);
    element_to_neigh_vb(mesh);
    side_shape_specific(mesh);
    side_to_node(mesh);
    neigh_bb_topology(mesh);
    neigh_bb_to_edge_both(mesh);
    edge_to_side_both(mesh);
    neigh_vb_to_edge_both(mesh);
    side_types(mesh);
    count_side_types(mesh);
    xprintf(MsgVerb, "Topology O.K.\n")/*orig verb 4*/;


    read_boundary(mesh);

}

//=============================================================================
// COUNT ELEMENT TYPES
//=============================================================================

void count_element_types(Mesh* mesh) {
    //ElementIter elm;

    FOR_ELEMENTS(elm)
    switch (elm->type) {
        case 1:
            mesh->n_lines++;
            break;
        case 2:
            mesh->n_triangles++;
            break;
        case 4:
            mesh->n_tetrahedras++;
            break;
    }
}
//=============================================================================
// RETURN MAX NUMBER OF ENTRIES IN THE ROW
//=============================================================================

int *max_entry() {
    Mesh* mesh = (Mesh*) ConstantDB::getInstance()->getObject(MESH::MAIN_INSTANCE);

    int *max_size, size, i;
//    ElementIter elm;

    max_size = (int*) xmalloc(2 * sizeof (int));

    max_size[0] = 0; // entries count
    max_size[1] = 0; // row

    FOR_ELEMENTS(elm) {
        size = 0; // uloha se zapornymi zdroji =1

        FOR_ELEMENT_SIDES(elm, si) { //same dim
            if (elm->side[si]->cond != NULL) size++;
            else
                size += elm->side[ si ]->edge->n_sides - 1;
            if (elm->side[si]->neigh_bv != NULL) size++; // comp model
        } // end same dim


        //   printf("SD id:%d,size:%d\n",elm->id,size);
        //

        FOR_ELM_NEIGHS_VB(elm, i) { // comp model
            size += elm->neigh_vb[i]->n_elements - 1;
            //     printf("VB id:%d,size:%d\n",elm->id,size);
        } // end comp model


        /*
        if (elm->dim > 1)
          FOR_NEIGHBOURS(ngh)
            FOR_NEIGH_ELEMENTS(ngh,n)
              if (ngh->element[n]->id == elm->id && n == 1)
                size++;
         */

        size += elm->n_neighs_vv; // non-comp model

        max_size[0] += size;
        if (max_size[1] < size) max_size[1] = size;
    }
    // getchar();
    return max_size;
}


void Mesh::read_intersections(string file_name) {

using namespace boost;


file_name = IONameHandler::get_instance()->get_input_file_name(file_name);
ElementFullIter master(element), slave(element);

char *tmp_line;
FILE *in = xfopen( file_name, "rt" );

tokenizer<>::iterator tok;

    xprintf( Msg, "Reading intersections...")/*orig verb 2*/;
    skip_to( in, "$Intersections" );
    xfgets( tmp_line, LINE_SIZE - 2, in );
    int n_intersect = atoi( xstrtok( tmp_line) );
    INPUT_CHECK( n_intersect >= 0 ,"Negative number of neighbours!\n");

    intersections.reserve(n_intersect);

    for(int i=0; i<n_intersect; i++) {
        xfgets( tmp_line, LINE_SIZE - 2, in );
        string line = tmp_line;
        tokenizer<> line_tokenizer(line);
        tok=line_tokenizer.begin();


        try {
        	++tok; // skip id token
        	int type = lexical_cast<int>(*tok); ++tok;
            int master_id = lexical_cast<int>(*tok); ++tok;
            int slave_id = lexical_cast<int>(*tok); ++tok;
            int sigma = lexical_cast<int>(*tok); ++tok;

            int n_intersect_points = lexical_cast<int>(*tok); ++tok;
            master=element.find_id(master_id);
            slave=element.find_id(slave_id);
            intersections.push_back(Intersection(n_intersect_points-1, master, slave, tok));
        } catch( bad_lexical_cast &) {
            xprintf(UsrErr, "Wrong number at line %d in file %s\n",i, file_name.c_str());
        }


    }


    xprintf( Msg, "O.K.\n")/*orig verb 2*/;

}


//-----------------------------------------------------------------------------
// vim: set cindent:
