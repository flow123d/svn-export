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


#include "xio.h"
#include "output.h"
#include "mesh.h"

// TODO: remove in the future
#include "constantdb.h"

#include <string>

/**
 * \brief Constructor for OutputData storing names of output data and their
 * units.
 */
OutputData::OutputData(string data_name,
        string data_units,
        int *data_data,
        unsigned int size)
{
    name = new string(data_name); units = new string(data_units);
    data = (void*)&data_data;
    type = OUT_ARRAY_INT_SCA;
    comp_num = 1;
    num = size;
}

/**
 * \brief Constructor for OutputData storing names of output data and their
 * units.
 */
OutputData::OutputData(string data_name,
        string data_units,
        float *data_data,
        unsigned int size)
{
    name = new string(data_name); units = new string(data_units);
    data = (void*)&data_data;
    type = OUT_ARRAY_FLOAT_SCA;
    comp_num = 1;
    num = size;
}

/**
 * \brief Constructor for OutputData storing names of output data and their
 * units.
 */
OutputData::OutputData(string data_name,
        string data_units,
        double *data_data,
        unsigned int size)
{
    name = new string(data_name); units = new string(data_units);
    data = (void*)&data_data;
    type = OUT_ARRAY_DOUBLE_SCA;
    comp_num = 1;
    num = size;
}

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
    type = OUT_VECTOR_INT_SCA;
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
    type = OUT_VECTOR_INT_VEC;
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
    type = OUT_VECTOR_FLOAT_SCA;
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
    type = OUT_VECTOR_FLOAT_VEC;
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
    type = OUT_VECTOR_DOUBLE_SCA;
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
    type = OUT_VECTOR_DOUBLE_VEC;
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
    if(node_scalar != NULL) {
        delete node_scalar->scalars;
        delete node_scalar;
    }

    if(element_scalar != NULL) {
        delete element_scalar->scalars;
        delete element_scalar;
    }

    if(element_vector != NULL) {
        delete element_vector->vectors;
        delete element_vector;
    }
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
 * \brief Register data on nodes. This function will add reference on this
 * array of data to the Output object. Own data will be written to the
 * file, when write_data method will be called.
 */
template <typename _Data>
int Output::register_node_data(std::string name,
        std::string unit,
        _Data *data,
        uint size)
{
    if(mesh->node_vector.size() == size) {
        int found = 0;

        OutputData *out_data = new OutputData(name, unit, data, size);
        node_data->push_back(*out_data);

        return 1;
    } else {
        xprintf(Err, "Number of values: %d is not equal to number of nodes: %d\n", data.size(), mesh->node_vector.size());
        return 0;
    }
}

/**
 * \brief Register data on elements. This function will add reference on this
 * array of data to the Output object. Own data will be written to the
 * file, when write_data method will be called.
 */
template <typename _Data>
int Output::register_elem_data(std::string name,
        std::string unit,
        _Data *data,
        uint size)
{
    if(mesh->element.size() == size) {
        int found = 0;

        OutputData *out_data = new OutputData(name, unit, data, size);
        elem_data->push_back(*out_data);

        return 1;
    } else {
        xprintf(Err, "Number of values: %d is not equal to number of elements: %d\n", data.size(), mesh->element.size());
        return 0;
    }
}

/**
 * \brief Register data on nodes.
 *
 * This function will add reference on this data to the Output object. Own data
 * will be written to the file, when write_data() method will be called.
 *
 * \param[in]   name    The name of data
 * \param[in]   unit    The name of units
 * \param[in]   data    The reference on data
 *
 * \return This function return 1, when the length of data vector is the same as
 * number of nodes in mesh. When the the number is different, then this function
 * returns 0.
 */
template <typename _Data>
int Output::register_node_data(std::string name, std::string unit, std::vector<_Data> &data)
{
    if(mesh->node_vector.size() == data.size()) {
        OutputData *out_data = new OutputData(name, unit, data);
        node_data->push_back(*out_data);
        return 1;
    } else {
        xprintf(Err, "Number of values: %d is not equal to number of nodes: %d\n", data.size(), mesh->node_vector.size());
        return 0;
    }
}

/**
 * \brief Register data on elements. This function will add reference on the
 * data to the Output object. Own data will be writen to the file, when
 * write_data method will be called.
 * \param[in]   name    The name of data
 * \param[in]   unit    The name of units
 * \param[in]   data    The reference on data
 * \return This function return 1, when the length of data vector is the same as
 * number of elements in mesh. When the the number is different, then this
 * function returns 0.
 */
template <typename _Data>
int Output::register_elem_data(std::string name,
        std::string unit,
        std::vector<_Data> &data)
{
    if(mesh->element.size() == data.size()) {
        OutputData *out_data = new OutputData(name, unit, data);
        elem_data->push_back(*out_data);
        return 1;
    } else {
        xprintf(Err, "Number of values: %d is not equal to number of elements: %d\n", data.size(), mesh->element.size());
        return 0;
    }
}

/**
 * \brief NULL function for not yet supported formats
 * \param[in]   *output The pointer at output object.
 */
int write_null_data(Output *output)
{
    xprintf(Msg, "This file format is not yet supported\n");

    return 0;
}

/**
 * \brief This function call pointer at _write_data(Output). It writes
 * registered data to specified file format.
 * \return This function return result of pointer at output function.
 */
int Output::write_data(void)
{
    return _write_data(this);
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

    base_filename = new string(fname);

    mesh = _mesh;
    node_data = new OutputDataVec;
    elem_data = new OutputDataVec;

    // TODO: remove in the future
    format_type = ConstantDB::getInstance()->getInt("Pos_format_id");

    switch(format_type) {
    case VTK_SERIAL_ASCII:
    case VTK_PARALLEL_ASCII:
        _write_data = write_vtk_data;
        break;
    case POS_ASCII:
    case POS_BIN:
        _write_data = write_msh_data;
        break;
    default:
        _write_data = write_null_data;
        break;
    }

    base_filename = new string(fname);

    get_data_from_mesh();
}

/**
 * \brief Destructor of the Output object.
 */
Output::~Output()
{
    free_data_from_mesh();

    // Free all reference on node and element data
    if(node_data != NULL) {
        delete node_data;
    }

    if(elem_data != NULL) {
        delete elem_data;
    }

    if(base_filename != NULL) {
        delete base_filename;
    }

    if(base_file != NULL) {
        base_file->close();
        delete base_file;
    }

    xprintf(Msg, "O.K.\n");
}

/**
 * \brief This method get data from transport
 */
void OutputTime::free_data_from_transport(void)
{
    /* Delete vectors */
    delete element_vector->vectors;
    delete element_vector;
    /* Delete scalars */
    for(int subst_id=0; subst_id<elem_sca_count; subst_id++) {
        delete element_scalar[subst_id].scalars;
    }
    delete[] element_scalar;
}

/**
 * \brief This method get data from transport
 */
void OutputTime::get_data_from_transport(struct Transport *transport)
{
    Mesh *mesh = get_mesh();
    NodeIter node;
    element_scalar = new OutScalar[transport->n_substances];
    element_vector = new OutVector;

    xprintf(Msg, "Getting data from transport ...\n");

    elem_sca_count = transport->n_substances;

    /* Go through all substances and add them to vector of scalars */
    for(int subst_id=0; subst_id<elem_sca_count; subst_id++) {
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
        register_elem_data(element_scalar[subst_id].name, element_scalar[subst_id].unit, *element_scalar[subst_id].scalars);
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
    register_elem_data(element_vector->name, element_vector->unit, *element_vector->vectors);

    xprintf(Msg, "O.K.\n");
}

/**
 * \brief Register data on nodes. This function will add reference on this
 * array of data to the Output object. Own data will be written to the
 * file, when write_data method will be called.
 *
 * TODO: Test this method!
 */
template <typename _Data>
int OutputTime::register_node_data(std::string name,
        std::string unit,
        _Data *data,
        uint size)
{
    std::vector<OutputData> *node_data = get_node_data();
    Mesh *mesh = get_mesh();

    if(mesh->node_vector.size() == size) {
        int found = 0;

        for(std::vector<OutputData>::iterator od_iter = node_data->begin();
                od_iter != node_data->end();
                od_iter++)
        {
            if(*od_iter->name == name) {
                od_iter->data = (void*)&data;
                found = 1;
                break;
            }
        }

        if(found == 0) {
            OutputData *out_data = new OutputData(name, unit, data, size);
            node_data->push_back(*out_data);
        }

        return 1;
    } else {
        xprintf(Err, "Number of values: %d is not equal to number of nodes: %d\n", data.size(), mesh->node_vector.size());
        return 0;
    }
}

/**
 * \brief This function register data on elements.
 *
 * This function will add reference on this array of data to the Output object.
 * It is possible to call this function only once, when data are at the same
 * address during time. it is possible to call this function for each step, when
 * data are not at the same address, but name of the data has to be same.
 * Own data will be written to the file, when write_data() method will be called.
 *
 * \param[in] name  The name of data
 * \param[in] unit  The units of data
 * \param[in] *data The pointer at data (array of int, float or double)
 * \param[in] size  The size of array (number of values)
 *
 * \return This function returns 1, when data were registered. This function
 * returns 0, when it wasn't able to register data (number of values isn't
 * same as number of elements).
 *
 * TODO: Test this method!
 */
template <typename _Data>
int OutputTime::register_elem_data(std::string name,
        std::string unit,
        _Data *data,
        uint size)
{
    std::vector<OutputData> *elem_data = get_elem_data();
    Mesh *mesh = get_mesh();

    if(mesh->element.size() == size) {
        int found = 0;

        for(std::vector<OutputData>::iterator od_iter = elem_data->begin();
                od_iter != elem_data->end();
                od_iter++)
        {
            if(*od_iter->name == name) {
                od_iter->data = (void*)&data;
                found = 1;
                break;
            }
        }

        if(found == 0) {
            OutputData *out_data = new OutputData(name, unit, data, size);
            elem_data->push_back(*out_data);
        }

        return 1;
    } else {
        xprintf(Err, "Number of values: %d is not equal to number of elements: %d\n", data.size(), mesh->element.size());
        return 0;
    }
}

/**
 * \brief This function register data on nodes.
 *
 * This function will add reference on this array of data to the Output object.
 * It is possible to call this function only once, when data are at the same
 * address during time. it is possible to call this function for each step, when
 * data are not at the same address, but name of the data has to be same.
 * Own data will be written to the file, when write_data() method will be called.
 *
 * \param[in] name  The name of data
 * \param[in] unit  The units of data
 * \param[in] *data The pointer at data (array of int, float or double)
 * \param[in] size  The size of array (number of values)
 *
 * \return This function returns 1, when data were registered. This function
 * returns 0, when it wasn't able to register data (number of values isn't
 * same as number of nodes).
 *
 * TODO: Test this method!
 */
template <typename _Data>
int OutputTime::register_node_data(std::string name,
        std::string unit,
        std::vector<_Data> &data)
{
    std::vector<OutputData> *node_data = get_node_data();
    Mesh *mesh = get_mesh();

    if(mesh->node_vector.size() == data.size()) {
        int found = 0;

        for(std::vector<OutputData>::iterator od_iter = node_data->begin();
                od_iter != node_data->end();
                od_iter++)
        {
            if(*od_iter->name == name) {
                od_iter->data = (void*)&data;
                found = 1;
                break;
            }
        }

        if(found == 0) {
            OutputData *out_data = new OutputData(name, unit, data);
            node_data->push_back(*out_data);
        }

        return 1;
    } else {
        xprintf(Err, "Number of values: %d is not equal to number of nodes: %d\n", data.size(), mesh->node_vector.size());
        return 0;
    }
}

/**
 * \brief Register data on elements for the step. This function will add
 * reference on the data to the Output object. Own data will be written to the
 * file, when write_data method will be called. When the step is different
 * than current step, then old data will be deleted and current step will be
 * updated.
 */
template <typename _Data>
int OutputTime::register_elem_data(std::string name,
        std::string unit,
        std::vector<_Data> &data)
{
    std::vector<OutputData> *elem_data = get_elem_data();
    Mesh *mesh = get_mesh();

    if(mesh->element.size() == data.size()) {
        int found = 0;
        for(std::vector<OutputData>::iterator od_iter = elem_data->begin();
                od_iter != elem_data->end();
                od_iter++)
        {
            if(*od_iter->name == name) {
                od_iter->data = (void*)&data;
                found = 1;
                break;
            }
        }

        if(found == 0) {
            OutputData *out_data = new OutputData(name, unit, data);
            elem_data->push_back(*out_data);
        }
        return 1;
    } else {
        xprintf(Err, "Number of values: %d is not equal to number of elements: %d\n", data.size(), mesh->element.size());
        return 0;
    }

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
 * \brief This function call pointer at appropriate pointer at function,
 * that write data to file format
 */
int OutputTime::write_data(double time)
{
    return _write_data(this, time, current_step++);
}

/**
 * \brief Constructor of OutputTime object. It opens base file for writing.
 */
OutputTime::OutputTime(Mesh *_mesh, string fname)
{
    std::vector<OutputData> *node_data;
    std::vector<OutputData> *elem_data;
    Mesh *mesh = _mesh;
    ofstream *base_file;
    string *base_filename;
    int format_type;

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

    node_data = new OutputDataVec;
    elem_data = new OutputDataVec;

    // TODO: remove in the future
    format_type = ConstantDB::getInstance()->getInt("Pos_format_id");

    set_base_file(base_file);
    set_base_filename(base_filename);
    set_mesh(mesh);
    set_node_data(node_data);
    set_elem_data(elem_data);

    set_format_type(format_type);

    switch(format_type) {
    case VTK_SERIAL_ASCII:
    case VTK_PARALLEL_ASCII:
        _write_head = write_vtk_head;
        _write_data = write_vtk_time_data;
        _write_tail = write_vtk_tail;
        break;
    case POS_ASCII:
    case POS_BIN:
        _write_head = write_msh_head;
        _write_data = write_msh_time_data;
        _write_tail = write_msh_tail;
        break;
    default:
        _write_head = write_null_head;
        _write_data = write_null_time_data;
        _write_tail = write_null_tail;
        break;
    }

    _write_head(this);
}

/**
 * \brief Destructor of OutputTime. It doesn't do anything, because all
 * necessary destructors will be called in destructor of Output
 */
OutputTime::~OutputTime(void)
{
    _write_tail(this);
}
