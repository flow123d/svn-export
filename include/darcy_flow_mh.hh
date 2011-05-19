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
 * @brief mixed-hybrid model of linear Darcy flow, possibly unsteady.
 *
 * Main object for mixed-hybrid discretization of the linear elliptic PDE (Laplace)
 * on  a multidimensional domain. Discretization of saturated Darcy flow using
 * RT0 approximation for the velocity
 *
 *
 *
 *  @author Jan Brezina
 *
 */

/*
 * list of files dependent on this one:
 *
 * posprocess.cc
 * problem.cc
 * main.hh
 * transport.cc
 */


#ifndef DARCY_FLOW_MH_HH
#define DARCY_FLOW_MH_HH

#include <petscmat.h>
#include <sys_vector.hh>
#include <time_governor.hh>

/// external types:
class LinSys;
struct Solver;
class Mesh;
class SchurComplement;
class Distribution;
class SparseGraph;
class FieldP0;


/**
 * @brief mixed-hybrid model of linear Darcy flow, possibly unsteady.
 *
 *
 */

/**
 * TODO: TimeGovernor should distinct const  methods and here get_tim should return reference to const time object.
 * then caller can ask TimeGovernor about time, compare time (with relative precision given by actual dt), nut still
 * can not modify TimeGovernor object. TimeGovernor should allow recasting to double.
 *
 * split compute_one_step to :
 * 1) prepare_next_timestep
 * 2) actualize_solution - this is for iterative nonlinear solvers
 *
 * make interface of DarcyFlowMH a general interface of time depenedent model. ....
 */

class DarcyFlowMH {
public:
    virtual void compute_one_step() =0;
    virtual void compute_until( double time);
    inline const TimeGovernor& get_time()
        {return *time;}
    virtual double * solution_vector() =0;
    virtual void postprocess() =0;

protected:
    TimeGovernor *time;

};

class DarcyFlowMH_Steady : public DarcyFlowMH
{
public:
    DarcyFlowMH_Steady(Mesh &mesh);
    virtual void compute_one_step();
    virtual double * solution_vector();
    virtual void postprocess() {};
    ~DarcyFlowMH_Steady();

protected:
    virtual void modify_system() {};
    void prepare_parallel();
    void make_row_numberings();
    void mh_abstract_assembly();
    void make_schur0();
    void make_schur1();
    void make_schur2();


	Mesh *mesh;   // structured water equation ( so far pointer to  the mesh struture )

	int size;				// global size of MH matrix
	int  n_schur_compls;  	// number of shur complements to make
	double  *solution; 			// sequantial scattered solution vector

	struct Solver *solver;

	LinSys *schur0;  		// whole MH Linear System
	SchurComplement *schur1;  	// first schur compl.
	SchurComplement *schur2;	// second ..

	FieldP0 *sources;

	// parallel
	int np;  // number of procs
	int myp; // my proc number
	int	 lsize;				// local size of whole MH matrix
	Distribution *edge_ds; // optimal distribution of edges
	Distribution *el_ds; // optimal distribution of elements
	Distribution *side_ds; // optimal distribution of elements
	Distribution *rows_ds; // final distribution of rows of MH matrix

	int *el_4_loc;		// array of ids of local elements (in ordering matching the optimal global)
	int *row_4_el;		// element id to matrix row
	int *side_id_4_loc;		// array of ids of local sides
	int	*side_row_4_id;		// side id to matrix row
	int *edge_id_4_loc;		// array of ids of local edges
	int	*edge_row_4_id;		// edge id to matrix row
	int *old_4_new;        // aux. array should be only part of parallel LinSys

	// MATIS related arrays
	int ndof_loc;                   // size of local block of MATIS matrix 
	int *global_row_4_sub_row;      // global dof index for subdomain index
    
	ISLocalToGlobalMapping map_side_local_to_global; ///< PETSC mapping form local SIDE indices of subdomain to global indices

	// gather of the solution
	Vec sol_vec;			// vector over solution array
	VecScatter par_to_all;
};


void make_element_connection_graph(Mesh *mesh, SparseGraph * &graph,bool neigh_on = false);
void id_maps(int n_ids, int *id_4_old, const Distribution &old_ds,
        int *loc_part, Distribution * &new_ds, int * &id_4_loc, int * &new_4_id);
void mat_count_off_proc_values(Mat m, Vec v);

void create_water_linsys(Mesh*, DarcyFlowMH**);
void solve_water_linsys(DarcyFlowMH*);

class DarcyFlowMH_Unsteady : public DarcyFlowMH_Steady
{
public:
    DarcyFlowMH_Unsteady(Mesh &mesh);
    DarcyFlowMH_Unsteady();
protected:
    virtual void modify_system();
private:
    Vec steady_diagonal;
    Vec steady_rhs;
    Vec new_diagonal;
    Vec previous_solution;


};

class DarcyFlowMH_UnsteadyLumped : public DarcyFlowMH_Steady
{
public:
    DarcyFlowMH_UnsteadyLumped(Mesh &mesh);
    DarcyFlowMH_UnsteadyLumped();
protected:
    virtual void modify_system();
    virtual void postprocess();
private:
    Vec steady_diagonal;
    Vec steady_rhs;
    Vec new_diagonal;
    Vec previous_solution;
    Vec time_term;


};

#endif  //DARCY_FLOW_MH_HH
//-----------------------------------------------------------------------------
// vim: set cindent:
