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
 * @brief   Header: The functions for all outputs.
 *
 */

#ifndef OUTPUT_H
#define OUTPUT_H

#include <vector>
#include <string>
#include <fstream>

#include "system.hh"

/// External types
struct Problem;
struct Transport;
class Mesh;

// FILE formats
#define POS_ASCII           1
#define POS_BIN             2
#define VTK_SERIAL_ASCII    3
#define VTK_PARALLEL_ASCII  4

// VTK Element types
#define VTK_VERTEX          1
#define VTK_POLY_VERTEX     2
#define VTK_LINE            3
#define VTK_POLY_LINE       4
#define VTK_TRIANGLE        5
#define VTK_TRIANGLE_STRIP  6
#define VTK_POLYGON         7
#define VTK_PIXEL           8
#define VTK_QUAD            9
#define VTK_TETRA           10
#define VTK_VOXEL           11
#define VTK_HEXAHEDRON      12
#define VTK_WEDGE           13
#define VTK_PYRAMID         14

#define VTK_QUADRIC_EDGE        21
#define VTK_QUADRIC_TRIANGLE    22
#define VTK_QUADRIC_QUAD        23
#define VTK_QUADRIC_TETRA       24
#define VTK_QUADRIC_HEXAHEDRON  25

// VTK Element size (number of nodes)
#define VTK_LINE_SIZE       2
#define VTK_TRIANGLE_SIZE   3
#define VTK_TETRA_SIZE      4

// Types of output files
#define GMSH_STYLE  1
#define FLOW_DATA_FILE 2
#define BOTH_OUTPUT 3

// Types of data, that could be written to output file
#define OUT_INT         1
#define OUT_INT_VEC     2
#define OUT_FLOAT       3
#define OUT_FLOAT_VEC   4
#define OUT_DOUBLE      5
#define OUT_DOUBLE_VEC  6

/* Temporary structure for storing data */
typedef std::vector<double> ScalarFloatVector;
typedef std::vector< vector<double> > VectorFloatVector;

/* Temporary structure for storing data */
typedef struct OutScalar {
    ScalarFloatVector   *scalars;
    string              name;
    string              unit;
} OutScalars;

/* Temporary structure for storing data */
typedef struct OutVector {
    VectorFloatVector   *vectors;
    string              name;
    string              unit;
} OutVector;

/**
 * Class of output data storing reference on data
 */
class OutputData {
private:
    string          name;
    string          units;
    void            *data;
    unsigned char   type;
    int             comp_num;
    OutputData() {};          // Un-named constructor can't be called
public:
    string getName(void) { return name; };
    string getUnits(void) { return units; };
    int getCompNum(void) { return comp_num; };
    void writeData(ofstream &file, string item_sep, string vec_sep);
    OutputData(std::string name, std::string unit, std::vector<int> &data);
    OutputData(std::string name, std::string unit, std::vector< vector<int> > &data);
    OutputData(std::string name, std::string unit, std::vector<float> &data);
    OutputData(std::string name, std::string unit, std::vector< vector<float> > &data);
    OutputData(std::string name, std::string unit, std::vector<double> &data);
    OutputData(std::string name, std::string unit, std::vector< vector<double> > &data);
    ~OutputData();
};

typedef std::vector<OutputData> OutputDataVec;

/**
 * Class of output
 */
class Output {
private:
    ofstream    *file;          ///< Output stream
    string      *filename;      ///< String with output filename
    char        format_type;    ///< Type of output
    Mesh        *mesh;
    std::vector<OutputData> *node_data;    ///< List of data on nodes
    std::vector<OutputData> *elem_data;    ///< List of data on elements

    Output() {};            // Un-named constructor can't be called

    // Internal API for file formats
public:
    Output(Mesh *mesh, string filename);
    ~Output();

    ///< This method writes geometry, topology of mesh and all data to the file
    //int write_data(void);
    int (*write_data)(Output *output);

    ///< This method registers node data, that will be written to the file,
    ///< when write_data() will be called
    template <typename _Data>
    int register_node_data(std::string name, std::string unit, std::vector<_Data> &data);

    ///< This method register element data
    template <typename _Data>
    int register_elem_data(std::string name, std::string unit, std::vector<_Data> &data);

    // Getters
    std::vector<OutputData> *get_node_data(void) { return node_data; };
    std::vector<OutputData> *get_elem_data(void) { return elem_data; };
    ofstream& get_file(void) { return *file; };
    Mesh *get_mesh(void) { return mesh; };
};

/**
 * Class of output of during time
 */
class OutputTime : public Output {
    int         current_step;
public:
    OutputTime(string filename);
    ~OutputTime();

    ///< This method writes geometry, topology of mesh and all data to the file
    int write_data(double time, int step);
    ///< This method registers node data, that will be written to the file,
    ///< when write_data() will be called
    template <typename _Data>
    int register_node_data(std::string name, std::string unit, int step, std::vector<_Data> &data);
    ///< This method register element data
    template <typename _Data>
    int register_elem_data(std::string name, std::string unit, int step, std::vector<_Data> &data);
};

typedef std::vector<OutScalar> OutScalarsVector;
typedef std::vector<OutVector> OutVectorsVector;

/* TODO: remove this function prototype (temporary solution) */
void output( struct Problem *problem );

/* TODO: remove following function prototype from this .h file */

void output_flow_field_init(char *fname);
void output_flow_field_in_time(struct Problem *problem,double time);

void output_init(void);
void output_time(double time);

void output_msh_init_bin(char *file);
void output_msh_init_ascii(char *file);

void write_trans_init_vtk_serial_ascii(char *file);
void write_trans_finish_vtk_serial_ascii(char *file);

void output_transport_time_bin(struct Transport *transport, double time, int step, char *file);
void output_transport_time_ascii(struct Transport *transport, double time, int step, char *file);
void write_trans_time_vtk_serial_ascii(struct Transport *transport, double time, int step, char *file);

void write_flow_vtk_serial(FILE *out);
int write_vtk_data(Output *output);

#endif
//-----------------------------------------------------------------------------
// vim: set cindent:
