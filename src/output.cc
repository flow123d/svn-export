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
 * @brief   The functions for all outputs. This file should be split according to the
 *          quantities to output. In this general file, there should remain only general output functions.
 *
 */

#include "constantdb.h"
#include "mesh/ini_constants_mesh.hh"

#include "system.hh"
#include "xio.h"
#include "output.h"
#include "math_fce.h"
#include "mesh.h"
#include "convert.h"

#include <mpi.h>
#include <petsc.h>

#include <stdarg.h>
#include <string>

/**
 * \brief This function get data from Mesh to temporary structure It creates
 * object Output and write all static data to the file
 */
void output( struct Problem *problem )
/* TODO: This is temporary solution. This should be removed in the future */
{
    Mesh* mesh = (Mesh*) ConstantDB::getInstance()->getObject(MESH::MAIN_INSTANCE);
    NodeIter node;

    // Do output only in first process
    int my_proc;
    MPI_Comm_rank(PETSC_COMM_WORLD,&my_proc);
    if (my_proc != 0) return;

    OutScalarsVector *node_scalar_arrays = new OutScalarsVector;
    OutScalarsVector *element_scalar_arrays = new OutScalarsVector;
    OutVectorsVector *element_vector_arrays = new OutVectorsVector;
    struct OutScalar node_scalar;
    struct OutScalar element_scalar;
    struct OutVector element_vector;

    const char* out_fname = OptGetFileName("Output", "Output_file", NULL);

    xprintf( Msg, "Writing flow output files: %s ... ", out_fname);

    ASSERT(!( problem == NULL ),"NULL as argument of function output_compute_mh()\n");
    if( OptGetBool("Output", "Write_output_file", "no") == false )
        return;

    /* Fill temporary vector of node scalars */
    node_scalar.scalars = new ScalarFloatVector;
    node_scalar.name = "node_scalars";
    node_scalar.unit = "";
    /* Generate vector for scalar data of nodes */
    node_scalar.scalars->reserve(mesh->node_vector.size());   // reserver memory for vector
    FOR_NODES( node ) {
        node_scalar.scalars->push_back(node->scalar);
    }

    /* Fill vectors of element scalars and vectors */
    element_scalar.scalars = new ScalarFloatVector;
    element_scalar.name = "element_scalars";
    element_scalar.unit = "";
    element_vector.vectors = new VectorFloatVector;
    element_vector.name = "element_vectors";
    element_vector.unit = "";
    /* Generate vectors for scalar and vector data of nodes */
    element_scalar.scalars->reserve(mesh->n_elements());
    element_vector.vectors->reserve(mesh->n_elements());
    FOR_ELEMENTS(ele) {
        /* Add scalar */
        element_scalar.scalars->push_back(ele->scalar);
        /* Add vector */
        vector<double> vec;
        vec.reserve(3);
        vec.push_back(ele->vector[0]);
        vec.push_back(ele->vector[1]);
        vec.push_back(ele->vector[2]);
        element_vector.vectors->push_back(vec);
    }

    Output *output = new Output(mesh, string(out_fname));

    output->register_node_data(node_scalar.name, node_scalar.unit, *node_scalar.scalars);
    output->register_elem_data(element_scalar.name, element_scalar.unit, *element_scalar.scalars);
    output->register_elem_data(element_vector.name, element_vector.unit, *element_vector.vectors);

    output->write_data(output);

    /* Delete temporary object */
    delete output;

    delete node_scalar.scalars;
    delete element_scalar.scalars;
    delete element_vector.vectors;

    xprintf( Msg, "O.K.\n");
}

/**
 * \brief Constructor for OutputData storing names of output data and their
 * units.
 */
OutputData::OutputData(string data_name,
        string data_units,
        std::vector<int> &data_data)
{
    name = data_name; units = data_units;
    data = (void*)&data_data;
    type = OUT_INT;
    comp_num = 1;
}

/**
 * \brief Constructor for OutputData storing names of output data and their
 * units.
 */
OutputData::OutputData(string data_name,
        string data_units,
        std::vector< vector<int> > &data_data)
{
    name = data_name; units = data_units;
    data = (void*)&data_data;
    type = OUT_INT_VEC;
    comp_num = 3;
}

/**
 * \brief Constructor for OutputData storing names of output data and their
 * units.
 */
OutputData::OutputData(string data_name,
        string data_units,
        std::vector<float> &data_data)
{
    name = data_name; units = data_units;
    data = (void*)&data_data;
    type = OUT_FLOAT;
    comp_num = 1;
}

/**
 * \brief Constructor for OutputData storing names of output data and their
 * units.
 */
OutputData::OutputData(string data_name,
        string data_units,
        std::vector< vector<float> > &data_data)
{
    name = data_name; units = data_units;
    data = (void*)&data_data;
    type = OUT_FLOAT_VEC;
    comp_num = 3;
}

/**
 * \brief Constructor for OutputData storing names of output data and their
 * units.
 */
OutputData::OutputData(string data_name,
        string data_units,
        std::vector<double> &data_data)
{
    name = data_name; units = data_units;
    data = (void*)&data_data;
    type = OUT_DOUBLE;
    comp_num = 1;
}

/**
 * \brief Constructor for OutputData storing names of output data and their
 * units.
 */
OutputData::OutputData(string data_name,
        string data_units,
        std::vector< vector<double> > &data_data)
{
    name = data_name; units = data_units;
    data = (void*)&data_data;
    type = OUT_DOUBLE_VEC;
    comp_num = 3;
}

/**
 * \brief This function
 */
void OutputData::writeData(ofstream &file, string item_sep, string vec_sep)
{
    switch(type) {
    case OUT_INT:
        for( std::vector<int>::iterator item = ((std::vector<int>*)data)->begin();
                item != ((std::vector<int>*)data)->end();
                item++) {
            file << *item << item_sep;
        }
        break;
    case OUT_INT_VEC:
        for( std::vector< vector<int> >::iterator vec = ((std::vector< vector<int> >*)data)->begin();
                vec != ((std::vector< vector<int> >*)data)->end();
                vec++) {
            for (std::vector<int>::iterator item = vec->begin();
                    item != vec->end();
                    item++) {
                file << *item << item_sep;
            }
            file << vec_sep;
        }
        break;
    case OUT_FLOAT:
        for( std::vector<float>::iterator item = ((std::vector<float>*)data)->begin();
                item != ((std::vector<float>*)data)->end();
                item++) {
            file << *item << item_sep;
        }
        break;
    case OUT_FLOAT_VEC:
        for( std::vector< vector<float> >::iterator vec = ((std::vector< vector<float> >*)data)->begin();
                vec != ((std::vector< vector<float> >*)data)->end();
                vec++) {
            for (std::vector<float>::iterator item = vec->begin();
                    item != vec->end();
                    item++) {
                file << *item << item_sep;
            }
            file << vec_sep;
        }
        break;
    case OUT_DOUBLE:
        for( std::vector<double>::iterator item = ((std::vector<double>*)data)->begin();
                item != ((std::vector<double>*)data)->end();
                item++) {
            file << *item << item_sep;
        }
        break;
    case OUT_DOUBLE_VEC:
        for( std::vector< vector<double> >::iterator vec = ((std::vector< vector<double> >*)data)->begin();
                vec != ((std::vector< vector<double> >*)data)->end();
                vec++) {
            for (std::vector<double>::iterator item = vec->begin();
                    item != vec->end();
                    item++) {
                file << *item << item_sep;
            }
            file << vec_sep;
        }
        break;
    }
}

/**
 * \brief Destructor for OutputData
 */
OutputData::~OutputData()
{
///
}

/**
 * \brief Register data on nodes. This function will add reference on this data
 * to the Output object. Own data will be writen to the file, when write_data
 * method will be called.
 */
template <typename _Data>
int Output::register_node_data(std::string name, std::string unit, std::vector<_Data> &data)
{
    OutputData *out_data = new OutputData(name, unit, data);
    node_data->push_back(*out_data);
    return 1;
}

void pokus(char *name, char *units, std::vector<int> &data)
{
    OutputData out_data(name, units, data);
}

/**
 * \brief Register data on elements. This function will add reference on the
 * data to the Output object. Own data will be writen to the file, when
 * write_data method will be called.
 */
template <typename _Data>
int Output::register_elem_data(std::string name, std::string unit, std::vector<_Data> &data)
{
    OutputData *out_data = new OutputData(name, unit, data);
    elem_data->push_back(*out_data);
    return 1;
}

/**
 * \brief Constructor of the Output object
 * \param[in]   *fname  The name of the output file
 */
Output::Output(Mesh *_mesh, string fname)
{
    if( OptGetBool("Output", "Write_output_file", "no") == false ) {
        base_filename = NULL;
        base_file = NULL;
        mesh = NULL;

        return;
    }

    base_file = new ofstream;

    base_file->open(fname.c_str());
    if(base_file->is_open() == false) {
        xprintf(Msg, "Could not write output to the file: %s\n", fname.c_str());
        base_filename = NULL;
        delete base_file;
        base_file = NULL;
        mesh = NULL;

        return;
    } else {
        xprintf(Msg, "Writing flow output file: %s ... ", fname.c_str());
    }

    mesh = _mesh;
    node_data = new OutputDataVec;
    elem_data = new OutputDataVec;

    format_type = ConstantDB::getInstance()->getInt("Pos_format_id");

    switch(format_type) {
    case VTK_SERIAL_ASCII:
    case VTK_PARALLEL_ASCII:
        write_data = write_vtk_data;
        break;
    default:
        write_data = NULL;
        break;
    }

    base_filename = new string(fname);
}

/**
 * \brief Destructor of the Output object.
 */
Output::~Output()
{
    // Free all reference on node and element data
    if(node_data != NULL) {
        delete node_data;
    }

    if(elem_data != NULL) {
        delete elem_data;
    }

    if(base_filename!=NULL) {
        delete base_filename;
    }

    if(base_file!=NULL) {
        base_file->close();
        delete base_file;
    }
}

/**
 * \brief This method write output to the file for specific time
 */
int OutputTime::write_data(double time, int step)
{
    //
}

/**
 * \brief Constructor of OutputTime object. It opens base file for writing.
 */
OutputTime::OutputTime(Mesh *_mesh, string fname)
{
    switch(get_format_type()) {
    case VTK_SERIAL_ASCII:
    case VTK_PARALLEL_ASCII:
        write_head = write_vtk_head;
        write_tail = write_vtk_tail;
        break;
    default:
        write_head = NULL;
        write_tail = NULL;
        break;
    }

}

OutputTime::~OutputTime(void)
{

}
