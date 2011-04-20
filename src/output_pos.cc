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

//=============================================================================
// OUTPUT ROUTINE FOR INITIALIZING FILE FOR FLOW FIELD (.msh)
//=============================================================================
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

//=============================================================================
// OUTPUT ROUTINE FOR FLOW FIELD IN SPEC. TIME
//=============================================================================
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
//=============================================================================
// OUTPUT ROUTINE FOR FLOW FIELD IN SPEC. TIME - VERSION 2 - INC. VECTOR VALUE
//=============================================================================
void output_flow_field_in_time_2(struct Problem *problem,double time)
{
    Mesh* mesh = (Mesh*) ConstantDB::getInstance()->getObject(MESH::MAIN_INSTANCE);

    int i,cit;
    ElementIter ele;
    FILE *out;
    char dbl_fmt[ 16 ];

    ASSERT(!( problem == NULL ),"NULL as argument of function output_flow_field_in_time_2()\n");
    if( OptGetBool("Output", "Write_output_file", "no") == false )
        return;

    xprintf( Msg, "%s: Writing output file %s ... ", __func__, problem->out_fname_2);

    out = xfopen( problem->out_fname_2, "at" );
    sprintf( dbl_fmt, " %%15.%dg ", ConstantDB::getInstance()->getInt("Out_digit"));
    xfprintf( out, "$FlowField\n" );
    xfprintf( out, "T = ");
    xfprintf( out, dbl_fmt, time);
    xfprintf( out, "\n%d\n", mesh->n_elements() );
    cit = 0;
    FOR_ELEMENTS( ele ) {
        xfprintf( out, "%d  ", cit);
        xfprintf( out, "%d  ", ele.id());
        xfprintf( out, " %d ", ele->n_sides);
        xfprintf( out, dbl_fmt, ele->scalar);
        for (i = 0; i < ele->n_sides; i++)
            xfprintf( out, dbl_fmt, ele->side[i]->scalar);
        xfprintf( out, "\n               ");
        for (i = 0; i < ele->n_sides; i++)
            xfprintf( out, dbl_fmt, ele->side[i]->flux);
        for (i = 0; i < 3; i++)
            xfprintf( out, dbl_fmt, ele->vector[i]);
        xfprintf( out, "%d ", ele->n_neighs_vv);
        for (i = 0; i < ele->n_neighs_vv; i++)
            xfprintf( out, "%d ", ele->neigh_vv[i]->id);
        xfprintf( out, "\n\n" );
        cit ++;
    }
    xfprintf( out, "$EndFlowField\n\n" );
    xfclose( out );

    xprintf( Msg, "O.K.\n");
}

//==============================================================================
//	INITIALIZE OUTPUT MSH-like FILES (BINARY)
//==============================================================================
void output_msh_init_bin(char *file)
{
    Mesh* mesh = (Mesh*) ConstantDB::getInstance()->getObject(MESH::MAIN_INSTANCE);

    FILE *out;
    ElementIter elm;
    NodeIter nod;
    int i,j,type,tags,zero,one,id;

    zero = 0;
    tags = 3;
    one = 1;
    type = 0;

    xprintf( Msg, "%s: Writing output file %s ... ", __func__, file);

    out = xfopen(file,"wb");
    xfprintf(out,"$MeshFormat\n");
    xfprintf(out,"%d %d %d\n",2,1,sizeof(double));
    xfwrite(&one,sizeof(int),1,out);
    xfprintf(out,"\n");
    xfprintf(out,"$EndMeshFormat\n");
    xfprintf(out,"$Nodes\n");
    xfprintf(out,"%d\n",mesh->node_vector.size());
    FOR_NODES( nod ){
        int label = nod->id;
        xfwrite(&label,sizeof(int),1,out);
        double x = nod->getX();
        xfwrite(&x,sizeof(double),1,out);
        double y = nod->getY();
        xfwrite(&y,sizeof(double),1,out);
        double z = nod->getZ();
        xfwrite(&z,sizeof(double),1,out);
    }
    xfprintf(out,"\n");
    xfprintf(out,"$EndNodes\n");
    xfprintf(out,"$Elements\n");
    xfprintf(out,"%d\n",mesh->n_elements());

    for(j = 0; j < 3; j++){
        switch(j) {
        case 0:
            type = 1;
            if(mesh->n_lines != 0){
                xfwrite(&type,sizeof(int),1,out);
                xfwrite(&mesh->n_lines,sizeof(int),1,out);
                id = 1;
            }
            else
                id = 0;
            break;
        case 1:
            type = 2;
            if(mesh->n_triangles != 0){
                xfwrite(&type,sizeof(int),1,out);
                xfwrite(&mesh->n_triangles,sizeof(int),1,out);
                id = 1;
            }
            else
                id = 0;
            break;
        case 2:
            type = 4;
            if(mesh->n_tetrahedras != 0){
                xfwrite(&type,sizeof(int),1,out);
                xfwrite(&mesh->n_tetrahedras,sizeof(int),1,out);
                id = 1;
            }
            else
                id = 0;
            break;
        default:
            id = 0;
            break;
        }

        if(id == 1) {
            xfwrite(&tags,sizeof(int),1,out);
            FOR_ELEMENTS(elm) {
                if(elm->type == type) {
                    int tmp_id=elm.id();
                    xfwrite(&tmp_id,sizeof(int),1,out);
                    xfwrite(&elm->mid,sizeof(int),1,out);
                    xfwrite(&elm->rid,sizeof(int),1,out);
                    xfwrite(&elm->pid,sizeof(int),1,out);
                    FOR_ELEMENT_NODES(elm,i) {
                        int label = elm->node[i]->id;
                        xfwrite(&label,sizeof(int),1,out);
                    }
                }
            }
        }
    }
    xfprintf(out,"\n");
    xfprintf(out,"$EndElements\n");

    xfclose(out);

    xprintf( Msg, "O.K.\n");
}

//==============================================================================
//	INITIALIZE OUTPUT MSH-like FILES (ASCII)
//==============================================================================
void output_msh_init_ascii(char* file)
{
    Mesh* mesh = (Mesh*) ConstantDB::getInstance()->getObject(MESH::MAIN_INSTANCE);

    FILE *out;
    ElementIter elm;
    NodeIter nod;
    int i,type,tags,zero,one;

    zero = 0;
    tags = 3;
    one = 1;
    type = 0;

    xprintf( Msg, "%s: Writing output file %s ... ", __func__, file);

    out = xfopen(file,"wt");
    xfprintf(out,"$MeshFormat\n");
    xfprintf(out,"%d %d %d\n",2,0,sizeof(double));
    xfprintf(out,"$EndMeshFormat\n");
    xfprintf(out,"$Nodes\n");
    xfprintf(out, "%d\n", mesh->node_vector.size());
    FOR_NODES( nod ) {
        xfprintf(out,"%d %f %f %f\n",nod->id,nod->getX(),nod->getY(),nod->getZ());
    }
    xfprintf(out,"$EndNodes\n");
    xfprintf(out,"$Elements\n");
    xfprintf(out,"%d\n",mesh->n_elements());

    FOR_ELEMENTS(elm) {
        xfprintf(out,"%d %d %d %d %d %d",elm.id(),elm->type,3,elm->mid,elm->rid,elm->pid);
        FOR_ELEMENT_NODES(elm,i)
        xfprintf(out," %d",elm->node[i]->id);
        xfprintf(out,"\n");
    }
    xfprintf(out,"$EndElements\n");

    xfclose(out);

    xprintf( Msg, "O.K.\n");
}

//==============================================================================
//	OUTPUT TRANSPORT SCALAR FIELD (BINARY)
//==============================================================================
void output_transport_time_bin(struct Transport *transport,
		double time,
		int step,
		char *file)
{
    Mesh* mesh = (Mesh*) ConstantDB::getInstance()->getObject(MESH::MAIN_INSTANCE);

    FILE *out;
    int sbi,el,itags,rtags,stags,comp,i,vcomp;
    int i_out;
    double vector[3];
    itags = 3;
    rtags = 1;
    stags = 1;
    comp = 1;
    vcomp = 3;

    xprintf( Msg, "%s: Writing output file %s ... ", __func__, file);

    out = xfopen(file,"ab");

    /* Scalar view */
    for(sbi=0;sbi<transport->n_substances;sbi++) {
        xfprintf(out,"$ElementData\n");
        xfprintf(out,"%d\n",stags);                     // one string tag
        xfprintf(out,"\"Concentration of %s\"\n",transport->substance_name[sbi]);          // string tag
        xfprintf(out,"%d\n",rtags);                                   // one raal tag
        xfprintf(out,"%f\n",time);                                // first real tag = time
        xfprintf(out,"%d\n",itags);                                   // 3 int tags
        xfprintf(out,"%d\n",step);                              // step number (start = 0)
        xfprintf(out,"%d\n",comp);                                   // one component - scalar field
        xfprintf(out,"%d\n",mesh->n_elements());                    // n follows elements
        FOR_ELEMENTS(ele) {
            i_out=ele.id();
            xfwrite(&i_out,sizeof(int),1,out);
            xfwrite(&transport->out_conc[sbi][MOBILE][ele.index()],sizeof(double),1,out);
        }
        xfprintf(out,"\n$EndElementData\n");
    }

    /* Vector view */
    for(sbi=0;sbi<transport->n_substances;sbi++){

        xfprintf(out,"$ElementData\n");
        xfprintf(out,"%d\n",stags);                     // one string tag
        xfprintf(out,"\"Concentration of %s\"\n",transport->substance_name[sbi]);          // string tag
        xfprintf(out,"%d\n",rtags);                                   // one raal tag
        xfprintf(out,"%f\n",time);                                // first real tag = time
        xfprintf(out,"%d\n",itags);                                   // 3 int tags
        xfprintf(out,"%d\n",step);                              // step number (start = 0)
        xfprintf(out,"%d\n",vcomp);                                   // three components - vector field
        xfprintf(out,"%d\n",mesh->n_elements());                    // n follows elements
        FOR_ELEMENTS(ele) {
            if(vector_length(ele->vector) > ZERO){
                for(i = 0; i < 3; i++)
                    vector[i] = mesh->element[el].vector[i];
                normalize_vector(vector);
                scale_vector(vector,transport->out_conc[sbi][MOBILE][el]);
            }
            else {
                for(i = 0; i < 3; i++) vector[i] = 0.0;
            }

            i_out=ele.id();
            xfwrite(&i_out,sizeof(int),1,out);
            xfwrite(&vector,3*sizeof(double),1,out);
        }
    }
    xfprintf(out,"$EndElementData\n");

    xfclose(out);

    xprintf( Msg, "O.K.\n");
}

/**
 * \brief This function writes only scalar data of the transport to the ascii
 * pos file.
 * \param[in]	*transport	The transport structure
 * \param[in]	time		The unused parameter of time
 * \param[in]	step		The current time frame
 * \param[in]	*file		The name of base name of the file
 */
void output_transport_time_ascii(struct Transport *transport,
		double time,
		int step,
		char *file)
{
    Mesh* mesh = (Mesh*) ConstantDB::getInstance()->getObject(MESH::MAIN_INSTANCE);

    FILE *out;
    int sbi,el,itags,rtags,stags,comp,i,vcomp;
    double vector[3];
    itags = 3;
    rtags = 1;
    stags = 1;
    comp = 1;
    vcomp = 3;

    xprintf( Msg, "%s: Writing output file %s ... ", __func__, file);

    out = xfopen(file,"at");
    /* Scalar view */
    for(sbi=0; sbi<transport->n_substances; sbi++){
        xfprintf(out,"$ElementData\n");
        xfprintf(out,"%d\n",stags);  // one string tag
        xfprintf(out,"\"Concentration of %s\"\n",transport->substance_name[sbi]);    // string tag
        xfprintf(out,"%d\n",rtags);  // one raal tag
        xfprintf(out,"%f\n",time);   // first real tag = time
        xfprintf(out,"%d\n",itags);  // 3 int tags
        xfprintf(out,"%d\n",step);   // step number (start = 0)
        xfprintf(out,"%d\n",comp);   // one component - scalar field
        xfprintf(out,"%d\n",mesh->n_elements());   // n follows elements
        FOR_ELEMENTS(ele)
        		xfprintf(out,"%d %f\n", ele.id(), transport->out_conc[sbi][MOBILE][ele.index()]);
        xfprintf(out,"$EndElementData\n");
    }

    xfclose(out);

    xprintf( Msg, "O.K.\n");
}

void write_msh_header(Output *output)
{
    // Write simple header
    output->get_base_file() << "$MeshFormat" << endl;
    output->get_base_file() << "2" << " 0 " << sizeof(double) << endl;
    output->get_base_file() << "$EndMeshFormat" << endl;
}

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

void write_msh_topology(Output *output)
{
    Mesh* mesh = output->get_mesh();
    ElementIter elm;
    int i;

    // Write information about elements
    output->get_base_file() << "$Elements" << endl;
    output->get_base_file() << mesh->n_elements() << endl;
    FOR_ELEMENTS(elm) {
        output->get_base_file() << elm.id() << " " << elm->type << " 3 " << elm->mid << " " << elm->rid << " " << elm->pid;
        FOR_ELEMENT_NODES(elm,i)
            output->get_base_file() << " " << elm->node[i]->id;
        output->get_base_file() << endl;
    }
    output->get_base_file() << "$EndElements" << endl;
}

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

void write_msh_node_data(OutputTime *output, double time, int step)
{
}

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
int write_msh_data(OutputTime *output, double time, int step)
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
