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
 * @brief   The functions for outputs to VTK files.
 *
 */

#include <limits.h>

#include "output.h"
#include "xio.h"
#include "mesh.h"
#include "mesh/ini_constants_mesh.hh"
#include "constantdb.h"
#include "transport.h"

/**
 * \brief Write header of VTK file (.vtu)
 * \param[in]	out	The output file
 */
static void write_flow_vtk_header(Output *output)
{
    output->get_file() << "<?xml version=\"1.0\"?>" << endl;
    // TODO: test endianess of platform (this is important, when raw data are
    // saved to the VTK file)
    output->get_file() << "<VTKFile type=\"UnstructuredGrid\" version=\"0.1\" byte_order=\"LittleEndian\">" << endl;
    output->get_file() << "<UnstructuredGrid>" << endl;
}

/**
 * \brief Write geometry (position of nodes) to the VTK file (.vtu)
 * \param[in] *out The output file
 */
static void write_flow_vtk_geometry(Output *output)
{
    Mesh *mesh = output->get_mesh();

    NodeIter node;
    //char dbl_fmt[16];
    int tmp;

    /* TODO: digit precision */

    /* Write Points begin*/
    output->get_file() << "<Points>" << endl;
    /* Write DataArray begin */
    output->get_file() << "<DataArray type=\"Float64\" NumberOfComponents=\"3\" format=\"ascii\">" << endl;
    /* Write own coordinates */
    tmp = 0;
    FOR_NODES( node ) {
        node->aux = tmp;   /* store index in the auxiliary variable */

        output->get_file() << node->getX() << " ";
        output->get_file() << node->getY() << " ";
        output->get_file() << node->getZ() << " ";

        tmp++;
    }
    /* Write DataArray end */
    output->get_file() << endl << "</DataArray>" << endl;
    /* Write Points end*/
    output->get_file() << "</Points>" << endl;
}

/**
 * \brief Write topology (connection of nodes) to the VTK file (.vtu)
 * \param[in] *out The output file
 */
static void write_flow_vtk_topology(Output *output)
{
    //Mesh* mesh = (Mesh*) ConstantDB::getInstance()->getObject(MESH::MAIN_INSTANCE);
    Mesh *mesh = output->get_mesh();

    Node* node;
    ElementIter ele;
    int li, tmp;

    /* TODO: digit precision */

    /* Write Cells begin*/
    output->get_file() << "<Cells>" << endl;
    /* Write DataArray begin */
    output->get_file() << "<DataArray type=\"Int32\" Name=\"connectivity\" format=\"ascii\">" << endl;
    /* Write own coordinates */
    FOR_ELEMENTS(ele) {
        FOR_ELEMENT_NODES(ele, li) {
            node = ele->node[li];
            output->get_file() << node->aux << " ";   /* Write connectivity */
        }
    }
    /* Write DataArray end */
    output->get_file() << endl << "</DataArray>" << endl;

    /* Write DataArray begin */
    output->get_file() << "<DataArray type=\"Int32\" Name=\"offsets\" format=\"ascii\">" << endl;
    /* Write number of nodes for each element */
    tmp = 0;
    FOR_ELEMENTS(ele) {
        switch(ele->type) {
        case LINE:
            tmp += VTK_LINE_SIZE;
            break;
        case TRIANGLE:
            tmp += VTK_TRIANGLE_SIZE;
            break;
        case TETRAHEDRON:
            tmp += VTK_TETRA_SIZE;
            break;
        }
        output->get_file() << tmp << " ";
    }
    /* Write DataArray end */
    output->get_file() << endl << "</DataArray>" << endl;

    /* Write DataArray begin */
    output->get_file() << "<DataArray type=\"UInt8\" Name=\"types\" format=\"ascii\">" << endl;
    /* Write type of nodes for each element */
    FOR_ELEMENTS(ele) {
        switch(ele->type) {
        case LINE:
            output->get_file() << (int)VTK_LINE << " ";
            break;
        case TRIANGLE:
            output->get_file() << (int)VTK_TRIANGLE << " ";
            break;
        case TETRAHEDRON:
            output->get_file() << (int)VTK_TETRA << " ";
            break;
        }
    }
    /* Write DataArray end */
    output->get_file() << endl << "</DataArray>" << endl;

    /* Write Cells end*/
    output->get_file() << "</Cells>" << endl;
}

/**
 * \brief Write scalar data to the VTK file (.vtu)
 * \param[in]	out		The output file
 */
static void write_flow_vtk_scalar_ascii(Output *output, OutputData *data/*FILE *out, const char *name, const int digit, ScalarFloatVector *vector*/)
{
    /* Write DataArray begin */
    output->get_file() << "<DataArray type=\"Float64\" Name=\"" << data->getName() << " " << data->getUnits() <<"\" format=\"ascii\">" << endl;//, name);
    /* Write own data */
    data->writeData(output->get_file(), " ", "  ");

    /* Write DataArray end */
    output->get_file() << endl << "</DataArray>" << endl;
}

/**
 * \brief Write vector data to VTK file (.vtu)
 * \param[in]	out		The output file
 */
static void write_flow_vtk_ascii(Output *output, OutputData *data)
{
    /* Write DataArray begin */
    output->get_file() << "<DataArray type=\"Float64\" Name=\"" << data->getName() << "_" << data->getUnits() << "\" NumberOfComponents=\"" << data->getCompNum() << "\" format=\"ascii\">" << endl;

    /* Write own data */
    data->writeData(output->get_file(), " ", "  ");

    /* Write DataArray end */
    output->get_file() << endl << "</DataArray>" << endl;
}

/**
 * \brief Go through all vectors of scalars and vectors and call functions that
 * write these data to VTK file (.vtu)
 * \param[in]	*out		The output file
 */
static void write_flow_vtk_data(Output *output, std::vector<OutputData> *data)
{
    /* Write data on nodes or elements */
    if(data != NULL) {
        for(OutputDataVec::iterator dta = data->begin();
                dta != data->end(); dta++) {
            write_flow_vtk_ascii(output, &(*dta));
        }
    }
}

/**
 * \brief Write names of scalar and vector values to the VTK file (.vtu)
 * \param[in]	*out		The output file
 */
static void write_flow_vtk_data_names(Output *output, vector<OutputData> *data)
{
    /* Write names of scalars */
    output->get_file() << "Scalars=\"";
    for(OutputDataVec::iterator dta = data->begin();
                dta != data->end(); dta++) {
        if(dta->getCompNum() == 1) {
            output->get_file() << dta->getName() << "_" << dta->getUnits();
            if((dta+1) != data->end()) {
                output->get_file() << ",";
            }
        }
    }
    output->get_file() << "\" ";

    /* Write names of vectors */
    output->get_file() << "Vectors=\"";
    for(OutputDataVec::iterator dta = data->begin();
                dta != data->end(); dta++) {
        if(dta->getCompNum() == 3) {
            output->get_file() << dta->getName() << "_" << dta->getUnits();
            if((dta+1) != data->end()) {
                output->get_file() << ",";
            }
        }
    }
    output->get_file() << "\"";
}

/**
 * \brief Write data on nodes to the VTK file (.vtu)
 * \param[in]	*out		The output file
 */
static void write_flow_vtk_node_data(Output *output)
{
    std::vector<OutputData> *node_data = output->get_node_data();

    /* Write PointData begin */
    output->get_file() << "<PointData ";
    write_flow_vtk_data_names(output, node_data);
    output->get_file() << ">" << endl;

    /* Write own data */
    write_flow_vtk_data(output, node_data);

    /* Write PointData end */
    output->get_file() << "</PointData>" << endl;
}

/**
 * \brief Write data on elements to the VTK file (.vtu)
 * \param[in]	*output		The output object
 */
static void write_flow_vtk_element_data(Output *output)
{
    std::vector<OutputData> *elem_data = output->get_elem_data();

    /* Write PointData begin */
    output->get_file() << "<CellData ";
    write_flow_vtk_data_names(output, elem_data);
    output->get_file() << ">" << endl;

    /* Write own data */
    write_flow_vtk_data(output, elem_data);

    /* Write PointData end */
    output->get_file() << "</CellData>" << endl;
}

/**
 * \brief Write tail of VTK file (.vtu)
 * \param[in]	*out	The output file
 */
static void write_flow_vtk_tail(Output *output)
{
    output->get_file() << "</UnstructuredGrid>" << endl;
    output->get_file() << "</VTKFile>" << endl;
}

/**
 * \brief This function writes mesh to the output file
 * \param[in]   *out    The output file
 */
void write_vtk_mesh(Output *output)
{
    //Mesh* mesh = (Mesh*) ConstantDB::getInstance()->getObject(MESH::MAIN_INSTANCE);

    Mesh *mesh = output->get_mesh();

    /* Write Piece begin */
    output->get_file() << "<Piece NumberOfPoints=\"" << mesh->node_vector.size() << "\" NumberOfCells=\"" << mesh->n_elements() <<"\">" << endl;

    /* Write VTK Geometry */
    write_flow_vtk_geometry(output);

    /* Write VTK Topology */
    write_flow_vtk_topology(output);
}

/**
 * \brief This function write all scalar and vector data on nodes and elements
 * to the VTK file (.vtu)
 * \param[in]	*out	The output file
 */
void write_flow_vtk_serial(Output *output)
{
    Mesh *mesh = output->get_mesh();

    /* Write header */
    write_flow_vtk_header(output);

    /* Write Piece begin */
    output->get_file() << "<Piece NumberOfPoints=\"" << mesh->node_vector.size() << "\" NumberOfCells=\"" << mesh->n_elements() <<"\">" << endl;

    /* Write VTK Geometry */
    write_flow_vtk_geometry(output);

    /* Write VTK Topology */
    write_flow_vtk_topology(output);

    /* Write VTK scalar and vector data on nodes to the file */
    write_flow_vtk_node_data(output);

    /* Write VTK data on elements */
    write_flow_vtk_element_data(output);

    /* Write Piece end */
    output->get_file() << "</Piece>" << endl;

    /* Write tail */
    write_flow_vtk_tail(output);

}

/**
 * \brief	This function create VTK file (.pvd) and writes header to it
 * \param[in]	*file The name of output file
 */
void write_trans_init_vtk_serial_ascii(char *file)
{
    FILE *out;

    xprintf( Msg, "%s: Writing output file %s ... ", __func__, file);

    out = xfopen(file, "wt");

    xfprintf(out, "<?xml version=\"1.0\"?>\n");
    xfprintf(out, "<VTKFile type=\"Collection\" version=\"0.1\" byte_order=\"LittleEndian\">\n");
    xfprintf(out, "<Collection>\n");

    xfclose(out);

    xprintf( Msg, "O.K.\n");
}

/**
 * \brief	This function writes tail of VTK file (.pvd)
 * \param[in]	*file The name of output file
 */
void write_trans_finish_vtk_serial_ascii(char *file)
{
    FILE *out;

    xprintf( Msg, "%s: Writing output file %s ... ", __func__, file);

    out = xfopen(file, "at");

    xfprintf(out, "</Collection>\n");
    xfprintf(out, "</VTKFile>\n");

    xfclose(out);

    xprintf( Msg, "O.K.\n");
}

/**
 * \brief This function writes data of transport to the VTK file (.vtu). This
 * function writes data of one frame to the one .vtu file. The name of output
 * file is derived from *file and step. When writing of data is successful,
 * then reference of .vtu file is written at the end of the file.pvd.
 * \param[in]	*transport	The transport structure
 * \param[in]	time		The unused parameter of time
 * \param[in]	step		The current time frame
 * \param[in]	*file		The name of base name of the file
 */
void write_trans_time_vtk_serial_ascii(struct Transport *transport,
        double time,
        int step,
        char *file)
{
#if 0
    Mesh* mesh = (Mesh*) ConstantDB::getInstance()->getObject(MESH::MAIN_INSTANCE);

    OutScalarsVector *element_scalar_arrays = new OutScalarsVector;
    OutVectorsVector *element_vector_arrays = new OutVectorsVector;
    struct OutScalar *p_element_out_scalar;
    struct OutVector element_out_vector;
    FILE *p_out, *s_out;
    char frame_file[PATH_MAX];
    int  subst_id, i;

    sprintf(frame_file, "%s-%d.vtu", file, step);

    /* Try to open file for frame "step" */
    s_out = xfopen(frame_file, "wt");

    if(s_out!=NULL) {

    	/* Try to open .pvd file */
        p_out = xfopen(file, "at");

        /* If it is not possible to open pvd file, then close frame file */
        if(p_out==NULL) {
        	xfclose(s_out);
        	return;
        }

        xprintf(Msg, "%s: Writing output file %s ... ", __func__, file);

        /* Find first directory delimiter */
        for(i=strlen(file); i>=0; i--) {
            if(file[i]==DIR_DELIMITER) {
                break;
            }
        }
        /* Write reference to .vtu file of current frame to pvd file */
        if(i>0) {
            /* Strip out relative path, because vtu file is in the same directory as pvd file*/
            xfprintf(p_out, "<DataSet timestep=\"%d\" group=\"\" part=\"0\" file=\"%s-%d.vtu\"/>\n", step, &file[i+1], step);
        } else {
            /* No path was found in string "file" */
            xfprintf(p_out, "<DataSet timestep=\"%d\" group=\"\" part=\"0\" file=\"%s-%d.vtu\"/>\n", step, file, step);
        }

        xfclose(p_out);
        xprintf(Msg, "O.K.\n");
    } else {
    	return;
    }

    xprintf(Msg, "%s: Writing output (frame %d) file %s ... ", __func__, step, frame_file);

    /* Write header */
    write_flow_vtk_header(s_out);

    /* Write Piece begin */
    xfprintf(s_out,
            "<Piece NumberOfPoints=\"%d\" NumberOfCells=\"%d\">\n",
            mesh->node_vector.size(),
            mesh->n_elements());

    /* Write VTK geometry */
    write_flow_vtk_geometry(s_out);

    /* Write VTK topology */
    write_flow_vtk_topology(s_out);

    /* Allocate memory for array of element scalars */
    p_element_out_scalar = (OutScalar*)xmalloc(sizeof(struct OutScalar)*transport->n_substances);

    /* Go through all substances and add them to vector of scalars */
    for(subst_id=0; subst_id<transport->n_substances; subst_id++) {
        p_element_out_scalar[subst_id].scalars = new ScalarFloatVector;

        /* Reserve memory for vectors */
        p_element_out_scalar[subst_id].scalars->reserve(mesh->n_elements());

        /* Set up names */
        strcpy(p_element_out_scalar[subst_id].name, transport->substance_name[subst_id]);

        for(int el=0; el<mesh->n_elements(); el++) {
            /* Add scalar data to vector of scalars */
            p_element_out_scalar[subst_id].scalars->push_back(transport->out_conc[subst_id][MOBILE][el]);
        }

        element_scalar_arrays->push_back(p_element_out_scalar[subst_id]);
    }

    /* Add vectors to vector */
    element_out_vector.vectors = new VectorFloatVector;
    /* Reserve memory for vector */
    element_out_vector.vectors->reserve(mesh->n_elements());
    /* Set up name */
    strcpy(element_out_vector.name, "transport_vector");
    /* Copy data */
    FOR_ELEMENTS(ele) {
        /* Add vector data do vector of vectors */
        vector<double> vec;
        vec.reserve(3);
        vec.push_back(ele->vector[0]);
        vec.push_back(ele->vector[1]);
        vec.push_back(ele->vector[2]);
        element_out_vector.vectors->push_back(vec);
    }
    element_vector_arrays->push_back(element_out_vector);

    /* Write VTK data on elements */
    write_flow_vtk_element_data(s_out, element_scalar_arrays, element_vector_arrays);

    /* Write Piece end */
    xfprintf(s_out, "</Piece>\n");

    /* Write tail */
    write_flow_vtk_tail(s_out);

    xfclose(s_out);
    xprintf( Msg, "O.K.\n");

    /* Delete unused objects */
    for(subst_id=0; subst_id<transport->n_substances; subst_id++) {
        delete p_element_out_scalar[subst_id].scalars;
    }

    xfree(p_element_out_scalar);
    delete element_out_vector.vectors;

    delete element_scalar_arrays;
    delete element_vector_arrays;
#endif
}

int write_vtk_data(Output *output)
{
    write_flow_vtk_serial(output);
    return 1;
}
