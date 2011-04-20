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
 * @file main.cc
 * @mainpage
 * @author Otto Severýn, Milan Hokr, Jiří Kopal, Jan Březina, Jiří Hnídek, Jiří Jeníček
 * @date April, 2009
 *
 * @brief Flow123d is simulator of underground water flow and transport. Main features are:
 * - simulation of fully saturated water flow in 1D,2D,3D domains and their combination
 * - simulation of transport with linear sorption
 * - simulation of density driven flow
 *
 * This version links against some external software:
 * - PETSC : http://www.mcs.anl.gov/petsc/petsc-2/documentation/index.html
 *
 */

#include "constantdb.h"
#include "mesh/ini_constants_mesh.hh"

#include "transport.h"

#include <petsc.h>

#include "system.hh"
#include "xio.h"
#include "mesh.h"
#include "topology.h"
#include "postprocess.h"
#include "output.h"
#include "preprocess.h"
#include "problem.h"
#include "calculation.h"
#include "darcy_flow_mh.hh"
#include "main.h"
#include "read_ini.h"
#include "global_defs.h"
#include "btc.h"
#include "reaction.h"

#include "solve.h"

/*
#include "solve.h"
#include "elements.h"
#include "sides.h"
#include "math_fce.h"
#include "materials.h"
 */

#include "rev_num.h"
/// named version of the program
#define _VERSION_   "1.6.0"


static struct Problem G_problem;

static void main_compute_mh(struct Problem*);
static void main_compute_mh_unsteady_saturated(struct Problem*);
static void main_compute_mh_steady_saturated(struct Problem*);
static void main_convert_to_pos(struct Problem*);
static void main_compute_mh_density(struct Problem*);
//void output_transport_init_BTC(struct Problem *problem);
//void output_transport_time_BTC(struct Problem *problem, double time);

/**
 * @brief Main flow initialization
 * @param[in] argc       command line argument count
 * @param[in] argv       command line arguments
 * @param[out] goal      Flow computation goal
 * @param[out] ini_fname Init file name
 *
 * TODO: this parsing function should be in main.cc
 *
 */
void parse_cmd_line(const int argc, char * argv[], int &goal, string &ini_fname) {
    const char USAGE_MSG[] = "\
    Wrong program parameters.\n\
    Usage: flow123d [options] ini_file\n\
    Options:\n\
    -s       Compute MH problem (Obsolete)\n\
             Source files have to be in the current directory.\n\
    -S       Compute MH problem\n\
             Source files have to be in the same directory as ini file.\n\
    -c       Convert flow data files into Gmsh parsed post-processing file format\n";

    xprintf(MsgLog, "Parsing program parameters ...\n");

    goal = -1;

    // Check command line arguments
    if ((argc >= 3) && (strlen(argv[1]) == 2) && (argv[1][0] == '-')) {
        std::string ini_argument ( argv[2] );
        std::string ini_dir;

        // Try to find absolute or relative path in fname
        int delim_pos=ini_argument.find_last_of(DIR_DELIMITER);
        if (delim_pos < ini_argument.npos) {
            // It seems, that there is some path in fname ... separate it
            ini_dir=ini_argument.substr(0,delim_pos);
            ini_fname=ini_argument.substr(delim_pos+1); // till the end
        } else {
            ini_dir=".";
            ini_fname=ini_argument;
        }

        switch (argv[ 1 ][ 1 ]) {
            case 's': goal = COMPUTE_MH;
                ini_fname=ini_argument;
                break;
            case 'S': goal = COMPUTE_MH;
                xchdir(ini_dir.c_str());
                break;
            case 'c': goal = CONVERT_TO_POS;
                break;
        }

    }

    if (goal < 0) {
        //xprintf(UsrErr, USAGE_MSG);   // Caused crash of flow123d
        printf("%s", USAGE_MSG);
    }
}

//=============================================================================

/**
 *  FUNCTION "MAIN"
 */
int main(int argc, char **argv) {
    int goal;
    std::string ini_fname;

    F_ENTRY;

    parse_cmd_line(argc, argv, goal, ini_fname); // command-line parsing
    if (goal == -1) {
        return EXIT_FAILURE;
    } else {
        ConstantDB::getInstance()->setInt("Goal", goal);
    }

    system_init(argc, argv); // Petsc, open log, read ini file
    OptionsInit(ini_fname.c_str()); // Read options/ini file into database
    system_set_from_options();

    // Say Hello
    xprintf(Msg, "This is FLOW-1-2-3, version %s rev: %s\n", _VERSION_,REVISION);
    xprintf(Msg, "Built on %s at %s.\n", __DATE__, __TIME__);

    // Read inputs
    problem_init(&G_problem);
    if (OptGetBool("Transport", "Transport_on", "no") == true) {
        alloc_transport(&G_problem);
        transport_init(&G_problem);
    }

    make_mesh(&G_problem);

    /* Test of object storage */
    Mesh* mesh = (Mesh*) ConstantDB::getInstance()->getObject(MESH::MAIN_INSTANCE);
    int numNodes = mesh->node_vector.size();
    xprintf(Msg, " - Number of nodes in the mesh is: %d\n", numNodes);

    // Calculate
    make_element_geometry();
    preprocess(&G_problem);
    switch (ConstantDB::getInstance()->getInt("Goal")) {
        case CONVERT_TO_POS:
            main_convert_to_pos(&G_problem);
            break;
        case COMPUTE_MH:
            main_compute_mh(&G_problem);
            break;
    }

    // Say Goodbye
    return xterminate(false);
}

/**
 * FUNCTION "MAIN" FOR CONVERTING FILES TO POS
 */
void main_convert_to_pos(struct Problem *problem) {
    // TODO: write outputs
    xprintf(Err, "Not implemented yet in this version\n");
}

/**
 * FUNCTION "MAIN" FOR COMPUTING MIXED-HYBRID PROBLEM
 */
void main_compute_mh(struct Problem *problem) {
    switch (ConstantDB::getInstance()->getInt("Problem_type")) {
        case STEADY_SATURATED:
            main_compute_mh_steady_saturated(problem);
            break;
        case UNSTEADY_SATURATED:
            main_compute_mh_unsteady_saturated(problem);
            break;
        case PROBLEM_DENSITY:
            main_compute_mh_density(problem);
            break;
    }
}

/**
 * FUNCTION "MAIN" FOR COMPUTING MIXED-HYBRID PROBLEM FOR UNSTEADY SATURATED FLOW
 */
void main_compute_mh_unsteady_saturated(struct Problem *problem) {
    Mesh* mesh = (Mesh*) ConstantDB::getInstance()->getObject(MESH::MAIN_INSTANCE);

    int t, i;
    output_flow_field_init(problem->out_fname_2);
    //        output_flow_field_in_time(problem,0);
    calculation_mh(problem);


    for (t = 0, i = 0; t * problem->time_step < problem->stop_time; t++, i++) {
        calculation_unsteady(problem);
        problem->water=new DarcyFlowMH(*mesh);
        problem->water->solve();
        postprocess(problem);

        if ((i * problem->time_step >= problem->save_step) || (t == 0)) {
            output_flow_field_in_time(problem, t * problem->time_step);
            i = 0;
        }
    }
}

/**
 * FUNCTION "MAIN" FOR COMPUTING MIXED-HYBRID PROBLEM FOR STEADY SATURATED FLOW
 */
void main_compute_mh_steady_saturated(struct Problem *problem)
{
    Mesh* mesh = (Mesh*) ConstantDB::getInstance()->getObject(MESH::MAIN_INSTANCE);

    struct Transport *transport;
    int rank;
    /*
       Mesh* mesh;
       ElementIter elm;
       struct Side *sde;
       FILE *out;
       int i;
       mesh=problem->mesh;
     */

    MPI_Comm_rank(PETSC_COMM_WORLD, &rank);

    calculation_mh(problem);
    problem->water=new DarcyFlowMH(*mesh);
    problem->water->solve();

    if (OptGetBool("Transport", "Transport_on", "no") == true)
            {
            make_transport(problem->transport);
            }

	xprintf( Msg, "O.K.\n")/*orig verb 2*/;

    postprocess(problem);

    output();

    // pracovni vystup nekompatibilniho propojeni
    // melo by to byt ve water*
    /*
       {
            ElementIter ele;
            Element *ele2;
            int ngi;
            Neighbour *ngh;
            Mesh* mesh = problem->mesh;

            double sum1,sum2;
            DarcyFlowMH *w=problem->water;
            double *x = w->schur0->vx;


            FOR_ELEMENTS_IT( ele ) {
                FOR_ELM_NEIGHS_VV( ele, ngi ) {
                    ngh = ele->neigh_vv[ ngi ];
                    // get neigbour element, and set appropriate column
               ele2 = ( ngh->element[ 0 ] == &(*ele) ) ? ngh->element[ 1 ] : ngh->element[ 0 ];

               double out_flux=0.0;
               for(i=0;i<=ele->dim;i++) out_flux+=x[w->side_row_4_id[ele->side[i]->id]];
               xprintf(Msg,"El 1 (%f,%f) %d %f %g\n",
                       ele->centre[0],
                       ele->centre[1],
                       ele->dim,
                       x[w->el_row_4_id[ele->id]],out_flux);

               out_flux=0.0;
               for(i=0;i<=ele2->dim;i++) out_flux+=x[w->side_row_4_id[ele2->side[i]->id]];

               xprintf(Msg,"El 2 (%f,%f) %d %f %g\n",
                                  ele2->centre[0],
                                  ele2->centre[1],
                                  ele2->dim,
                                  x[w->el_row_4_id[ele2->id]],out_flux);
                }
            }
       }
     */
    transport = problem->transport;

    /*
        out = xfopen("pepa.txt","wt");

        FOR_ELEMENTS(elm)
                    elm->aux = 0;

        FOR_ELEMENTS(elm)
            FOR_ELEMENT_SIDES(elm,i)
                            if(elm->side[i]->type == EXTERNAL)
                                    if((elm->side[i]->centre[2] - elm->centre[2]) > 0.0)
                                            if(elm->side[i]->normal[2] != 0.0)
                                                    if((elm->side[i]->centre[2]) > 300.0)
                                                            if((elm->material->id == 2200) || (elm->material->id == 2207) || (elm->material->id == 2212) || (elm->material->id == 2217) || (elm->material->id == 9100) || (elm->material->id == 9107) || (elm->material->id == 9112) || (elm->material->id == 9117))
                                                                    elm->aux = 1;

        FOR_ELEMENTS(elm)
                    if(elm->aux == 1)
                            xfprintf(out,"%d\n",elm->id);

        xfclose(out);
     */

    if (OptGetBool("Transport", "Transport_on", "no") == true) {
        OutputTime *_output_time = NULL;

        if (transport->reaction_on == true) { /* tohle presunu na rozumnejsi misto, jen co takove bude */
            read_reaction_list(transport);
        }

        if (rank == 0) {
            _output_time = new OutputTime(mesh, transport->transport_out_fname);
            _output_time->write_head(_output_time);
            //transport_output_init(transport);
            _output_time->get_data_from_transport(transport, 0);
            _output_time->write_data(_output_time, 0.0, 0);
            _output_time->free_data_from_transport(transport);
            //transport_output(transport, 0.0, 0);
        }
	
        
       // if (problem->transport->mpi != 1) {
       //     transport_output(transport, 0.0, 0);
       // }
        

        // TODO: there is an uncoditioned jump in open_temp_files
        // also this function should be moved to btc.*
        // btc should be documented and have an clearly defined interface
        // not strictly dependent on Transport.
        //btc_check(transport);

        convection(transport, _output_time);
        /*
                if(problem->cross_section == true)
                {
                    elect_cross_section_element(problem);
                    output_transport_init_CS(problem);
                    output_transport_time_CS(problem, 0 * problem->time_step);
                }
         */
        //transport_output_finish(transport);
        if(_output_time != NULL) {
            _output_time->write_tail(_output_time);
            delete _output_time;
        }
    }
}

//-----------------------------------------------------------------------------
// vim: set cindent:
//-----------------------------------------------------------------------------

/**
 * FUNCTION "MAIN" FOR COMPUTING MIXED-HYBRID PROBLEM FOR UNSTEADY SATURATED FLOW
 */
void main_compute_mh_density(struct Problem *problem)
{
    Mesh* mesh = (Mesh*) ConstantDB::getInstance()->getObject(MESH::MAIN_INSTANCE);
    int i, j, dens_step, n_step, frame = 0;
    double save_step, stop_time; // update_dens_time
    char statuslog[255];
    struct Transport *trans = problem->transport;
    FILE *log;

    OutputTime *_output_time = new OutputTime(mesh ,trans->transport_out_fname);

    _output_time->write_head(_output_time);
    // transport_output_init(problem->transport); TODO: remove

    _output_time->get_data_from_transport(trans, 0);
    _output_time->write_data(_output_time, 0.0, ++frame);
    _output_time->free_data_from_transport(trans);
    // transport_output(problem->transport, 0.0, ++frame); TODO: remove

    save_step = problem->save_step;
    stop_time = problem->stop_time;
    trans->update_dens_time = problem->save_step / (ceil(problem->save_step / trans->dens_step));
    dens_step = (int) ceil(problem->stop_time / trans->update_dens_time);
    n_step = (int) (problem->save_step / trans->update_dens_time);

    // DF problem - I don't understend to this construction !!!
    problem->save_step = problem->stop_time = trans->update_dens_time;

    //------------------------------------------------------------------------------
    //      Status LOG head
    //------------------------------------------------------------------------------
    //sprintf( statuslog,"%s.txt",problem->log_fname);
    sprintf(statuslog, "density_log.txt");
    log = xfopen(statuslog, "wt");
    xfprintf(log, "Stop time = %f (%f) \n", trans->update_dens_time * dens_step, stop_time);
    xfprintf(log, "Save step = %f \n", save_step);
    xfprintf(log, "Density  step = %f (%d) \n\n", trans->update_dens_time, trans->dens_step);
    xfprintf(log, "Time\t Iteration number\n");
    //------------------------------------------------------------------------------

    for (i = 0; i < dens_step; i++) {
        xprintf(Msg, "dens step %d \n", i);
        save_time_step_C(problem);

        for (j = 0; j < trans->max_dens_it; j++) {
            xprintf(Msg, "dens iter %d \n", j);
            save_restart_iteration_H(problem);
            //restart_iteration(problem);
            calculation_mh(problem);
            problem->water=new DarcyFlowMH(*mesh);
            problem->water->solve();
            restart_iteration_C(problem);
            postprocess(problem);
            convection(trans, _output_time);

            if (trans->dens_implicit == 0) {
                xprintf(Msg, "no density iterations (explicit)", j);
                break;
            }
            if (compare_dens_iter(problem) && (j > 0)) {
                break; //at least one repeat of iteration is necessary to update both conc and pressure
            }
        }
        if (trans -> write_iterations == 0) {
            //transport_output(problem->transport, i * problem->time_step, ++frame);
            _output_time->get_data_from_transport(trans, ++frame);
            // call _output_time->register_node_data(name, unit, frame, data) to register other data on nodes
            // call _output_time->register_elem_data(name, unit, frame, data) to register other data on elements
            _output_time->write_data(_output_time, i * problem->time_step, frame);
            _output_time->free_data_from_transport(trans);
        }

        if ((trans -> write_iterations == 0) && (((i + 1) % n_step == 0) || (i == (dens_step - 1)))) {
            //transport_output(problem->transport, (i + 1) * problem->stop_time, ++frame);
            _output_time->get_data_from_transport(trans, ++frame);
            // call _output_time->register_node_data(name, unit, frame, data) to register other data on nodes
            // call _output_time->register_elem_data(name, unit, frame, data) to register other data on elements
            _output_time->write_data(_output_time, (i + 1) * problem->stop_time, frame);
            _output_time->free_data_from_transport(trans);
        }
        xprintf(Msg, "step %d finished at %d density iterations\n", i, j);
        xfprintf(log, "%f \t %d\n", (i + 1) * trans->update_dens_time, j); // Status LOG
    }

    //transport_output_finish(problem->transport);
    _output_time->write_tail(_output_time);
    delete _output_time;

    output();
    xfclose(log);
}
