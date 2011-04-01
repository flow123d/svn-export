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

#include "ppfcs.h"  //FCS - DOPLNIT

// following deps. should probably be removed
#include "boundaries.h"
#include "problem.h"
#include "sources.h"
#include "concentrations.h"
#include "boundaries.h"
#include "transport_bcd.h"
#include "transport.h"
#include "postprocess.h"
#include "neighbours.h"

//=============================================================================
// GENERAL OUTPUT ROUTINE
//=============================================================================
// TODO: zrusit. Kazdy modul by mel mit vlastni volani vystupnich funkci
// TODO: prevod do pos (pokud ho vubec podporovat) by mel byt proveden pri destrukci vystupniho objektu
void output( struct Problem *problem )
{
    // TODO - tohle by melo asi prijit jinam
    int my_proc;
    MPI_Comm_rank(PETSC_COMM_WORLD,&my_proc);
    if (my_proc != 0) return;

    printf("****** 'new Output()' *****\n");

    Output *output = new Output(problem->out_fname_2);
    string node_name = "node_data_name";
    string node_unit = "node_data_unit";
    string elem_name = "elem_data_name";
    string elem_unit = "elem_data_unit";
    std::vector<int> node_scalar_data(6);
    std::vector< vector<int> > node_vector_data;
    std::vector<float> elem_scalar_data(6);
    std::vector< vector<float> > elem_vector_data;

    std::vector<int> vec;
    vec.push_back(0); vec.push_back(1); vec.push_back(2);
    node_vector_data.push_back(vec);
    node_vector_data.push_back(vec);
    node_vector_data.push_back(vec);

    std::vector<float> fvec;
    fvec.push_back(0.0); fvec.push_back(1.1); fvec.push_back(2.2);
    elem_vector_data.push_back(fvec);
    elem_vector_data.push_back(fvec);
    elem_vector_data.push_back(fvec);

    node_scalar_data[0] = 0;
    node_scalar_data[1] = 1;
    node_scalar_data[2] = 2;
    node_scalar_data[3] = 3;
    node_scalar_data[4] = 4;
    node_scalar_data[5] = 5;

    elem_scalar_data[0] = 0.1;
    elem_scalar_data[1] = 1.1;
    elem_scalar_data[2] = 2.1;
    elem_scalar_data[3] = 3.1;
    elem_scalar_data[4] = 4.1;
    elem_scalar_data[5] = 5.1;

    // Test of new Output system
    output->register_node_data(node_name, node_unit, node_scalar_data);
    output->register_node_data(node_name, node_unit, node_vector_data);
    output->register_elem_data(elem_name, elem_unit, elem_scalar_data);
    output->register_elem_data(elem_name, elem_unit, elem_vector_data);

    output->write_data();

    delete output;

    printf("****** 'delete Output()' *****\n");

    if ((ConstantDB::getInstance()->getInt("Goal") == COMPUTE_MH) &&
            (ConstantDB::getInstance()->getInt("Problem_type") == STEADY_SATURATED ||
             ConstantDB::getInstance()->getInt("Problem_type") == PROBLEM_DENSITY))
    {
        switch (ConstantDB::getInstance()->getInt("Out_file_type"))
        {
        case GMSH_STYLE:
            output_compute_mh( problem );
            break;
        case FLOW_DATA_FILE:
            output_flow_field_init(problem->out_fname_2);
            output_flow_field_in_time( problem, 0 );
            break;
        case BOTH_OUTPUT:
            // pridano -- upravy Ji. -- oba soubory najednou
            output_compute_mh( problem );
            output_flow_field_init(problem->out_fname_2);
            output_flow_field_in_time_2( problem, 0 );
            break;
        }
    }

    if (ConstantDB::getInstance()->getInt("Goal") == CONVERT_TO_POS)
        output_convert_to_pos(problem);
}
//=============================================================================
// OUTPUT ROUTINE FOR COMPUTING_MH
//=============================================================================
// TODO: prepinani typu vystupu by melo byt dano pri konstrukci; vystup presunout do proudeni
void output_compute_mh(struct Problem *problem)
{
    FILE *out;

    ASSERT(!( problem == NULL ),"NULL as argument of function output_compute_mh()\n");
    if( OptGetBool("Output", "Write_output_file", "no") == false )
        return;

    const char* out_fname = OptGetFileName("Output", "Output_file", NULL);
    xprintf( Msg, "Writing flow output files: %s ... ", out_fname);

/*
    // Temporary for debugging, necessary to find better more complicated solution for output of flow time steps
    if ( problem->type == PROBLEM_DENSITY)
        out = xfopen( out_fname, "at" );
*/

    switch(ConstantDB::getInstance()->getInt("Pos_format_id")){
    case POS_ASCII:
        out = xfopen( out_fname, "wt" );
        write_flow_ascii_data(out,problem);
        break;
    case POS_BIN:
        out = xfopen( out_fname, "wb" );
        write_flow_binary_data(out,problem);
        break;
    case VTK_SERIAL_ASCII:
        out = xfopen( out_fname, "wt");
        write_flow_vtk_serial(out);
        break;
    case VTK_PARALLEL_ASCII:
        xprintf(Msg, "VTK_PARALLEL_ASCII file format not supported yet\n.");
        break;
    }

    xfclose( out );

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
    name = data_name;
    units = data_units;
    data = (void*)&data_data;
    type = OUT_INT;
}

/**
 * \brief Constructor for OutputData storing names of output data and their
 * units.
 */
OutputData::OutputData(string data_name,
        string data_units,
        std::vector< vector<int> > &data_data)
{
    name = data_name;
    units = data_units;
    data = (void*)&data_data;
    type = OUT_INT_VEC;
}

/**
 * \brief Constructor for OutputData storing names of output data and their
 * units.
 */
OutputData::OutputData(string data_name,
        string data_units,
        std::vector<float> &data_data)
{
    name = data_name;
    units = data_units;
    data = (void*)&data_data;
    type = OUT_FLOAT;
}

/**
 * \brief Constructor for OutputData storing names of output data and their
 * units.
 */
OutputData::OutputData(string data_name,
        string data_units,
        std::vector< vector<float> > &data_data)
{
    name = data_name;
    units = data_units;
    data = (void*)&data_data;
    type = OUT_FLOAT_VEC;
}

void OutputData::writeData(void)
{
    switch(type) {
    case OUT_INT:
        for( std::vector<int>::iterator item = ((std::vector<int>*)data)->begin();
                item != ((std::vector<int>*)data)->end();
                item++) {
            cout << *item << " ";
        }
        break;
    case OUT_INT_VEC:
        for( std::vector< vector<int> >::iterator vec = ((std::vector< vector<int> >*)data)->begin();
                vec != ((std::vector< vector<int> >*)data)->end();
                vec++) {
            for (std::vector<int>::iterator item = vec->begin();
                    item != vec->end();
                    item++) {
                cout << *item << " ";
            }
            cout << "  ";
        }
        break;
    case OUT_FLOAT:
        for( std::vector<float>::iterator item = ((std::vector<float>*)data)->begin();
                item != ((std::vector<float>*)data)->end();
                item++) {
            cout << *item << " ";
        }
        break;
    case OUT_FLOAT_VEC:
        for( std::vector< vector<float> >::iterator vec = ((std::vector< vector<float> >*)data)->begin();
                vec != ((std::vector< vector<float> >*)data)->end();
                vec++) {
            for (std::vector<float>::iterator item = vec->begin();
                    item != vec->end();
                    item++) {
                cout << *item << " ";
            }
            cout << "  ";
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
 * \brief This function writes geometry and topology of Mesh to the output file
 * as well all data on nodes and elements.
 */
int Output::write_data(void)
{
    /* Write data on nodes */
    if(node_data != NULL) {
        for(OutputDataVec::iterator dta = node_data->begin();
                dta != node_data->end(); dta++) {
            cout << "Node name: " << dta->getName() << ", units: " << dta->getUnits() << endl;
            dta->writeData();
            cout << endl;
        }
    }

    /* Write data on elements */
    if(elem_data != NULL) {
        for(OutputDataVec::iterator dta = elem_data->begin();
                dta != elem_data->end(); dta++) {
            cout << "Elem name: " << dta->getName() << ", units: " << dta->getUnits() << endl;
            dta->writeData();
            cout << endl;
        }
    }

    return 1;
}

/**
 * \brief Constructor of the Output object
 * \param[in]   *fname  The name of the output file
 */
Output::Output(char *fname)
{
    if( OptGetBool("Output", "Write_output_file", "no") == false ) {
        filename = NULL;
        file = NULL;

        return;
    }

    node_data = new OutputDataVec;
    elem_data = new OutputDataVec;

    format_type = ConstantDB::getInstance()->getInt("Pos_format_id");

    filename = (char *)xmalloc(strlen(fname)+1);
    strcpy(filename, fname);

    file = xfopen(filename, "wt");
    if(file == NULL) {
        xprintf( Msg, "Could not write output to the file: %s\n", fname);
    } else {
        xprintf( Msg, "Writing flow output file: %s ...\n", fname);
    }

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

    if(file != NULL) xfclose(file);
    if(filename != NULL) xfree(filename);
}

//-----------------------------------------------------------------------------
// vim: set cindent:
