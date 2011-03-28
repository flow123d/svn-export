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

        if ((ConstantDB::getInstance()->getInt("Goal") == COMPUTE_MH) &&
                (ConstantDB::getInstance()->getInt("Problem_type") == STEADY_SATURATED || ConstantDB::getInstance()->getInt("Problem_type") == PROBLEM_DENSITY)){
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


// FTRANS           if (problem->ftrans_out == true && problem->type == STEADY_SATURATED)
//                	output_veloc_ftrans(problem, 0);


                if (OptGetBool("Transport", "Transport_on", "no") == true)
                        {
                //        output_transport_convert(problem);
                        //if (problem->type == PROBLEM_DENSITY)
                        //	output_convert(problem);    // time variable flow field
                        }


        }
        if (ConstantDB::getInstance()->getInt("Goal") == CONVERT_TO_POS)
                output_convert_to_pos(problem);

        //flow_cs(problem);

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
template <typename _Data>
OutputData<_Data>::OutputData(char *data_name,
        char *data_unit,
        vector<_Data> *data_data)
{
    name = xstrcpy(data_name);
    unit = xstrcpy(data_unit);
    data = data_data;
}

/**
 * \brief Destructor for OutputData
 */
template <typename _Data>
OutputData<_Data>::~OutputData()
{
    xfree(name);
    xfree(unit);
}

/**
 * \brief Register data on elements. This function will add reference on the
 * data to the Output object. Own data will be writen to the file, when
 * write_data method will be called.
 */
template <typename OutputData>
int Output::register_elem_data(char *name, char *unit, vector<OutputData> *data)
{
    OutputData *out_data = new OutputData(name, unit, data);
    elem_data->push_back(out_data);
    return 1;
}

/**
 * \brief Register data on nodes. This function will add reference on this data
 * to the Output object. Own data will be writen to the file, when write_data
 * method will be called.
 */
template <typename OutputData>
int Output::register_node_data(char *name, char *unit, vector<OutputData> *data)
{
    OutputData *out_data = new OutputData(name, unit, data);
    node_data->push_back(out_data);
    return 1;
}

/**
 * \brief This function writes geometry and topology of Mesh to the output file
 * as well all data on nodes and elements.
 */
int Output::write_data(void)
{
    if(node_data != NULL) {
        for(BaseOutputDataVec::iterator dta = node_data->begin();
                dta != node_data->end(); dta++) {
            xprintf(MsgDbg, "Node name: %s, units: %s\n", dta->name, dta->unit);
        }
    }
    if(elem_data != NULL) {
        for(BaseOutputDataVec::iterator dta = elem_data->begin();
                dta != elem_data->end(); dta++) {
            xprintf(MsgDbg, "Elem name: %s, units: %s\n", dta->name, dta->unit);
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

    node_data = new BaseOutputDataVec;
    elem_data = new BaseOutputDataVec;

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
