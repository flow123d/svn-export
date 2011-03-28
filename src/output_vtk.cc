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
static void write_flow_vtk_header(FILE *out)
{
    xfprintf(out, "<?xml version=\"1.0\"?>\n");
    // TODO: test endianess of platform (this is important, when raw data are
    // saved to the VTK file)
    xfprintf(out, "<VTKFile type=\"UnstructuredGrid\" version=\"0.1\" byte_order=\"LittleEndian\">\n");
    xfprintf(out, "<UnstructuredGrid>\n");
}

/**
 * \brief Write geometry (position of nodes) to the VTK file (.vtu)
 * \param[in] *out The output file
 */
static void write_flow_vtk_geometry(FILE *out)
{
    Mesh* mesh = (Mesh*) ConstantDB::getInstance()->getObject(MESH::MAIN_INSTANCE);

    NodeIter node;
    char dbl_fmt[16];
    int tmp;

    /* Digit precision */
    sprintf(dbl_fmt, "%%.%dg ", ConstantDB::getInstance()->getInt("Out_digit"));

    /* Write Points begin*/
    xfprintf(out, "<Points>\n");
    /* Write DataArray begin */
    xfprintf(out, "<DataArray type=\"Float64\" NumberOfComponents=\"3\" format=\"ascii\">\n");
    /* Write own coordinates */
    tmp = 0;
    FOR_NODES( node ) {
        node->aux = tmp;   /* store index in the auxiliary variable */

        xfprintf(out, dbl_fmt, node->getX());
        xfprintf(out, dbl_fmt, node->getY());
        xfprintf(out, dbl_fmt, node->getZ());

        tmp++;
    }
    /* Write DataArray end */
    xfprintf(out, "\n</DataArray>\n");
    /* Write Points end*/
    xfprintf(out, "</Points>\n");
}

/**
 * \brief Write topology (connection of nodes) to the VTK file (.vtu)
 * \param[in] *out The output file
 */
static void write_flow_vtk_topology(FILE *out)
{
    Mesh* mesh = (Mesh*) ConstantDB::getInstance()->getObject(MESH::MAIN_INSTANCE);

    Node* node;
    ElementIter ele;
    char dbl_fmt[16];
    int li, tmp;

    /* Digit precision */
    sprintf(dbl_fmt, "%%.%dg ", ConstantDB::getInstance()->getInt("Out_digit"));

    /* Write Cells begin*/
    xfprintf(out, "<Cells>\n");
    /* Write DataArray begin */
    xfprintf(out, "<DataArray type=\"Int32\" Name=\"connectivity\" format=\"ascii\">\n");
    /* Write own coordinates */
    FOR_ELEMENTS(ele) {
        FOR_ELEMENT_NODES(ele, li) {
            node = ele->node[li];
            xfprintf(out, "%d ", node->aux);   /* Write connectivity */
        }
    }
    /* Write DataArray end */
    xfprintf(out, "\n</DataArray>\n");

    /* Write DataArray begin */
    xfprintf(out, "<DataArray type=\"Int32\" Name=\"offsets\" format=\"ascii\">\n");
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
        xfprintf(out, "%d ", tmp);
    }
    /* Write DataArray end */
    xfprintf(out, "\n</DataArray>\n");

    /* Write DataArray begin */
    xfprintf(out, "<DataArray type=\"UInt8\" Name=\"types\" format=\"ascii\">\n");
    /* Write type of nodes for each element */
    FOR_ELEMENTS(ele) {
        switch(ele->type) {
        case LINE:
            xfprintf(out, "%d ", VTK_LINE);
            break;
        case TRIANGLE:
            xfprintf(out, "%d ", VTK_TRIANGLE);
            break;
        case TETRAHEDRON:
            xfprintf(out, "%d ", VTK_TETRA);
            break;
        }
    }
    /* Write DataArray end */
    xfprintf(out, "\n</DataArray>\n");

    /* Write Cells end*/
    xfprintf(out, "</Cells>\n");
}

/**
 * \brief Write scalar data to the VTK file (.vtu)
 * \param[in]	out		The output file
 * \param[in]	*name	The name of scalar data
 * \param[in]	digit	The digit precision
 * \param[in]	*vector	The vector of scalar values
 */
static void write_flow_vtk_scalar_ascii(FILE *out, const char *name, const int digit, ScalarFloatVector *vector)
{
    char dbl_fmt[16];

    /* Digit precision */
    sprintf(dbl_fmt, "%%.%dg ", digit);

    /* Write DataArray begin */
    xfprintf(out, "<DataArray type=\"Float64\" Name=\"%s\" format=\"ascii\">\n", name);
    /* Write own data */
    for(ScalarFloatVector::iterator val=vector->begin(); val != vector->end(); val++) {
        xfprintf(out, dbl_fmt, *val);
    }
    /* Write DataArray end */
    xfprintf(out, "\n</DataArray>\n");
}

/**
 * \brief Write vector data to VTK file (.vtu)
 * \param[in]	out		The output file
 * \param[in]	*name	The name of vector data
 * \param[in]	digit	The digit precision
 * \param[in]	*vector	The vector of vectors
 */
static void write_flow_vtk_vector_ascii(FILE *out, const char *name, const int digit, VectorFloatVector *vector)
{
    char dbl_fmt[16];

    /* Digit precision */
    sprintf(dbl_fmt, "%%.%dg ", digit);

    /* Write DataArray begin */
    xfprintf(out, "<DataArray type=\"Float64\" Name=\"%s\" NumberOfComponents=\"3\" format=\"ascii\">\n", name);
    /* Write own data */
    for(VectorFloatVector::iterator val=vector->begin(); val != vector->end(); val++) {
        xfprintf(out, dbl_fmt, val->d[0]);
        xfprintf(out, dbl_fmt, val->d[1]);
        xfprintf(out, dbl_fmt, val->d[2]);
    }
    /* Write DataArray end */
    xfprintf(out, "\n</DataArray>\n");
}

/**
 * \brief Go through all vectors of scalars and vectors and call functions that
 * write these data to VTK file (.vtu)
 * \param[in]	*out		The output file
 * \param[in]	*scalars	The vector of scalars
 * \param[in]	*vectors	The vector of vectors
 */
static void write_flow_vtk_data(FILE *out, OutScalarsVector *scalars, OutVectorsVector *vectors)
{
    int digit = ConstantDB::getInstance()->getInt("Out_digit");

    /* Write to the file all scalar arrays if any */
    if(scalars != NULL) {
        for(OutScalarsVector::iterator sca = scalars->begin();
                sca != scalars->end(); sca++) {
            write_flow_vtk_scalar_ascii(out, sca->name, digit , sca->scalars);
        }
    }

    /* Write to the file all vector arrays if any */
    if(vectors != NULL) {
       for(OutVectorsVector::iterator vec = vectors->begin();
                vec != vectors->end(); vec++) {
            write_flow_vtk_vector_ascii(out, vec->name, digit, vec->vectors);
        }

    }
}

/**
 * \brief Write names of scalar and vector values to the VTK file (.vtu)
 * \param[in]	*out		The output file
 * \param[in]	*scalars	The vector of scalars
 * \param[in]	*vectors	The vector of vectors
 */
static void write_flow_vtk_data_names(FILE *out, OutScalarsVector *scalars, OutVectorsVector *vectors)
{
    /* Write list of scalar array names if any */
    if(scalars != NULL) {
        xfprintf(out, "Scalars=\"");
        /* Write all names of scalar arrays first */
        for(OutScalarsVector::iterator sca = scalars->begin();
                sca != scalars->end(); sca++) {
            xfprintf(out, "%s", sca->name);
            if((sca+1) != scalars->end()) {
                xfprintf(out, ",");
            }
        }
        xfprintf(out, "\"");
    }

    /* Write list of vector array names if any */
    if(vectors != NULL) {
        /* Write list of vector array names */
        xfprintf(out, " Vectors=\"");
        /* Write all names of scalar arrays first */
        for(OutVectorsVector::iterator vec = vectors->begin();
                vec != vectors->end(); vec++) {
            xfprintf(out, "%s", vec->name);
            if((vec+1) != vectors->end()) {
                xfprintf(out, ",");
            }
        }
        xfprintf(out, "\"");
    }
}

/**
 * \brief Write data on nodes to the VTK file (.vtu)
 * \param[in]	*out		The output file
 * \param[in]	*scalars	The vector of scalars
 * \param[in]	*vectors	The vector of vectors
 */
static void write_flow_vtk_node_data(FILE *out, OutScalarsVector *scalars, OutVectorsVector *vectors)
{
    /* Write PointData begin */
    xfprintf(out, "<PointData ");
    write_flow_vtk_data_names(out, scalars, vectors);
    xfprintf(out, ">\n");

    /* Write own data */
    write_flow_vtk_data(out, scalars, vectors);

    /* Write PointData end */
    xfprintf(out, "</PointData>\n");
}

/**
 * \brief Write data on elements to the VTK file (.vtu)
 * \param[in]	*out		The output file
 * \param[in]	*scalars	The vector of scalars
 * \param[in]	*vectors	The vector of vectors
 */
static void write_flow_vtk_element_data(FILE *out, OutScalarsVector *scalars, OutVectorsVector *vectors)
{
    /* Write PointData begin */
    xfprintf(out, "<CellData ");
    write_flow_vtk_data_names(out, scalars, vectors);
    xfprintf(out, ">\n");

    /* Write own data */
    write_flow_vtk_data(out, scalars, vectors);

    /* Write PointData end */
    xfprintf(out, "</CellData>\n");
}

/**
 * \brief Write tail of VTK file (.vtu)
 * \param[in]	*out	The output file
 */
static void write_flow_vtk_tail(FILE *out)
{
    xfprintf(out, "</UnstructuredGrid>\n");
    xfprintf(out, "</VTKFile>\n");
}

/**
 * \brief This function writes mesh to the output file
 * \param[in]   *out    The output file
 */
void write_vtk_mesh(FILE *out)
{
    Mesh* mesh = (Mesh*) ConstantDB::getInstance()->getObject(MESH::MAIN_INSTANCE);

    /* Write Piece begin */
    xfprintf(out,
            "<Piece NumberOfPoints=\"%d\" NumberOfCells=\"%d\">\n",
            mesh->node_vector.size(),
            mesh->n_elements());

    /* Write VTK Geometry */
    write_flow_vtk_geometry(out);

    /* Write VTK Topology */
    write_flow_vtk_topology(out);
}

/**
 * \brief This function write all scalar and vector data on nodes and elements
 * to the VTK file (.vtu)
 * \param[in]	*out	The output file
 */
void write_flow_vtk_serial(FILE *out)
{
    Mesh* mesh = (Mesh*) ConstantDB::getInstance()->getObject(MESH::MAIN_INSTANCE);

    NodeIter node;
    OutScalarsVector *node_scalar_arrays = new OutScalarsVector;
    OutScalarsVector *element_scalar_arrays = new OutScalarsVector;
    OutVectorsVector *element_vector_arrays = new OutVectorsVector;
    struct OutScalar node_out_scalar;
    struct OutScalar element_out_scalar;
    struct OutVector element_out_vector;

    node_out_scalar.scalars = new ScalarFloatVector;
    element_out_scalar.scalars = new ScalarFloatVector;
    element_out_vector.vectors = new VectorFloatVector;

    /* Write header */
    write_flow_vtk_header(out);

    /* Write Piece begin */
    xfprintf(out,
            "<Piece NumberOfPoints=\"%d\" NumberOfCells=\"%d\">\n",
            mesh->node_vector.size(),
            mesh->n_elements());

    /* Write VTK Geometry */
    write_flow_vtk_geometry(out);

    /* Write VTK Topology */
    write_flow_vtk_topology(out);

    /* Fill vector of node scalars */
    strcpy(node_out_scalar.name, "node_scalars");
    /* Generate vector for scalar data of nodes */
    node_out_scalar.scalars->reserve(mesh->node_vector.size());   // reserver memory for vector
    FOR_NODES( node ) {
        node_out_scalar.scalars->push_back(node->scalar);
    }
    node_scalar_arrays->push_back(node_out_scalar);

    /* Write VTK scalar and vector data on nodes to the file */
    write_flow_vtk_node_data(out, node_scalar_arrays, NULL);

    /* Fill vectors of element scalars and vectors */
    strcpy(element_out_scalar.name, "element_scalars");
    strcpy(element_out_vector.name, "element_vectors");
    /* Generate vectors for scalar and vector data of nodes */
    element_out_scalar.scalars->reserve(mesh->n_elements());
    element_out_vector.vectors->reserve(mesh->n_elements());
    FOR_ELEMENTS(ele) {
        tripple t;  /* TODO: more effective */
        t.d[0] = ele->vector[0];
        t.d[1] = ele->vector[1];
        t.d[2] = ele->vector[2];
        element_out_scalar.scalars->push_back(ele->scalar);
        element_out_vector.vectors->push_back(t);
    }
    element_scalar_arrays->push_back(element_out_scalar);
    element_vector_arrays->push_back(element_out_vector);

    /* Write VTK data on elements */
    write_flow_vtk_element_data(out, element_scalar_arrays, element_vector_arrays);

    /* Write Piece end */
    xfprintf(out, "</Piece>\n");

    /* Write tail */
    write_flow_vtk_tail(out);

    /* Delete unused object */
    delete node_out_scalar.scalars;
    delete element_out_scalar.scalars;
    delete element_out_vector.vectors;

    delete node_scalar_arrays;
    delete element_scalar_arrays;
    delete element_vector_arrays;
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
    Mesh* mesh = (Mesh*) ConstantDB::getInstance()->getObject(MESH::MAIN_INSTANCE);

    OutScalarsVector *element_scalar_arrays = new OutScalarsVector;
    OutVectorsVector *element_vector_arrays = new OutVectorsVector;
    struct OutScalar *p_element_out_scalar;
    struct OutVector element_out_vector;
    FILE *p_out, *s_out;
    char frame_file[PATH_MAX];
    int  subst_id, i;
    tripple t;

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
        t.d[0] = ele->vector[0];
        t.d[1] = ele->vector[1];
        t.d[2] = ele->vector[2];
        element_out_vector.vectors->push_back(t);
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
}
