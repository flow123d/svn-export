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
 * @brief   The functions for outputs to GMSH files.
 *
 */

#include "output.h"
#include "xio.h"
#include "mesh.h"
#include "mesh/ini_constants_mesh.hh"
#include "constantdb.h"
#include "transport.h"
#include "problem.h"

/**
 * TODO: remove
 */
void output_flow_field_init(char *fname)
{
    FILE *out;

    if( OptGetBool("Output", "Write_output_file", "no") == false )
        return;

    xprintf( Msg, "%s: Writing output files... %s ", __func__, fname);

    out = xfopen( fname, "wt" );
    xfprintf( out, "$DataFormat\n" );
    xfprintf( out, "1.0 0 %d\n", sizeof( double ) );
    xfprintf( out, "$EndDataFormat\n" );
    xfclose( out );

    xprintf( Msg, "O.K.\n");
}

/**
 * TODO: remove
 */
void output_flow_field_in_time(struct Problem *problem, double time)
{
    Mesh* mesh = (Mesh*) ConstantDB::getInstance()->getObject(MESH::MAIN_INSTANCE);

    int i,cit;
    ElementIter ele;
    FILE *out;
    char dbl_fmt[ 16 ];

    ASSERT(!( problem == NULL ),"NULL as argument of function output_flow_field_in_time()\n");
    if( OptGetBool("Output", "Write_output_file", "no") == false )
        return;

    xprintf( Msg, "%s: Writing output file %s ... ", __func__, problem->out_fname_2);

    out = xfopen( problem->out_fname_2, "at" );
    sprintf( dbl_fmt, "%%.%dg ", ConstantDB::getInstance()->getInt("Out_digit"));
    xfprintf( out, "$FlowField\n" );
    xfprintf( out, "T = ");
    xfprintf( out, dbl_fmt, time);
    xfprintf( out, "\n%d\n", mesh->n_elements() );
    cit = 0;

    FOR_ELEMENTS( ele ) {
        xfprintf( out, "%d ", cit);
        xfprintf( out, "%d ", ele.id());
        xfprintf( out, dbl_fmt, ele->scalar);
        xfprintf( out, " %d ", ele->n_sides);
        for (i = 0; i < ele->n_sides; i++)
            xfprintf( out, dbl_fmt, ele->side[i]->scalar);
        xfprintf( out, "\t");
        for (i = 0; i < ele->n_sides; i++)
            xfprintf( out, dbl_fmt, ele->side[i]->flux);
        xfprintf( out, "\t");
        xfprintf( out, "%d ", ele->n_neighs_vv);
        for (i = 0; i < ele->n_neighs_vv; i++)
            xfprintf( out, "%d ", ele->neigh_vv[i]->id);
        xfprintf( out, "\n" );
        cit ++;
    }
    xfprintf( out, "$EndFlowField\n\n" );
    xfclose( out );

    xprintf( Msg, "O.K.\n");
}

/**
 * \brief This function write header of GMSH (.msh) file format
 * \param[in]   *output     The pointer at output object
 */
void write_msh_header(Output *output)
{
    // Write simple header
    output->get_base_file() << "$MeshFormat" << endl;
    output->get_base_file() << "2" << " 0 " << sizeof(double) << endl;
    output->get_base_file() << "$EndMeshFormat" << endl;
}

/**
 * \brief This function writes geometry (position of nodes) to GMSH (.msh) file
 * format
* \param[in]   *output     The pointer at output object
 */
void write_msh_geometry(Output *output)
{
    Mesh* mesh = output->get_mesh();
    NodeIter nod;
    int i;

    // Write information about nodes
    output->get_base_file() << "$Nodes" << endl;
    output->get_base_file() <<  mesh->node_vector.size() << endl;
    FOR_NODES( nod ) {
        output->get_base_file() << nod->id << " " << nod->getX() << " " << nod->getY() << " " << nod->getZ() << endl;
    }
    output->get_base_file() << "$EndNodes" << endl;
}

/**
 * \brief This function writes topology (connection of nodes) to the GMSH (.msh)
 * file format
 * \param[in]   *output     The pointer at output object
 */
void write_msh_topology(Output *output)
{
    Mesh* mesh = output->get_mesh();
    ElementIter elm;
    int i;

    // Write information about elements
    output->get_base_file() << "$Elements" << endl;
    output->get_base_file() << mesh->n_elements() << endl;
    FOR_ELEMENTS(elm) {
        // element_id element_type 3_other_tags material region partition
        output->get_base_file() << elm.id() << " " << elm->type << " 3 " << elm->mid << " " << elm->rid << " " << elm->pid;
        FOR_ELEMENT_NODES(elm,i)
            output->get_base_file() << " " << elm->node[i]->id;
        output->get_base_file() << endl;
    }
    output->get_base_file() << "$EndElements" << endl;
}

/**
 * \brief This function writes ascii data to GMSH (.msh) output file.
 * \param[in]   *output     The pointer at Output object
 * \param[in]   *out_data   The pointer at structure storing pointer at own data.
 */
void write_msh_ascii_data(Output *output, OutputData *out_data)
{
    ofstream &file = output->get_base_file();
    int id = 1;

    switch(out_data->type) {
    case OUT_INT:
        for( std::vector<int>::iterator item = ((std::vector<int>*)out_data->data)->begin();
                item != ((std::vector<int>*)out_data->data)->end();
                item++, id++) {
            file << id << " " << *item << endl;
        }
        break;
    case OUT_INT_VEC:
        for( std::vector< vector<int> >::iterator vec = ((std::vector< vector<int> >*)out_data->data)->begin();
                vec != ((std::vector< vector<int> >*)out_data->data)->end();
                vec++, id++)
        {
            file << id << " ";
            for (std::vector<int>::iterator item = vec->begin();
                    item != vec->end();
                    item++) {
                file << *item << " ";
            }
            file << endl;
        }
        break;
    case OUT_FLOAT:
        for( std::vector<float>::iterator item = ((std::vector<float>*)out_data->data)->begin();
                item != ((std::vector<float>*)out_data->data)->end();
                item++, id++) {
            file << id << " " << *item << endl;
        }
        break;
    case OUT_FLOAT_VEC:
        for( std::vector< vector<float> >::iterator vec = ((std::vector< vector<float> >*)out_data->data)->begin();
                vec != ((std::vector< vector<float> >*)out_data->data)->end();
                vec++)
        {
            file << id << " ";
            for (std::vector<float>::iterator item = vec->begin();
                    item != vec->end();
                    item++) {
                file << *item << " ";
            }
            file << endl;
        }
        break;
    case OUT_DOUBLE:
        for( std::vector<double>::iterator item = ((std::vector<double>*)out_data->data)->begin();
                item != ((std::vector<double>*)out_data->data)->end();
                item++, id++) {
            file << id << " " << *item << endl;
        }
        break;
    case OUT_DOUBLE_VEC:
        for( std::vector< vector<double> >::iterator vec = ((std::vector< vector<double> >*)out_data->data)->begin();
                vec != ((std::vector< vector<double> >*)out_data->data)->end();
                vec++, id++)
        {
            file << id << " ";
            for (std::vector<double>::iterator item = vec->begin();
                    item != vec->end();
                    item++) {
                file << *item << " ";
            }
            file << endl;
        }
        break;
    }
}

/**
 * \brief This function write all data on nodes to output file. This function
 * is used for static and dynamic data
 * \param[in]   *output     The pointer at output object
 * \param[in]   time        The time from start
 * \param[in]   step        The number of steps from start
 */
void write_msh_node_data(Output *output, double time, int step)
{
    std::vector<OutputData> *node_data = output->get_node_data();

    if(node_data != NULL) {
        for(OutputDataVec::iterator dta = node_data->begin();
                    dta != node_data->end();
                    dta++)
        {
            output->get_base_file() << "$NodeData" << endl;

            output->get_base_file() << "1" << endl;     // one string tag
            output->get_base_file() << "\"" << *dta->getName() << "_" << *dta->getUnits() <<"\"" << endl;

            output->get_base_file() << "1" << endl;     // one real tag
            output->get_base_file() << time << endl;    // first real tag = time

            output->get_base_file() << "3" << endl;     // 3 integer tags
            output->get_base_file() << step << endl;     // step number (start = 0)
            output->get_base_file() << dta->getCompNum() << endl;   // number of components
            output->get_base_file() << dta->getValueNum() << endl;  // number of values

            write_msh_ascii_data(output, &(*dta));

            output->get_base_file() << "$EndNodeData" << endl;
        }
    }
}

/**
 * \brief This function write all data on elements to output file. This
 * function is used for static and dynamic data
 * \param[in]   *output     The pointer at output object
 * \param[in]   time        The time from start
 * \param[in]   step        The number of steps from start
 */
void write_msh_elem_data(Output *output, double time, int step)
{
    std::vector<OutputData> *elem_data = output->get_elem_data();

    if(elem_data != NULL) {
        for(OutputDataVec::iterator dta = elem_data->begin();
                    dta != elem_data->end();
                    dta++)
        {
            output->get_base_file() << "$ElementData" << endl;

            output->get_base_file() << "1" << endl;     // one string tag
            output->get_base_file() << "\"" << *dta->getName() << "_" << *dta->getUnits() <<"\"" << endl;

            output->get_base_file() << "1" << endl;     // one real tag
            output->get_base_file() << time << endl;    // first real tag = time

            output->get_base_file() << "3" << endl;     // 3 integer tags
            output->get_base_file() << step << endl;     // step number (start = 0)
            output->get_base_file() << dta->getCompNum() << endl;   // number of components
            output->get_base_file() << dta->getValueNum() << endl;  // number of values

            write_msh_ascii_data(output, &(*dta));

            output->get_base_file() << "$EndElementData" << endl;
        }
    }
}

/**
 * \brief Write head of .msh file format
 */
int write_msh_data(Output *output)
{
    xprintf( Msg, "%s: Writing output file %s ... ", __func__, output->get_base_filename().c_str());

    write_msh_header(output);

    write_msh_geometry(output);

    write_msh_topology(output);

    write_msh_node_data(output, 0.0, 0);

    write_msh_elem_data(output, 0.0, 0);

    xprintf( Msg, "O.K.\n");

    return 1;
}

/**
 * \brief Write head of .msh file format
 */
int write_msh_head(OutputTime *output)
{
    xprintf( Msg, "%s: Writing output file %s ... ", __func__, output->get_base_filename().c_str());

    write_msh_header(output);

    write_msh_geometry(output);

    write_msh_topology(output);

    xprintf( Msg, "O.K.\n");

    return 1;
}


/**
 * \brief Write head of .msh file format
 */
int write_msh_time_data(OutputTime *output, double time, int step)
{
    xprintf( Msg, "%s: Writing output file %s ... ", __func__, output->get_base_filename().c_str());

    write_msh_node_data(output, time, step);

    write_msh_elem_data(output, time, step);

    xprintf( Msg, "O.K.\n");

    return 1;
}

/**
 * \brief Write tail of .msh file format
 */
int write_msh_tail(OutputTime *output)
{
    // It is stupid file format. It doesn't write anything special at the end of
    // the file

    return 1;
}
