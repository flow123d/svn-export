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

//==============================================================================
//      WRITE ELEMENT POSITION TO THE BINARY FILE
//==============================================================================
void write_elm_position_to_binary_output(ElementIter elm,FILE *file){
    int i;

    FOR_ELEMENT_NODES(elm,i) {
        double x = elm->node[i]->getX();
        xfwrite(&x,sizeof(double),1,file);
    }
    FOR_ELEMENT_NODES(elm,i) {
        double y = elm->node[i]->getY();
        xfwrite(&y,sizeof(double),1,file);
    }
    FOR_ELEMENT_NODES(elm,i) {
        double z = elm->node[i]->getZ();
        xfwrite(&z,sizeof(double),1,file);
    }
}

//==============================================================================
//      WRITE FLOW BINARY DATA TO THE POS FILE
//==============================================================================
// TODO: vystupni metody musi byt nezavisle na tom co vystupuji
void write_flow_binary_data(FILE *out,struct Problem *problem){

        ElementIter ele;

        Mesh* mesh = (Mesh*) ConstantDB::getInstance()->getObject(MESH::MAIN_INSTANCE);

        int   view,i,j;
        int one = 1;
        double ts = 0;
        double value;

        char dbl_fmt[16];

        int n_es;
        struct Neighbour *ngh;
        struct Side *sde;
        double vector[3];

        sprintf(dbl_fmt,"%%.%dg ", ConstantDB::getInstance()->getInt("Out_digit"));

        char* description = xstrcpy(OptGetStr("Global", "Description", "No description."));

        for(i=0; i < ((signed)strlen(description) - 2);i++)
                if((description[i] == ' ') || (description[i] == '\t'))
                        description[i] = '^';

        xfprintf(out, "$PostFormat\n");
        xfprintf(out, "%g %d %d\n", 1.4, 1, sizeof(double));
        xfprintf(out, "$EndPostFormat\n");
        for(view = 0;view < 3;view++){
                switch(view){
                        case 0:
                                xfprintf( out, "$View\n%s^-^p ", description );
                                break;
                        case 1:
                                xfprintf( out, "$View\n%s^-^pc ", description );
                                break;
                        case 2:
                        	xfprintf( out, "$View\n%s^-^pz ", description );
                                break;
                }

        xfprintf(out, "1 0 0 0 %d 0 0 %d 0 0 0 0 0 %d 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 \n",mesh->n_lines,mesh->n_triangles,mesh->n_tetrahedras);
        xfwrite(&one, sizeof(int), 1, out);
        xfwrite(&ts, sizeof(double), 1, out);

        for(i = 1;i < 4;i++)
                FOR_ELEMENTS( ele )
                        if(ele->dim == i){
                                write_elm_position_to_binary_output(ele,out);
                                        FOR_ELEMENT_NODES(ele,j)
                                                switch(view){
                                                 case 0:
                                                  xfwrite(&ele->node[j]->scalar,sizeof(double),1,out);
                                                  break;
                                                 case 1:
                                                  xfwrite(&ele->scalar,sizeof(double),1,out);
                                                  break;
                                                 case 2:
                                                  value = ele->scalar + ele->centre[2];
                                                  xfwrite(&value,sizeof(double),1,out);
                                                  break;
                                                 }
                        }
                xfprintf(out, "\n$EndView\n");
        }

	xfprintf( out, "$View\n%s^-^u ", description );
        xfprintf(out, "1 0 %d 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 \n",mesh->n_elements());
        xfwrite(&one, sizeof(int), 1, out);
        xfwrite(&ts, sizeof(double), 1, out);
	FOR_ELEMENTS( ele ){
                for(i=0;i<3;i++)
                        xfwrite(&ele->centre[i],sizeof(double),1,out);
                for(i=0;i<3;i++)
                        xfwrite(&ele->vector[i],sizeof(double),1,out);
        }
        xfprintf(out, "\n$EndView\n");

        // NB 20

     n_es = 0;
     FOR_NEIGHBOURS( ngh )
    	 if(ngh->type == VB_ES)
    		 n_es++;

     if(n_es > 0){
     xfprintf( out, "$View\n%s^-^u^comp ", description );
     xfprintf(out, "1 0 %d 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 \n",n_es);
     xfwrite(&one, sizeof(int), 1, out);
     xfwrite(&ts, sizeof(double), 1, out);

     FOR_NEIGHBOURS( ngh )
    	 if(ngh->type == VB_ES){

    	 ele = ngh->element[0];
    	 sde = ngh->element[1]->side[ngh->sid[1]];

    	 for(i=0;i<3;i++)
    		 xfwrite(&ele->centre[i],sizeof(double),1,out);

         for(i=0;i<3;i++)
        	 vector[i] = sde->normal[i];

         scale_vector(vector,-sde->flux);

         for(i=0;i<3;i++)
        	 xfwrite(&vector[i],sizeof(double),1,out);
     }

     xfprintf(out, "\n$EndView\n");

     // END NB 20
     }


     /*
	xfprintf( out, "$View\n%s^-^Qv ", description );
        xfprintf(out, "1 0 %d 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 \n",mesh->n_elements());
        xfwrite(&one, sizeof(int), 1, out);
        xfwrite(&ts, sizeof(double), 1, out);
	FOR_ELEMENTS( ele ){
                for(i=0;i<3;i++)
                        xfwrite(&ele->centre[i],sizeof(double),1,out);

                switch(ele->dim){
                        case 1:
                        case 2:
                                for(i=0;i<3;i++){
                                        qvector = ele->vector[i] * ele->material->size;
                                        xfwrite(&qvector,sizeof(double),1,out);
                                }
                                break;
                        case 3:
                                flux = 0;
                                FOR_ELEMENT_SIDES(ele,i)
                                        if(ele->side[i]->flux > 0)
                                                flux += ele->side[i]->flux;
                                        for(j=0;j<3;j++){
                                                qvector = flux * ele->vector[j] / vector_length(ele->vector);
                                                xfwrite(&qvector,sizeof(double),1,out);
                                        }
                                break;
                }
        }
        xfprintf(out, "\n$EndView\n");
       */
}


//==============================================================================
//      WRITE FLOW ASCII DATA TO THE POS FILE
//==============================================================================
// TODO: nezavislost na vystupovanych datech
void write_flow_ascii_data(FILE *out,struct Problem *problem){
    Mesh* mesh = (Mesh*) ConstantDB::getInstance()->getObject(MESH::MAIN_INSTANCE);

    Node* nod;
    ElementIter ele;
    char dbl_fmt[ 16 ];
    int view,li;

    sprintf( dbl_fmt, "%%.%dg ", ConstantDB::getInstance()->getInt("Out_digit"));

    /* Write to the pos file format p, pc, pz */
    for(view=0; view<3; view++) {

        const char* description = OptGetStr("Global", "Description", "No description.");

        /* Write to the output name of problem  */
        switch(view){
            case 0:
                xfprintf( out, "View \"%s - p\" {\n",description );
                break;
            case 1:
                xfprintf( out, "View \"%s - pc\" {\n", description );
                break;
            case 2:
                xfprintf( out, "View \"%s - pz\" {\n", description );
                break;
        }

        /* Go through all elements */
        FOR_ELEMENTS(ele) {
            /* Write type and beginning of element position */
            switch( ele->type ) {
            case LINE:
                xfprintf( out, "SL (" );
                break;
            case TRIANGLE:
                xfprintf( out, "ST (" );
                break;
            case TETRAHEDRON:
                xfprintf( out, "SS (" );
                break;
            }

            /* Write coordinates of all point of current element */
            FOR_ELEMENT_NODES(ele, li) {
                nod = ele->node[ li ];
                xfprintf( out, dbl_fmt, nod->getX() );
                xfprintf( out, ", " );
                xfprintf( out, dbl_fmt, nod->getY() );
                xfprintf( out, ", " );
                xfprintf( out, dbl_fmt, nod->getZ() );
                /* Write delimiter between coordinates */
                if( li != ele->n_nodes - 1 )
                    xfprintf( out, ", " );
            }
            xfprintf( out, ") {" );

            /* Write value of element */
            FOR_ELEMENT_NODES(ele, li) {
                nod = ele->node[ li ];
                switch(view){
                case 0:
                    xfprintf( out, dbl_fmt, nod->scalar );
                    break;
                case 1:
                    xfprintf( out, dbl_fmt, ele->scalar );
                    break;
                case 2:
                    xfprintf( out, dbl_fmt, ele->scalar + ele->centre[2]);
                    break;
                }
                /* Write delimiter between values */
                if( li != ele->n_nodes - 1 )
                    xfprintf( out, ", " );
            }
            xfprintf( out, "};\n" );
        }
        xfprintf( out, "};\n" );
    }

    /* Write vectors in centers of elements */
	xfprintf( out, "View \"%s - u\" {\n", OptGetStr("Global", "Description", "No description.") );
	FOR_ELEMENTS( ele ) {
		xfprintf( out, "VP (" );
		xfprintf( out, dbl_fmt, ele->centre[ 0 ] );
		xfprintf( out, ", " );
		xfprintf( out, dbl_fmt, ele->centre[ 1 ] );
		xfprintf( out, ", " );
		xfprintf( out, dbl_fmt, ele->centre[ 2 ] );
		xfprintf( out, ") {" );
		xfprintf( out, dbl_fmt, ele->vector[ 0 ] );
		xfprintf( out, ", " );
		xfprintf( out, dbl_fmt, ele->vector[ 1 ] );
		xfprintf( out, ", " );
		xfprintf( out, dbl_fmt, ele->vector[ 2 ] );
		xfprintf( out, "};\n" );
	}
	xfprintf( out, "};\n" );
}

//=============================================================================
// OUTPUT ROUTINE FOR INITIALIZING FILE FOR FLOW FIELD
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
//      OUTPUT INIT
//==============================================================================
void output_init(void)
{
    Mesh* mesh = (Mesh*) ConstantDB::getInstance()->getObject(MESH::MAIN_INSTANCE);

    FILE *out;
    int i;
    ElementIter ele;
    NodeIter nod;
    char dbl_fmt[ 16 ];
    char filename[255];

    const char* out_fname = OptGetFileName("Output", "Output_file", NULL);
    xprintf( Msg, "%s: Writing output file %s ... ", __func__, out_fname);

    sprintf( dbl_fmt, "%%.%dg ", ConstantDB::getInstance()->getInt("Out_digit"));
    sprintf( filename,"%s.tmp", out_fname);
    out = xfopen( filename, "wt" );

    xfprintf( out, "Nodes {\n");
    FOR_NODES( nod ) {
        xfprintf(out,"%d   ",nod->id);
        xfprintf(out,dbl_fmt,nod->getX());xfprintf(out,"  ");
        xfprintf(out,dbl_fmt,nod->getY());xfprintf(out,"  ");
        xfprintf(out,dbl_fmt,nod->getZ());xfprintf(out,"\n");
    }
    xfprintf( out, "};\n");
    xfprintf( out, "Elements {\n");
    FOR_ELEMENTS(ele){
        xfprintf(out,"%d  %d  %d   ",ele.id(),ele->type,ele->rid);
        FOR_ELEMENT_NODES(ele,i) {
            xfprintf(out,"%d  ",ele->node[i]->id);
        }
        xfprintf(out,"\n");
    }
    xfprintf( out, "};\nVALUES\n");
    xfclose(out);

    xprintf( Msg, "O.K.\n");
}

//==============================================================================
//      OUTPUT FLOW IN THE TIME
//==============================================================================
void output_time(double time)
{
    Mesh* mesh = (Mesh*) ConstantDB::getInstance()->getObject(MESH::MAIN_INSTANCE);

    FILE *out;
    ElementIter ele;
    NodeIter nod;
    char dbl_fmt[16];
    char filename[PATH_MAX];

    const char* out_fname = OptGetFileName("Output", "Output_file", NULL);
    xprintf( Msg, "%s: Writing output file %s ... ", __func__, out_fname);

    sprintf( dbl_fmt, "%%.%dg ", ConstantDB::getInstance()->getInt("Out_digit"));
    sprintf( filename,"%s.tmp", out_fname);
    out = xfopen( filename, "at" );

    xfprintf( out, "Time =");
    xfprintf( out, dbl_fmt, time);
    xfprintf( out, "{\n");
    xfprintf( out, "  Nodes {\n" );
    FOR_NODES( nod ){
        xfprintf(out,"  %d   ",nod->id);
        xfprintf(out,dbl_fmt,nod->scalar);
        xfprintf(out,"\n");
    }
    xfprintf( out, "  };\n" );
    xfprintf( out, "  Elements {\n" );
    FOR_ELEMENTS( ele ){
        xfprintf(out,"  %d   ",ele.id());
        xfprintf(out,dbl_fmt,ele->scalar);
        xfprintf(out,dbl_fmt,ele->vector[0]);
        xfprintf(out,dbl_fmt,ele->vector[1]);
        xfprintf(out,dbl_fmt,ele->vector[2]);
        xfprintf(out,"\n");
    }
    xfprintf( out, "  };\n" );
    xfprintf( out, "};\n" );
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
        xfprintf(out,"%d\n",vcomp);                                   // one component - scalar field
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

    /* Vector view */
/*
    for(sbi=0; sbi<transport->n_substances; sbi++){
        xfprintf(out,"$ElementData\n");
        xfprintf(out,"%d\n",stags);  // one string tag
        xfprintf(out,"\"Concentration of %s\"\n",transport->substance_name[sbi]);          // string tag
        xfprintf(out,"%d\n",rtags);  // one raal tag
        xfprintf(out,"%f\n",time);   // first real tag = time
        xfprintf(out,"%d\n",itags);  // 3 int tags
        xfprintf(out,"%d\n",step);   // step number (start = 0)
        xfprintf(out,"%d\n",vcomp);  // one component - scalar field
        xfprintf(out,"%d\n",mesh->n_elements());   // n follows elements
        for(el=0;el < mesh->n_elements();el++) {
            if(vector_length(mesh->element_hash[mesh->epos_id[el]]->vector) > ZERO) {
                for(i = 0; i < 3; i++)
                    vector[i] = mesh->element_hash[mesh->epos_id[el]]->vector[i];
                normalize_vector(vector);
                scale_vector(vector,transport->conc[sbi][MOBILE][el]);
            } else {
                for(i = 0; i < 3; i++)
                    vector[i] = 0.0;
            }
            xfprintf(out,"%d %f %f %f\n",mesh->epos_id[el],vector[0],vector[1],vector[2]);
        }
    }
    xfprintf(out,"$EndElementData\n");
    */
    xfclose(out);

    xprintf( Msg, "O.K.\n");
}

/* =============================================================================
 * IMPORTANT NOTE: This is unused code. When following functions will not be
 * used, then it will be deleted in the future!
 * =============================================================================*/
#if 0
//==============================================================================
//      OUTPUT TRANSPORT INIT
//==============================================================================
void output_transport_init(struct Problem *problem)
{
    FILE **out;
    int i,j;
    ElementIter ele;
    Mesh* mesh;
    Node* nod;
    char dbl_fmt[ 16 ];

    sprintf( dbl_fmt, "%%.%dg ", problem->out_digit );
    out = open_temp_files(problem->transport,"%s.tmp","wt");
    mesh = problem->mesh;

    for(j = 0; j < 4; j++)
    {
        if (out[j] == NULL) continue;
        xfprintf( out[j], "Nodes {\n");
        FOR_NODES(nod){
            xfprintf(out[j],"%d   ",nod->id);
            xfprintf(out[j],dbl_fmt,nod->getX());xfprintf(out[j],"  ");
            xfprintf(out[j],dbl_fmt,nod->getY());xfprintf(out[j],"  ");
            xfprintf(out[j],dbl_fmt,nod->getZ());xfprintf(out[j],"\n");
        }
        xfprintf( out[j], "};\n");
        xfprintf( out[j], "Elements {\n");
        FOR_ELEMENTS(ele){
            xfprintf(out[j],"%d  %d  %d   ",ele.id(),ele->type,ele->rid);
            FOR_ELEMENT_NODES(ele,i)
            xfprintf(out[j],"%d  ",ele->node[i]->id);
            xfprintf(out[j],"\n");
        }
        xfprintf( out[j], "};\nVALUES\n");
        xfclose(out[j]);
    } //for end
    xfree(out);
}

//==============================================================================
//      OUTPUT TRANSPORT TIME
//==============================================================================
void output_transport_time(struct Problem *problem, double time)
{
        int i;
        FILE **out;
        ElementIter ele;
        Mesh* mesh = problem->mesh;
        Node* nod;
        char dbl_fmt[ 16 ];
        char filename[255];       // out
    int sbi;
    int n_subst;

    n_subst = problem->transport->n_substances;
    sprintf( dbl_fmt, "%%.%dg ", problem->out_digit );
    sprintf( filename,"%s.tmp",problem->transport->transport_out_fname);
    out = open_temp_files(problem->transport,"%s.tmp","at");

        for(i = 0; i < 4; i++)        // phase for
        {
        if (out[i] == NULL) continue;
        xfprintf( out[i], "Time =");
        xfprintf( out[i], dbl_fmt, time);
        xfprintf( out[i], "{\n");
        xfprintf( out[i], "  Nodes {\n" );
        FOR_NODES( nod ){
                xfprintf(out[i],"  %d   ",nod->id);
        for( sbi = 0; sbi < n_subst; sbi++ )
                        switch(i)
                        {
                        case 0:
                    xfprintf(out[i],dbl_fmt,nod->conc[sbi]);
                        break;
                        case 1:
                    xfprintf(out[i],dbl_fmt,nod->conc_immobile[sbi]);
                        break;
                        case 2:
                    xfprintf(out[i],dbl_fmt,nod->conc_sorb[sbi]);
                        break;
                        case 3:
                    xfprintf(out[i],dbl_fmt,nod->conc_immobile_sorb[sbi]);
                        break;
                        }
                xfprintf(out[i],"\n");
        }
        xfprintf( out[i], "  };\n" );
        xfprintf( out[i], "  Elements {\n" );
        FOR_ELEMENTS( ele ){
                xfprintf(out[i],"  %d   ",ele.id());
        for( sbi = 0; sbi < n_subst; sbi++ )
                        switch(i)
                        {
                        case 0:
                    xfprintf(out[i],dbl_fmt,ele->conc[sbi]);
                        break;
                        case 1:
                    xfprintf(out[i],dbl_fmt,ele->conc_immobile[sbi]);
                        break;
                        case 2:
                    xfprintf(out[i],dbl_fmt,ele->conc_sorb[sbi]);
                        break;
                        case 3:
                    xfprintf(out[i],dbl_fmt,ele->conc_immobile_sorb[sbi]);
                        break;
                        }
                xfprintf(out[i],"\n");
        }
        xfprintf( out[i], "  };\n" );
        xfprintf( out[i], "};\n" );
        xfclose( out[i] );

        } //end for
        xfree(out);
}

//==============================================================================
// outputs velocity field (vector per element) in the ftrans coef file format
//==============================================================================

void output_veloc_ftrans(struct Problem *problem,double time)
{
  int i,cit;
    ElementIter ele;
    Mesh* mesh;
    FILE *out;
    char dbl_fmt[ 16 ];

    ASSERT(!( problem == NULL ),"NULL as argument of function output_flow_field_in_time()\n");
    if( problem->write_output == false )
        return;
    mesh = problem->mesh;
        xprintf( Msg, "Writing flow output files... ");// orig verb 2
        out = xfopen( "coef_veloc.txt", "wt" );
    //sprintf( dbl_fmt, "%%.%dg ", problem->out_digit );
        sprintf( dbl_fmt, " %%15.%dg ", problem->out_digit );
        xfprintf( out, "Velocity_field\n" );
    xfprintf( out, "default 0    0   0   0   0   0\n");
    xfprintf( out, "{\n");
//        xfprintf( out, dbl_fmt, time);
//  xfprintf( out, "\n%d\n", mesh->n_elements() );
        cit = 0;
    FOR_ELEMENTS( ele ) {
//                xfprintf( out, "%d  ", cit);
                xfprintf( out, "0  ");        // fracture NR. is always zero
                xfprintf( out, "%d  ", ele.id());
//                xfprintf( out, " %d ", ele->n_sides);     // 2do: test elem type - only for triangles

        //now works only in xy plane
                xfprintf( out, dbl_fmt, ele->vector[0]);
                xfprintf( out, " 0 ");  // additinal coefs for non-constant velocity on element
                xfprintf( out, " 0 ");
                xfprintf( out, dbl_fmt, ele->vector[1]);
                xfprintf( out, " 0 ");  // additinal coefs for non-constant velocity on element
                xfprintf( out, " 0 ");

                xfprintf( out, "\n");
                cit ++;
        }
        xfprintf( out, "}\n" );
        xfclose( out );
 }

//==============================================================================
// INITIALIZE TRANSPORT OUTPUT FILE FOR CROSS SECTION
//==============================================================================
void output_transport_init_CS(struct Problem *problem)
{
        FILE **out;
        int i,j;
        char dbl_fmt[ 16 ];
        ElementIter *elm_list;

        sprintf( dbl_fmt, "%%.%dg\t", problem->out_digit );
        elm_list = problem->section->element_list;

        out = open_temp_files(problem, "%s.cs", "wt" );

        for(i=0; i < 4; i++)
        if(out[i] == NULL) continue;
        else
        {
        for(j=0; j < problem->section->n_elm; j++)
        xfprintf(out[i], dbl_fmt, elm_list[j]->faux );
        xfprintf(out[i],"\n");
        xfclose(out[i]);
        }
}
//==============================================================================
// TRANSPORT CROSS SECTION OUTPUT IN TIME
//==============================================================================
void output_transport_time_CS(struct Problem *problem, double time)
{
        FILE **out;
      //  Mesh* mesh;
      //  TNode* nod;
    char dbl_fmt[ 16 ];
    int i,j,sbi,n_elm,n_subst;
        ElementIter *elm,**elm_list;

        elm = problem->mesh->element_hash;
        n_elm = problem->section->n_elm;
        elm_list = problem->section->element_list;
    n_subst = problem->n_substances;
    sprintf( dbl_fmt, "%%.%dg\t", problem->out_digit );
       // mesh = problem->mesh;
        out = open_temp_files(problem, "%s.cs", "at" );
        for(i=0; i < 4; i++){
        if(out[i] == NULL) continue;
        for( sbi = 0; sbi < n_subst; sbi++ )
        {
     //   xfprintf( out[i], "%s\t", problem->substance_name[sbi]);
     //  xfprintf( out[i], dbl_fmt, time);
        for(j=0; j < n_elm; j++)
                switch(i)
                {
                case 0:
                xfprintf(out[0],dbl_fmt,elm_list[j]->conc[sbi]);
                break;
                case 1:
                xfprintf(out[1],dbl_fmt,elm_list[j]->conc_immobile[sbi]);
                break;
                case 2:
                xfprintf(out[2],dbl_fmt,elm_list[j]->conc_sorb[sbi]);
                break;
                case 3:
                xfprintf(out[3],dbl_fmt,elm_list[j]->conc_immobile_sorb[sbi]);
                break;
                }
                xfprintf(out[i],"\n");
        }
     //   xfprintf( out[i], "\n" );
        xfclose( out[i] );
        }
}

//==============================================================================
//      OUTPUT TRANSPORT TIME MATRIX
//==============================================================================
void output_transport_time_matrix(struct Problem *problem, double time)
{
        int i,el;
        FILE **out;
        ElementIter ele;
        Mesh* mesh;
        Node* nod;
    char dbl_fmt[ 16 ];
    int sbi;
    int n_subst;

    n_subst = problem->transport->n_substances;
        mesh = problem->mesh;
    sprintf( dbl_fmt, "%%.%dg ", problem->out_digit );
        out = open_temp_files(problem->transport,"%s.tmp","at");

        for(i = 0; i < 4; i++)        // phase for
        {
        if (out[i] == NULL) continue;
        xfprintf( out[i], "Time =");
        xfprintf( out[i], dbl_fmt, time);
        xfprintf( out[i], "{\n");
        xfprintf( out[i], "  Nodes {\n" );
        FOR_NODES( nod ){
                xfprintf(out[i],"  %d   ",nod->id);
        for( sbi = 0; sbi < n_subst; sbi++ )
                        switch(i)
                        {
                        case 0:
                    xfprintf(out[i],dbl_fmt,nod->conc[sbi]);
                        break;
                        case 1:
                    xfprintf(out[i],dbl_fmt,nod->conc_immobile[sbi]);
                        break;
                        case 2:
                    xfprintf(out[i],dbl_fmt,nod->conc_sorb[sbi]);
                        break;
                        case 3:
                    xfprintf(out[i],dbl_fmt,nod->conc_immobile_sorb[sbi]);
                        break;
                        }
                xfprintf(out[i],"\n");
        }
        xfprintf( out[i], "  };\n" );
        xfprintf( out[i], "  Elements {\n" );
        for(el=0;el<mesh->n_elements();el++){
                xfprintf(out[i],"  %d   ",mesh->epos_id[el]);
        for( sbi = 0; sbi < n_subst; sbi++ )
                        switch(i)
                        {
                        case 0:
                    xfprintf(out[i],dbl_fmt,problem->transport->conc[sbi][MOBILE][el]);
                        break;
                        case 1:
                    xfprintf(out[i],dbl_fmt,ele->conc_immobile[sbi]);
                        break;
                        case 2:
                    xfprintf(out[i],dbl_fmt,ele->conc_sorb[sbi]);
                        break;
                        case 3:
                    xfprintf(out[i],dbl_fmt,ele->conc_immobile_sorb[sbi]);
                        break;
                        }
                xfprintf(out[i],"\n");
        }
        xfprintf( out[i], "  };\n" );
        xfprintf( out[i], "};\n" );
        xfclose( out[i] );

        } //end for
        xfree(out);
}

#endif
