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


#include "system/system.hh"
#include "system/xio.h"
#include "input/input_type.hh"

#include <boost/tokenizer.hpp>
#include "boost/lexical_cast.hpp"

#include "mesh/mesh.h"

// think about following dependencies
#include "mesh/boundaries.h"
#include "mesh/accessors.hh"


//TODO: sources, concentrations, initial condition  and similarly boundary conditions should be
// instances of a Element valued field
// concentrations is in fact reimplemented in transport REMOVE it HERE

// After removing non-geometrical things from mesh, this should be part of mash initializing.
#include "mesh/topology.cc"
#include "mesh/msh_reader.h"
#include "mesh/msh_gmshreader.h"
#include "mesh/region.hh"

void count_element_types(Mesh*);
void read_node_list(Mesh*);


using namespace Input::Type;

Record BoundarySegment::input_type
    = Record("BoundarySegment","Record with specification of boundary segments,\n"
            "i.e. subsets of the domain boundary where we prescribe one boundary condition.\n"
            "Currently very GMSH oriented. (NOT IMPLEMENTED YET)")
    .declare_key("physical_domains", Array(Integer(0)),
                "Numbers of physical domains (submeshes) that forms a segment and that "
                "will be removed from the computational mesh.")
    .declare_key("elements", Array(Integer(0)),
                        "Numbers of elements that forms a segment and that "
                        "will be removed from the computational mesh.")
    .declare_key("sides", Array( Array(Integer(0), 2,2) ),
                        "Pairs [ element, local_side] specifying sides that are part of the boundary segment."
                        "Sides are NOT removed from computation.");


Record Mesh::input_type
	= Record("Mesh","Record with mesh related data." )
	.declare_key("mesh_file", FileName::input(), Default::obligatory(),
			"Input file with mesh description.")
	.declare_key("regions", Array( RegionDB::region_input_type ), Default::optional(),
	        "List of additional region definitions not contained in the mesh.")
	.declare_key("sets", Array( RegionDB::region_set_input_type), Default::optional(),
	        "List of region set definitions.")
    .declare_key("boundary",
            Record( "BoundaryLists", "Lists of boundary regions and sets.\n "
                    "All regions with name starting with period '.' are automatically treated as boundary regions.")
            .declare_key("ids", Array( Integer(0) ), Default::optional(),
                    "List with ID numbers of boundary regions.")
            .declare_key("labels", Array( String() ), Default::optional(),
                    "List of labels of boundary regions.")
            .declare_key("sets", Array( String() ), Default::optional(),
                    "List of regions sets whose regions will be marked as boundary.")
            .close(),
            Default::optional(),
            "")
    .declare_key("neighbouring", FileName::input(), Default::obligatory(),
    		"File with mesh connectivity data.");



Mesh::Mesh()
{
    reinit(in_record_);
}



Mesh::Mesh(Input::Record in_record)
: in_record_(in_record) {
    reinit(in_record_);
}



void Mesh::reinit(Input::Record in_record)
{

    n_materials = NDEF;

    n_insides = NDEF;
    n_exsides = NDEF;
    n_sides_ = NDEF;

    // number of element of particular dimension
    n_lines = 0;
    n_triangles = 0;
    n_tetrahedras = 0;

    // indices of side nodes in element node array
    // Currently this is made ad libitum
    // with some ordering here we can get sides with correct orientation.
    // This speedup normal calculation.

    side_nodes.resize(3); // three side dimensions
    for(int i=0; i < 3; i++) {
        side_nodes[i].resize(i+2); // number of sides
        for(int j=0; j < i+2; j++)
            side_nodes[i][j].resize(i+1);
    }

    side_nodes[0][0][0] = 0;
    side_nodes[0][1][0] = 1;


    side_nodes[1][0][0] = 0;
    side_nodes[1][0][1] = 1;

    side_nodes[1][1][0] = 1;
    side_nodes[1][1][1] = 2;

    side_nodes[1][2][0] = 2;
    side_nodes[1][2][1] = 0;


    side_nodes[2][0][0] = 1;
    side_nodes[2][0][1] = 2;
    side_nodes[2][0][2] = 3;

    side_nodes[2][1][0] = 0;
    side_nodes[2][1][1] = 2;
    side_nodes[2][1][2] = 3;

    side_nodes[2][2][0] = 0;
    side_nodes[2][2][1] = 1;
    side_nodes[2][2][2] = 3;

    side_nodes[2][3][0] = 0;
    side_nodes[2][3][1] = 1;
    side_nodes[2][3][2] = 2;
}


unsigned int Mesh::n_sides()
{
    if (n_sides_ == NDEF) {
        n_sides_=0;
        FOR_ELEMENTS(this, ele) n_sides_ += ele->n_sides();
    }
    return n_sides_;
}


//=============================================================================
// COUNT ELEMENT TYPES
//=============================================================================

void Mesh::count_element_types() {
    F_ENTRY;

    FOR_ELEMENTS(this, elm)
    switch (elm->dim()) {
        case 1:
            n_lines++;
            break;
        case 2:
            n_triangles++;
            break;
        case 3:
            n_tetrahedras++;
            break;
        }
}

/**
 *  Setup whole topology for read mesh.
 */
void Mesh::setup_topology(istream *in) {
    F_ENTRY;
    Mesh *mesh=this;


    count_element_types();

    // topology
    //node_to_element();
    if (in) {
        read_neighbours(*in);
    } else if ( ! in_record_.is_empty() ) {
        string ngh_file_name = in_record_.val<FilePath>("neighbouring");
        ifstream ngh_in(  ngh_file_name.c_str(), std::ifstream::in );
        read_neighbours(ngh_in);
    } else {
        return;
    }

    edge_to_side();

    neigh_vb_to_element_and_side();
    element_to_neigh_vb();
    create_external_boundary();

    count_side_types();



    xprintf(MsgVerb, "Topology O.K.\n")/*orig verb 4*/;


    // cleanup
    {
        vector<Neighbour_both> empty_vec;
        neighbours_.swap(empty_vec);
    }

}


/**
 *   Creates back references from nodes to elements.
 *
 *   TODO: This is not necessary after the topology setup so
 *   we should make that as an independent structure which can be easily deleted.
 */
void Mesh::node_to_element()
{
    F_ENTRY;
/*
    int li;
    NodeIter nod;
    ElementIter ele;

    xprintf( MsgVerb, "   Node to element... ");

    // Set counter of elements in node to zero
    FOR_NODES(this,  nod )
        nod->n_elements = 0;
    // Count elements
    FOR_ELEMENTS(this,  ele )
        FOR_ELEMENT_NODES( ele, li ) {
            nod = ele->node[ li ];
            (nod->n_elements)++;
        }
    // Allocate arrays
    FOR_NODES(this,  nod ) {
                if (nod->n_elements == 0)
                        continue;
            nod->element = (ElementIter *) xmalloc( nod->n_elements * sizeof( ElementIter ) );
        nod->aux = 0;
    }
    // Set poiners in arrays
    FOR_ELEMENTS(this,  ele )
        FOR_ELEMENT_NODES( ele, li ) {
            nod = ele->node[ li ];
            nod->element[ nod->aux ] = ele;
            (nod->aux)++;
        }
    xprintf( MsgVerb, "O.K.\n");*/
}

//
void Mesh::count_side_types()
{
    struct Side *sde;

    n_insides = 0;
    n_exsides = 0;
    FOR_SIDES(this,  sde )
        if (sde->is_external()) n_exsides++;
        else n_insides++;
}



void Mesh::read_neighbours(istream &in) {
    char line[LINE_SIZE];   // line of data file
    unsigned int id;

    xprintf( Msg, "Reading neighbours...A\n");
    skip_to( in, "$Neighbours" );
    in.getline(line, LINE_SIZE);

    unsigned int n_neighs = atoi( xstrtok( line ) );
    INPUT_CHECK( n_neighs > 0 ,"Number of neighbours  < 1 in read_neighbour_list()\n");
    neighbours_.resize( n_neighs );

    n_bb_neigh = 0;
    n_vb_neigh = 0;

    for(vector<Neighbour_both>::iterator ngh= neighbours_.begin();
            ngh != neighbours_.end(); ++ngh ) {
        in.getline(line, LINE_SIZE);

        id              = atoi( xstrtok( line ) );
        ngh->type            = atoi( xstrtok( NULL) );

        switch( ngh->type ) {
            case BB_E:
                xprintf(UsrErr, "Not supported - Neighboring of type (10) - of elements of same dimension without local side number!\n");
                break;
            case BB_EL:
                n_bb_neigh++;
                ngh->n_sides = atoi( xstrtok( NULL) );
                INPUT_CHECK(!( ngh->n_sides < 2 ),"Neighbour %d has bad number of elements: %d\n", id, ngh->n_sides );

                ngh->eid = new int [ngh->n_sides];
                ngh->sid = new int [ngh->n_sides];

                for( int i = 0; i < ngh->n_sides; i++) {
                    ngh->eid[ i ] = atoi( xstrtok( NULL) );
                    ngh->sid[ i ] = atoi( xstrtok( NULL) );
                }

                break;
            case VB_ES:
                n_vb_neigh++;
                ngh->n_sides = 2;
                ngh->eid = new int [ngh->n_sides];
                ngh->sid = new int [ngh->n_sides];

                ngh->eid[ 0 ] = atoi( xstrtok( NULL) );
                ngh->eid[ 1 ] = atoi( xstrtok( NULL) );
                ngh->sid[ 0 ] = NDEF;
                ngh->sid[ 1 ] = atoi( xstrtok( NULL) );

                ngh->sigma = atof( xstrtok( NULL) );
                break;
            case VV_2E:
                xprintf(UsrErr, "Not supported - Neighboring of type (30) - Noncompatible only elements!\n");
                break;
            default:
                xprintf(UsrErr,"Neighbour %d is of the unsupported type %d\n", id, ngh->type );
                break;
        }
    }
    xprintf( Msg, " %d VB neighbours %d BB neigs. readed. ", n_vb_neigh, n_bb_neigh );
}



void Mesh::edge_to_side()
{
    F_ENTRY;

    struct Edge *edg;
    Element *ele;

    xprintf( MsgVerb, "   Edge to side and back... \n");

    // count edges (in NGHfile there are missing (??not sure) edges on boundary and between dimensions
    unsigned int n_edges = n_sides();
    for(vector<Neighbour_both>::iterator it= neighbours_.begin();
        it != neighbours_.end(); ++it )
        if ( it->type == BB_EL ) n_edges -= ( it->n_sides - 1 );

    // create edge vector
    edge.resize(n_edges);
    xprintf( Msg, "Created  %d edges.\n.", n_edges );

    // set edge, side connections
    unsigned int i_edge=0;
    for(vector<Neighbour_both>::iterator it= neighbours_.begin();
            it != neighbours_.end(); ++it ) {

        if ( it->type != BB_EL ) continue;

        edg = &( edge[i_edge++] );

        // init edge (can init all its data), set element to edge, for Side iterators
        edg->n_sides = it->n_sides;
        edg->side_ = new SideIter [edg->n_sides];

        for(int si=0; si < it->n_sides; si++) {
            ele = element.find_id( it->eid[si] );
            edg->side_[ si ] = ele->side( it->sid[ si ] );
            ele->edges_[ it->sid[ si ] ] = edg;
        }
    }

    // now the external ones ( pair all remaining edges with external sides)
    FOR_SIDES(this, sde) {
        if ( sde->edge() == NULL ) {
            edg = &( edge[i_edge++] );

            // make external edges and edges on neighborings.
            edg->n_sides = 1;
            edg->side_ = new SideIter [ edg->n_sides ];
            edg->side_[ 0 ] = sde;

            sde->element()->edges_[sde->el_idx()] = edg;
        }
    }
    ASSERT(i_edge == n_edges, "Actual number of edges %d do not match size %d of its array.\n", i_edge, n_edges);

    //FOR_SIDES(mesh, side) ASSERT(side->edge != NULL, "Empty side %d !\n", side->id);
}





/**
 * Make
 ***/
void Mesh::neigh_vb_to_element_and_side()
{

    vb_neighbours_.resize( n_vb_neigh );

    ElementIter ele_lower, ele_higher;
    Edge *edg;

    xprintf( MsgVerb, "   Creating %d VB neighbours... ", n_vb_neigh);

    vector<Neighbour>::iterator new_ngh = vb_neighbours_.begin();

    for(vector<Neighbour_both>::iterator ngh= neighbours_.begin(); ngh != neighbours_.end(); ++ngh ) {

        if ( ngh->type != VB_ES ) continue;

        ele_lower = element.find_id( ngh->eid[0]);
        ele_higher = element.find_id( ngh->eid[1] );
        edg = ele_higher->side( ngh->sid[ 1 ] )->edge();

        ASSERT(edg->n_sides == 1, "Edge with %d\n", edg->n_sides);
        ASSERT( ele_higher == edg->side(0)->element(),"Diff els.\n");
        new_ngh->reinit(  ele_lower, edg , ngh->sigma);


        //DBGMSG(" %d %d -> %d %d\n", ngh->eid[0], ngh->eid[1],
        //        new_ngh->element()->index(),
        //        new_ngh->side()->element()->index());

        ++new_ngh;
    }

    ASSERT( new_ngh == vb_neighbours_.end(), "Some VB neigbourings wasn't set.\n");


    xprintf( MsgVerb, "O.K.\n");
}


/**
 * Set Element->boundaries_ for all external sides. (temporary solution)
 */
void Mesh::create_external_boundary()
{
    // set to non zero all pointers including boundary connected to lower dim elements
    // these have only one side per edge
    Boundary empty_boundary;


    FOR_ELEMENTS(this, ele) {
        // is there any outer side
        bool outer=false;
        FOR_ELEMENT_SIDES(ele, si)
            if ( ele->side(si)->edge()->n_sides == 1) {
                outer=true;
                break;
            }
       if (outer) {
           // for elements on the boundary set boundaries_
           FOR_ELEMENT_SIDES(ele,si)
                if ( ele->side(si)->edge()->n_sides == 1)
                    ele->boundaries_[si] = &empty_boundary;
                else
                    ele->boundaries_[si] = NULL;

       } else {
           // can delete boundaries on internal elements !!
            delete ele->boundaries_;
            ele->boundaries_=NULL;
       }
    }

    int count=0;
    // pass through neighbours and set to NULL internal interfaces
    FOR_NEIGHBOURS(this,  ngh ) {
        SideIter s = ngh->side();
        if (s->element()->boundaries_ == NULL) continue;
        s->element()->boundaries_[ s->el_idx() ] = NULL;
    }

    // count remaining
    unsigned int n_boundaries=0;
    FOR_ELEMENTS(this, ele) {
        if (ele->boundaries_ == NULL) continue;
        FOR_ELEMENT_SIDES(ele, si)
            if (ele->boundaries_[si]) n_boundaries ++;
    }

    // fill boundaries
    BoundaryFullIter bcd(boundary);
    unsigned int ni;

    boundary.reserve(n_boundaries);
    bc_elements.reserve(n_boundaries);
    FOR_ELEMENTS(this, ele) {
         if (ele->boundaries_ == NULL) continue;
         FOR_ELEMENT_SIDES(ele, si)
             if (ele->boundaries_[si]) {
                 // add boundary object
                 bcd = boundary.add_item();

                 // fill boundary element
                 Element * bc_ele = &( bc_elements[bcd.index()] );
                 bc_ele->dim_ = ele->dim()-1;
                 bc_ele->node = new Node * [bc_ele->n_nodes()];
                 FOR_ELEMENT_NODES(bc_ele, ni) {
                     bc_ele->node[ni] = (Node *)ele->side(si)->node(ni);
                 }

                 // fill Boudary object
                 bcd->side = ele->side(si);
                 bcd->bc_element_ = bc_ele;
                 ele->boundaries_[si] = bcd;

             }
    }
}


//=============================================================================
//
//=============================================================================
void Mesh::element_to_neigh_vb()
{

    xprintf( MsgVerb, "   Element to neighbours of vb2 type... ")/*orig verb 5*/;

    FOR_ELEMENTS(this,ele) ele->n_neighs_vb =0;

    // count vb neighs per element
    FOR_NEIGHBOURS(this,  ngh )  ngh->element_->n_neighs_vb++;

    // Allocation of the array per element
    FOR_ELEMENTS(this,  ele )
        if( ele->n_neighs_vb > 0 ) {
            ele->neigh_vb = new struct Neighbour* [ele->n_neighs_vb];
            ele->n_neighs_vb=0;
        }

    // fill
    ElementIter ele;
    FOR_NEIGHBOURS(this,  ngh ) {
        ele = ngh->element();
        ele->neigh_vb[ ele->n_neighs_vb++ ] = &( *ngh );
    }

    xprintf( MsgVerb, "O.K.\n")/*orig verb 6*/;
}



void Mesh::setup_materials( MaterialDatabase &base)
{
    xprintf( MsgVerb, "   Element to material... ")/*orig verb 5*/;
    FOR_ELEMENTS(this, ele ) {
        ele->material=base.find_id(ele->region().id());
        INPUT_CHECK( ele->material != base.end(),
                "Reference to undefined material %d in element %d\n", ele->region().id(), ele.id() );
    }
    xprintf( MsgVerb, "O.K.\n")/*orig verb 6*/;
}


void Mesh::read_intersections() {

    using namespace boost;

    ElementFullIter master(element), slave(element);

    char tmp_line[LINE_SIZE];
    string file_name = in_record_.val<FilePath>("neighbouring");
    FILE *in = xfopen( file_name , "rt" );

    tokenizer<boost::char_separator<char> >::iterator tok;

    xprintf( Msg, "Reading intersections...")/*orig verb 2*/;
    skip_to(in, "$Intersections");
    xfgets(tmp_line, LINE_SIZE - 2, in);
    int n_intersect = atoi(xstrtok(tmp_line));
    INPUT_CHECK( n_intersect >= 0 ,"Negative number of neighbours!\n");

    intersections.reserve(n_intersect);

    for (int i = 0; i < n_intersect; i++) {
        xfgets(tmp_line, LINE_SIZE - 2, in);
        string line = tmp_line;
        tokenizer<boost::char_separator<char> > line_tokenizer(line, boost::char_separator<char>("\t \n"));

        tok = line_tokenizer.begin();

        try {
            ++tok; // skip id token
            int type = lexical_cast<int> (*tok);
            ++tok;
            int master_id = lexical_cast<int> (*tok);
            ++tok;
            int slave_id = lexical_cast<int> (*tok);
            ++tok;
            double sigma = lexical_cast<double> (*tok);
            ++tok;

            int n_intersect_points = lexical_cast<int> (*tok);
            ++tok;
            master = element.find_id(master_id);
            slave = element.find_id(slave_id);

            intersections.push_back(Intersection(n_intersect_points - 1, master, slave, tok));
        } catch (bad_lexical_cast &) {
            xprintf(UsrErr, "Wrong number format at line %d in file %s x%sx\n",i, file_name.c_str(),(*tok).c_str());
        }

    }

    xprintf( Msg, "O.K.\n")/*orig verb 2*/;

}


void Mesh::make_intersec_elements() {

     // calculate sizes and make allocations
     vector<int >sizes(n_elements(),0);
     for( vector<Intersection>::iterator i=intersections.begin(); i != intersections.end(); ++i )
     sizes[i->master_iter().index()]++;
     master_elements.resize(n_elements());
     for(int i=0;i<n_elements(); ++i ) master_elements[i].reserve(sizes[i]);

     // fill intersec_elements
     for( vector<Intersection>::iterator i=intersections.begin(); i != intersections.end(); ++i )
     master_elements[i->master_iter().index()].push_back( i-intersections.begin() );

}

ElementAccessor<3> Mesh::element_accessor(unsigned int idx, bool boundary) {
    return ElementAccessor<3>(this, idx, boundary);
}

/*
void Mesh::make_edge_list_from_neigh() {
    int edi;
    Mesh *mesh = this;
    struct Neighbour *ngh;

    xprintf( Msg, "Creating edges from neigbours... ");

    int n_edges = mesh->n_sides;
    FOR_NEIGHBOURS( ngh )
        if (ngh->type == BB_E || ngh->type == BB_EL)
            n_edges-=( ngh->n_elements - 1 );

    mesh->edge.resize(n_edges);

    xprintf( MsgVerb, " O.K. %d edges created.", mesh->n_edges());

    EdgeFullIter edg = mesh->edge.begin();
    n_edges=0;
    FOR_NEIGHBOURS( ngh )
        if (ngh->type == BB_E || ngh->type == BB_EL) {
            ngh->edge = edg;
            edg->neigh_bb = ngh;
            ++edg;
            n_edges++;
        }
    xprintf( MsgVerb, "O.K. %d\n");

}*/
//-----------------------------------------------------------------------------
// vim: set cindent:
