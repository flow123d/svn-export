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

#include <assert.h>
#include <stdarg.h>
#include <string>

/**
 * \brief Constructor for OutputData storing names of output data and their
 * units.
 */
OutputData::OutputData(string data_name,
        string data_units,
        std::vector<int> &data_data)
{
    name = new string(data_name); units = new string(data_units);
    data = (void*)&data_data;
    type = OUT_INT;
    comp_num = 1;
    num = data_data.size();
}

/**
 * \brief Constructor for OutputData storing names of output data and their
 * units.
 */
OutputData::OutputData(string data_name,
        string data_units,
        std::vector< vector<int> > &data_data)
{
    name = new string(data_name); units = new string(data_units);
    data = (void*)&data_data;
    type = OUT_INT_VEC;
    comp_num = 3;
    num = data_data.size();
}

/**
 * \brief Constructor for OutputData storing names of output data and their
 * units.
 */
OutputData::OutputData(string data_name,
        string data_units,
        std::vector<float> &data_data)
{
    name = new string(data_name); units = new string(data_units);
    data = (void*)&data_data;
    type = OUT_FLOAT;
    comp_num = 1;
    num = data_data.size();
}

/**
 * \brief Constructor for OutputData storing names of output data and their
 * units.
 */
OutputData::OutputData(string data_name,
        string data_units,
        std::vector< vector<float> > &data_data)
{
    name = new string(data_name); units = new string(data_units);
    data = (void*)&data_data;
    type = OUT_FLOAT_VEC;
    comp_num = 3;
    num = data_data.size();
}

/**
 * \brief Constructor for OutputData storing names of output data and their
 * units.
 */
OutputData::OutputData(string data_name,
        string data_units,
        std::vector<double> &data_data)
{
    name = new string(data_name); units = new string(data_units);
    data = (void*)&data_data;
    type = OUT_DOUBLE;
    comp_num = 1;
    num = data_data.size();
}

/**
 * \brief Constructor for OutputData storing names of output data and their
 * units.
 */
OutputData::OutputData(string data_name,
        string data_units,
        std::vector< vector<double> > &data_data)
{
    name = new string(data_name); units = new string(data_units);
    data = (void*)&data_data;
    type = OUT_DOUBLE_VEC;
    comp_num = 3;
    num = data_data.size();
}

/**
 * \brief Destructor for OutputData
 */
OutputData::~OutputData()
{
#if 0
    assert(0);

    if(name != NULL) {
        cout << "Deleting: " << *name << endl;
        delete name;
    }
    if(units != NULL) {
        cout << "Deleting: " << *units << endl;
        delete units;
    }
#endif
}

/**
 * \brief This function free data from Mesh
 */
void Output::free_data_from_mesh(void)
{
    delete node_scalar->scalars;
    delete node_scalar;
    delete element_scalar->scalars;
    delete element_scalar;
    delete element_vector->vectors;
    delete element_vector;
}

/**
 * \brief This function gets data from mesh and save them in Output
 */
void Output::get_data_from_mesh(void)
{
    NodeIter node;
    ElementIter ele;

    node_scalar = new OutScalar;
    element_scalar = new OutScalar;
    element_vector = new OutVector;

    /* Fill temporary vector of node scalars */
    node_scalar->scalars = new ScalarFloatVector;
    node_scalar->name = "node_scalars";
    node_scalar->unit = "";
    /* Generate vector for scalar data of nodes */
    node_scalar->scalars->reserve(mesh->node_vector.size());   // reserver memory for vector
    FOR_NODES( node ) {
        node_scalar->scalars->push_back(node->scalar);
    }

    /* Fill vectors of element scalars and vectors */
    element_scalar->scalars = new ScalarFloatVector;
    element_scalar->name = "element_scalars";
    element_scalar->unit = "";
    element_vector->vectors = new VectorFloatVector;
    element_vector->name = "element_vectors";
    element_vector->unit = "";
    /* Generate vectors for scalar and vector data of nodes */
    element_scalar->scalars->reserve(mesh->n_elements());
    element_vector->vectors->reserve(mesh->n_elements());
    FOR_ELEMENTS(ele) {
        /* Add scalar */
        element_scalar->scalars->push_back(ele->scalar);
        /* Add vector */
        vector<double> vec;
        vec.reserve(3);
        vec.push_back(ele->vector[0]);
        vec.push_back(ele->vector[1]);
        vec.push_back(ele->vector[2]);
        element_vector->vectors->push_back(vec);
    }

    register_node_data(node_scalar->name, node_scalar->unit, *node_scalar->scalars);
    register_elem_data(element_scalar->name, element_scalar->unit, *element_scalar->scalars);
    register_elem_data(element_vector->name, element_vector->unit, *element_vector->vectors);
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
 * \brief NULL function for not yet supported formats
 */
int write_null_data(Output *output)
{
    xprintf(Msg, "This file format is not yet supported\n");

    return 0;
}

/**
 * \brief Constructor of the Output object
 * \param[in]   *fname  The name of the output file
 */
Output::Output(Mesh *_mesh, string fname)
{
    xprintf(Msg, "Output(), file: %s\n", fname.c_str());

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

    base_filename = new string(fname);

    mesh = _mesh;
    node_data = new OutputDataVec;
    elem_data = new OutputDataVec;

    format_type = ConstantDB::getInstance()->getInt("Pos_format_id");

    switch(format_type) {
    case VTK_SERIAL_ASCII:
    case VTK_PARALLEL_ASCII:
        write_data = write_vtk_data;
        break;
    case POS_ASCII:
        write_data = write_msh_data;
        break;
    default:
        write_data = write_null_data;
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
        printf("node_data->size: %d\n", node_data->size());
        delete node_data;
    }

    if(elem_data != NULL) {
        printf("elem_data->size: %d\n", elem_data->size());
        delete elem_data;
    }

    if(base_filename != NULL) {
        delete base_filename;
    }

    if(base_file != NULL) {
        base_file->close();
        delete base_file;
    }
}

/**
 * \brief This method get data from transport
 */
void OutputTime::free_data_from_transport(struct Transport *transport)
{
    /* Delete vectors */
    delete element_vector->vectors;
    delete element_vector;
    /* Delete scalars */
    for(int subst_id=0; subst_id<transport->n_substances; subst_id++) {
        delete element_scalar[subst_id].scalars;
    }
    delete[] element_scalar;
}

/**
 * \brief This method get data from transport
 */
void OutputTime::get_data_from_transport(struct Transport *transport, int step)
{
    NodeIter node;
    element_scalar = new OutScalar[transport->n_substances];
    element_vector = new OutVector;

    xprintf(Msg, "Getting data from transport ...\n");

    /* Go through all substances and add them to vector of scalars */
    for(int subst_id=0; subst_id<transport->n_substances; subst_id++) {
        element_scalar[subst_id].scalars = new ScalarFloatVector;
        /* Set up names */
        element_scalar[subst_id].name = transport->substance_name[subst_id];
        element_scalar[subst_id].unit = ""; // TODO: get units from somewhere
        /* Reserve memory for vectors */
        element_scalar[subst_id].scalars->reserve(mesh->n_elements());
        /* Add scalar data to vector of scalars */
        for(int el=0; el<mesh->n_elements(); el++) {
            element_scalar[subst_id].scalars->push_back(transport->out_conc[subst_id][MOBILE][el]);
        }
        register_elem_data(element_scalar[subst_id].name, element_scalar[subst_id].unit, step, *element_scalar[subst_id].scalars);
    }

    /* Add vectors to vector */
    element_vector->vectors = new VectorFloatVector;
    /* Reserve memory for vector */
    element_vector->vectors->reserve(mesh->n_elements());
    /* Set up name */
    element_vector->name = "transport_vector";
    /* Copy data */
    FOR_ELEMENTS(ele) {
        /* Add vector data do vector of vectors */
        vector<double> vec;
        vec.reserve(3);
        vec.push_back(ele->vector[0]);
        vec.push_back(ele->vector[1]);
        vec.push_back(ele->vector[2]);
        element_vector->vectors->push_back(vec);
    }
    register_elem_data(element_vector->name, element_vector->unit, step, *element_vector->vectors);

    xprintf(Msg, "O.K.\n");
}

/**
 * \brief Register data on nodes. This function will add reference on this data
 * to the Output object. Own data will be writen to the file, when write_data
 * method will be called.
 */
template <typename _Data>
int OutputTime::register_node_data(std::string name, std::string unit, int step, std::vector<_Data> &data)
{
    if (current_step != step && current_step != -1) {
        delete node_data;
        node_data = new OutputDataVec;
        current_step = step;
    }

    OutputData *out_data = new OutputData(name, unit, data);
    node_data->push_back(*out_data);
    return 1;
}

/**
 * \brief Register data on elements for the step. This function will add
 * reference on the data to the Output object. Own data will be written to the
 * file, when write_data method will be called. When the step is different
 * than current step, then old data will be deleted and current step will be
 * updated.
 */
template <typename _Data>
int OutputTime::register_elem_data(std::string name, std::string unit, int step, std::vector<_Data> &data)
{
    if (current_step != step && current_step != -1) {
        delete elem_data;
        elem_data = new OutputDataVec;
        current_step = step;
    }

    OutputData *out_data = new OutputData(name, unit, data);
    elem_data->push_back(*out_data);
    return 1;
}

/**
 * \brief This is fake output function for not supported formats. It writes
 * only warning to the stdout and log file.
 */
int write_null_head(OutputTime *output)
{
    xprintf(Msg, "This file format: %d is not yet supported\n", output->get_format_type());

    return 0;
}

/**
 * \brief This is fake output function for not supported formats. It writes
 * only warning to the stdout and log file.
 */
int write_null_time_data(OutputTime *output, double time, int step)
{
    xprintf(Msg, "This file format: %d is not yet supported\n", output->get_format_type());

    return 0;
}

/**
 * \brief This is fake output function for not supported formats. It writes
 * only warning to the stdout and log file.
 */
int write_null_tail(OutputTime *output)
{
    xprintf(Msg, "This file format: %d is not yet supported\n", output->get_format_type());

    return 0;
}

/**
 * \brief Constructor of OutputTime object. It opens base file for writing.
 */
OutputTime::OutputTime(Mesh *_mesh, string fname)
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

    base_filename = new string(fname);

    mesh = _mesh;
    node_data = new OutputDataVec;
    elem_data = new OutputDataVec;

    format_type = ConstantDB::getInstance()->getInt("Pos_format_id");

    switch(format_type) {
    case VTK_SERIAL_ASCII:
    case VTK_PARALLEL_ASCII:
        write_head = write_vtk_head;
        write_data = write_vtk_time_data;
        write_tail = write_vtk_tail;
        break;
    case POS_ASCII:
        write_head = write_msh_head;
        write_data = write_msh_time_data;
        write_tail = write_msh_tail;
        break;
    default:
        write_head = write_null_head;
        write_data = write_null_time_data;
        write_tail = write_null_tail;
        break;
    }

}

/**
 * \brief Destructor of OutputTime. It doesn't do anything, because all
 * necessary destructors will be called in destructor of Output
 */
OutputTime::~OutputTime(void)
{
    //
}
