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
 * @brief Discontinuous Galerkin method for equation of transport with dispersion.
 *  @author Jan Stebel
 */

#include "petscmat.h"
#include <armadillo>
#include "system/xio.h"
#include "transport/transport_dg.hh"
#include "quadrature/quadrature_lib.hh"
#include "fem/fe_p.hh"
#include "fem/fe_values.hh"
#include "fem/mapping_p1.hh"
#include "la/solve.h"
#include "fem/fe_rt.hh"
#include "io/output.h"
#include "mesh/boundaries.h"
#include "la/distribution.hh"
#include "input/accessors.hh"
#include "flow/old_bcd.hh"


using namespace Input::Type;

Record TransportDG::input_type
	= Record("AdvectionDiffusion_DG", "DG solver for transport with diffusion.")
	.derive_from(TransportBase::input_type)
	.declare_key("sigma", Double(), Default("0"),
			"Coefficient of diffusive transfer through fractures.")
	.declare_key("alpha_l", Double(), Default("0"),
			"Longitudal dispersivity.")
	.declare_key("alpha_t", Double(), Default("0"),
			"Transversal dispersivity.")
	.declare_key("d_m", Double(), Default("1e-6"),
			"Molecular diffusivity.")
	.declare_key("dg_penalty", Double(0), Default("0"),
			"Penalty parameter influencing the discontinuity of the solution.")
    .declare_key("solver", Solver::input_type, Default::obligatory(),
            "Linear solver for MH problem.")
    .declare_key("bc_data", Array(TransportDG::EqData().boundary_input_type()
    		.declare_key("old_boundary_file", IT::FileName::input(),
    				"Input file with boundary conditions (obsolete).")
    		.declare_key("bc_times", Array(Double()), Default::optional(),
    				"Times for changing the boundary conditions (obsolete).")
    		), IT::Default::obligatory(), "")
    .declare_key("bulk_data", Array(TransportDG::EqData().bulk_input_type()),
    		IT::Default::obligatory(), "");


using namespace arma;

TransportDG::EqData::EqData() : TransportEqData("TransportDG")
{}

RegionSet TransportDG::EqData::read_boundary_list_item(Input::Record rec) {
	// Base method EqDataBase::read_boundary_list_item must be called first!
	RegionSet domain = EqDataBase::read_boundary_list_item(rec);
    FilePath bcd_file;
    if (rec.opt_val("old_boundary_file", bcd_file) ) {
    	Input::Iterator<Input::Array> bc_it = rec.find<Input::Array>("bc_times");
    	if (bc_it) bc_it->copy_to(bc_times);

    	if (bc_times.size() == 0) {
    		bc_time_level = -1;
    	} else {
            stringstream name_str;
            name_str << (string)bcd_file << "_" << setfill('0') << setw(3) << bc_time_level;
            bcd_file = FilePath(name_str.str(), FilePath::input_file);
            bc_time_level++;
        }
        OldBcdInput::instance()->read_transport(bcd_file, bc_conc);
    }
    return domain;
}

TransportDG::TransportDG(Mesh & init_mesh, const Input::Record &in_rec)
        : TransportBase(init_mesh, in_rec),
          advection(1e0),
          mass_matrix(0),
          tol_switch_dirichlet_neumann(1e-5)
          // TODO: this should be dependent on precision of the Flow solution
          // see also remark in BC application
{
    time_ = new TimeGovernor(in_rec.val<Input::Record>("time"), equation_mark_type_);

    time_->fix_dt_until_mark();
    
    // set up solver
    solver = new Solver;
    solver_init(solver, in_rec.val<Input::AbstractRecord>("solver"));


    /*
     * Read names of transported substances.
     */
    in_rec.val<Input::Array>("substances").copy_to(substance_names);
    n_substances = substance_names.size();



    /*
     * Set up physical parameters.
     */
    data.set_mesh(&init_mesh);
    data.init_conc.set_n_comp(n_substances);
    data.bc_conc.set_n_comp(n_substances);
    data.init_from_input( in_rec.val<Input::Array>("bulk_data"), in_rec.val<Input::Array>("bc_data") );
    data.set_time(*time_);

    sigma  = in_rec.val<double>("sigma");
    alphaL = in_rec.val<double>("alpha_l");
    alphaT = in_rec.val<double>("alpha_t");
    Dm 	   = in_rec.val<double>("d_m");


    // Set up the numerical penalty parameter
    alpha = in_rec.val<double>("dg_penalty");
    if (Dm != 0 && alpha == 0)
    	alpha = max(1e0,1e0/Dm);



    sorption = in_rec.val<bool>("sorption_enable");
    dual_porosity = in_rec.val<bool>("dual_porosity");

    // distribute DOFs
    dof_handler1d = new DOFHandler<1,3>(mesh());
    dof_handler2d = new DOFHandler<2,3>(mesh());
    dof_handler3d = new DOFHandler<3,3>(mesh());
    fe1d = new FE_P_disc<1,1,3>;
    fe2d = new FE_P_disc<1,2,3>;
    fe3d = new FE_P_disc<1,3,3>;
    dof_handler1d->distribute_dofs(*fe1d);
    dof_handler2d->distribute_dofs(*fe2d, dof_handler1d->n_global_dofs());
    dof_handler3d->distribute_dofs(*fe3d, dof_handler1d->n_global_dofs() + dof_handler2d->n_global_dofs());

    // distribute solution vectors on processors
    distr = new Distribution(Distribution::Block, dof_handler1d->n_global_dofs() + dof_handler2d->n_global_dofs() + dof_handler3d->n_global_dofs());


    // set up output class
    // TODO: Add corresponding record to the in_rec
    Input::Record output_rec = in_rec.val<Input::Record>("output");
    //transport_output = new OutputTime(mesh_, output_rec.val<Input::Record>("output_stream"));
    transport_output = OutputStream(mesh_, output_rec.val<Input::Record>("output_stream"));
    output_solution.resize(n_substances);
    for (int i=0; i<n_substances; i++)
    {
        output_solution[i] = new double[distr->size()];
        transport_output->register_corner_data<double>(substance_names[i], "M/L^3", output_solution[i], distr->size());
    }


    // set time marks for writing the output
    output_mark_type = this->mark_type() | time_->marks().type_fixed_time() | time_->marks().type_output();
    time_->marks().add_time_marks(0.0, output_rec.val<double>("save_step"), time_->end_time(), output_mark_type);
    

    ls    = new LinSys_MPIAIJ(distr->lsize());
    ls_dt = new LinSys_MPIAIJ(distr->lsize());

    // set initial conditions
    set_initial_condition();

    // save initial state
    output_data();
}


TransportDG::~TransportDG()
{
    //delete transport_output;
    delete time_;
    delete solver;
    delete ls;
    delete ls_dt;
    for (int i=0; i<n_substances; i++)
    {
    	delete[] output_solution[i];
    }

    gamma.clear();
    substance_names.clear();
}


void TransportDG::set_eq_data(Field< 3, FieldValue<3>::Scalar > *cross_section)
{
  data.cross_section = cross_section;
}



/**
 * Development notes:
 *
 * The goal is to implement a robust method (minimum of tuning parameters)
 * reliable both for pure transport (or transport dominated) regime and for
 * diffusion dominated regime.
 *
 * For dominated diffusion the DG method works well.
 *
 * For small or no diffusion, solution suffers from spurious oscillations
 * (Gibbs effect) which are to be suppressed. Possible ways out:
 * - apply TVD limiter (maybe tedious to implement, reduced accuracy in regions
 *   where the exact solution is smooth)
 * - some upwind in the volume part of advection (SUPG?)
 * - semi-implicit scheme (explicit discretization of advection + implicit discretization
 *   of diffusion)
 *
 * Currently update_solution() provides
 * - possibility to choose arbitrary theta-scheme for time discretization
 *   (theta=1 for implicit Euler, 0.5 for Crank-Nicholson, 0 for explicit Euler)
 * - simple limiter ensuring positivity of the solution
 *
 */

void TransportDG::update_solution()
{
	if (mass_matrix == NULL)
	{
	    // assemble mass matrix
	    ls_dt->start_allocation();
	    assemble_mass_matrix();
	    ls_dt->start_add_assembly();
	    assemble_mass_matrix();
	    ls_dt->finalize();
	    mass_matrix = ls_dt->get_matrix();
	}

    time_->next_time();
    time_->view();
    data.set_time(*time_);
    
    ::Mat old_stiffness_matrix = 0;
    ::Vec old_rhs, old_solution;
    bool ls_is_new = ls->is_new();
    double theta = 1;

    if (!ls_is_new) {
		MatConvert(stiffness_matrix, MATSAME, MAT_INITIAL_MATRIX, &old_stiffness_matrix);
		VecDuplicate(rhs, &old_rhs);
		VecCopy(rhs, old_rhs);
		VecDuplicate(ls->get_solution(), &old_solution);
		VecCopy(ls->get_solution(), old_solution);
    }



    // TODO: assemble system matrix only if velocity flux or boundary conditions changed
//    if (flux_changed || (bc->get_time_level() != -1 && time_->is_current(bc_mark_type_)))
//    {
        flux_changed = false;

        // check first time assembly - needs preallocation
        if (ls->is_new() ) {
            // preallocate system matrix
            ls->start_allocation();
            assemble_stiffness_matrix();
            set_boundary_conditions();
            stiffness_matrix = 0;
        }

        // new fluxes can change the location of Neumann boundary,
        // thus stiffness matrix must be reassembled
        ls->start_add_assembly();
        MatZeroEntries(ls->get_matrix());
        VecSet(ls->get_rhs(), 0);
        assemble_stiffness_matrix();
        set_boundary_conditions();
        ls->finalize();

        if (stiffness_matrix == 0)
            MatConvert(ls->get_matrix(), MATSAME, MAT_INITIAL_MATRIX, &stiffness_matrix);
        else
            MatCopy(ls->get_matrix(), stiffness_matrix, DIFFERENT_NONZERO_PATTERN);
        VecDuplicate(ls->get_rhs(), &rhs);
        VecCopy(ls->get_rhs(), rhs);
//    }

	if (ls_is_new) {
		MatConvert(ls->get_matrix(), MATSAME, MAT_INITIAL_MATRIX, &old_stiffness_matrix);
		VecDuplicate(ls->get_rhs(), &old_rhs);
		VecCopy(ls->get_rhs(), old_rhs);
		VecDuplicate(ls->get_solution(), &old_solution);
		VecCopy(ls->get_solution(), old_solution);
		MatZeroEntries(old_stiffness_matrix);
		VecSet(old_rhs, 0);
	}


    /* Apply backward Euler time integration.
     *
     * Denoting A the stiffness matrix and M the mass matrix, the algebraic system at the k-th time level reads
     *
     *   (1/dt M + A)u^k = f + 1/dt M.u^{k-1}
     *
     * Hence we modify at each time level the right hand side:
     *
     *   f^k = f + 1/dt M u^{k-1},
     *
     * where f stands for the term stemming from the force and boundary conditions.
     * Accordingly, we set
     *
     *   A^k = A + 1/dt M.
     *
     */

/*
    MatCopy(stiffness_matrix, ls->get_matrix(), DIFFERENT_NONZERO_PATTERN);
    // ls->get_matrix() = 1/dt*mass_matrix + ls->get_matrix()
    MatAXPY(ls->get_matrix(), 1./time_->dt(), mass_matrix, SUBSET_NONZERO_PATTERN);
    Vec y;
    VecDuplicate(rhs, &y);
    // y = mass_matrix*ls->get_solution()
    MatMult(mass_matrix, ls->get_solution(), y);
    // ls->get_rhs() = 1/dt*y + rhs
    VecWAXPY(ls->get_rhs(), 1./time_->dt(), y, rhs);

    //MatView( ls->get_matrix(), PETSC_VIEWER_STDOUT_SELF );

    VecDestroy(&y);
*/

    // Theta scheme
    // (M + dt*theta*A^k)u^k = dt*theta*f^k + dt*(1-theta)f^{k-1} + (M + dt*(theta-1)*A^{k-1})u^{k-1}
    MatCopy(mass_matrix, ls->get_matrix(), DIFFERENT_NONZERO_PATTERN);
    MatAXPY(ls->get_matrix(), time_->dt()*theta, stiffness_matrix, DIFFERENT_NONZERO_PATTERN);

    ::Mat tmp_mat = 0;
    MatConvert(mass_matrix, MATSAME, MAT_INITIAL_MATRIX, &tmp_mat);
    MatAXPY(tmp_mat, time_->dt()*(theta-1.), old_stiffness_matrix, DIFFERENT_NONZERO_PATTERN);
    MatMult(tmp_mat, old_solution, ls->get_rhs());
    VecAXPBYPCZ(ls->get_rhs(), time_->dt()*theta, time_->dt()*(1.-theta), 1., rhs, old_rhs);
    VecDestroy(&old_rhs);
    VecDestroy(&old_solution);
    MatDestroy(&tmp_mat);
    MatDestroy(&old_stiffness_matrix);

    //VecView( ls->get_rhs(), PETSC_VIEWER_STDOUT_SELF );
    // solve
    solve_system(solver, ls);

    // postprocessing for positivity preserving:
    unsigned int dof_indices[4], ndofs;
    double *sol = ls->get_solution_array();
    for (ElementFullIter cell = mesh_->element.begin(); cell != mesh_->element.end(); ++cell)
    {
        switch (cell->dim()) {
        case 1:
        	dof_handler1d->get_dof_indices(cell, dof_indices);
        	ndofs = dof_handler1d->n_local_dofs();
        	break;
        case 2:
        	dof_handler2d->get_dof_indices(cell, dof_indices);
        	ndofs = dof_handler2d->n_local_dofs();
        	break;
        case 3:
        	dof_handler3d->get_dof_indices(cell, dof_indices);
        	ndofs = dof_handler3d->n_local_dofs();
        	break;
        }

        double avg = 0, vmin = std::numeric_limits<double>::infinity(), vmax = -vmin, val, factor;

        for (int i=0; i<ndofs; i++)
        {
        	val = sol[dof_indices[i]];
        	avg += val;
        	if (val > vmax) vmax = val;
        	if (val < vmin) vmin = val;
        }

        avg /= ndofs;

        if (avg-vmin<1e-15)
        	factor = 1;
        else
        	factor =
        			min(
        			(max(vmin, 0.)-avg)/(vmin - avg)
        			,
        			(min(vmax,1.)-avg)/(vmax-avg)
        			)
        			;

        for (int i=0; i<ndofs; i++)
        {
        	sol[dof_indices[i]] = avg + (sol[dof_indices[i]]-avg)*factor;
        }
    }


    //VecView( ls->get_solution(), PETSC_VIEWER_STDOUT_SELF );
}




void TransportDG::get_solution_vector(double *& vector, unsigned int & size)
{
    vector = ls->get_solution_array();
    size   = ls->vec_lsize();
}



void TransportDG::get_parallel_solution_vector(Vec & vector)
{
    vector = ls->get_solution();
}



void TransportDG::set_velocity_field(const MH_DofHandler &dh)
{
    // So far the velocity_vector contains zeros, so we ignore it.
    // Instead we use the value Side.flux.
//	flux_vector = velocity_vector;

    mh_dh = &dh;
	flux_changed = true;

}



void TransportDG::output_data()
{
    double *solution;
    unsigned int dof_indices[max(fe1d->n_dofs(), max(fe2d->n_dofs(), fe3d->n_dofs()))];
    int n_nodes = mesh_->node_vector.size();
    int count[n_nodes];

    if (!time_->is_current(output_mark_type)) return;


    // gather the solution from all processors
    IS is;
	VecScatter output_scatter;
	int row_ids[distr->size()];
	Vec solution_vec[n_substances];

	for (int i=0; i<n_substances; i++)
		VecCreateSeq(PETSC_COMM_SELF, ls->size(), &solution_vec[i]);

	for (int i=0; i<distr->size(); i++)
		row_ids[i] = i;

	ISCreateGeneral(PETSC_COMM_SELF, ls->size(), row_ids, PETSC_COPY_VALUES, &is); //WithArray
	VecScatterCreate(ls->get_solution(), is, solution_vec[0], PETSC_NULL, &output_scatter);
	for (int sbi = 0; sbi < n_substances; sbi++)
	{
		VecScatterBegin(output_scatter, ls->get_solution(), solution_vec[sbi], INSERT_VALUES, SCATTER_FORWARD);
		VecScatterEnd(output_scatter, ls->get_solution(), solution_vec[sbi], INSERT_VALUES, SCATTER_FORWARD);
	}
	VecScatterDestroy(&(output_scatter));
	ISDestroy(&(is));

	// on the main processor fill the output array and save to file
    if (distr->myp() == 0)
    {
    	int id = 0, nid;

    	VecGetArray(solution_vec[0], &solution);

    	FOR_ELEMENTS(mesh_, elem)
		{
    		switch (elem->dim())
    		{
    		case 1:
    			dof_handler1d->get_dof_indices(elem, dof_indices);
    			break;
    		case 2:
    			dof_handler2d->get_dof_indices(elem, dof_indices);
    			break;
    		case 3:
    			dof_handler3d->get_dof_indices(elem, dof_indices);
    			break;
    		default:
    			break;
    		}
    		FOR_ELEMENT_NODES(elem, nid)
    		{
    			output_solution[0][id] = solution[dof_indices[nid]];
    			id++;
    		}
		}

		transport_output->write_data(time_->t());
    }

    for (int i=0; i<n_substances; i++)
    	VecDestroy(&solution_vec[i]);
}


void TransportDG::assemble_mass_matrix()
{
	assemble_mass_matrix(dof_handler1d, fe1d);
	assemble_mass_matrix(dof_handler2d, fe2d);
	assemble_mass_matrix(dof_handler3d, fe3d);
}


template<unsigned int dim>
void TransportDG::assemble_mass_matrix(DOFHandler<dim,3> *dh, FiniteElement<dim,3> *fe)
{
    MappingP1<dim,3> map;
    QGauss<dim> q(2);
    FEValues<dim,3> fe_values(map, q, *fe, update_values | update_JxW_values);
    unsigned int ndofs = fe->n_dofs();
    unsigned int dof_indices[ndofs];
    PetscScalar local_mass_matrix[ndofs*ndofs], local_rhs[ndofs];

    typename DOFHandler<dim,3>::CellIterator cell = dh->begin_cell();

    // assemble integral over elements
    for (cell = dh->begin_cell(); cell != dh->end_cell(); ++cell)
    {
        if (cell->dim() != dim) continue;

        fe_values.reinit(cell);

        dh->get_dof_indices(cell, dof_indices);

        arma::vec3 p = cell->centre();
        ElementAccessor<3> ele_acc = cell->element_accessor();

        // cross-section is assumed constant on element
        double elem_csec = data.cross_section->value(p, ele_acc);

        // assemble the local stiffness and mass matrix
        for (int i=0; i<ndofs; i++)
        {
        	local_rhs[i] = 0;
            for (int j=0; j<ndofs; j++)
            {
                local_mass_matrix[i*ndofs+j] = 0;
                if (dof_indices[i] < distr->begin() || dof_indices[i] > distr->end()) continue;
                for (int k=0; k<q.size(); k++)
                    local_mass_matrix[i*ndofs+j] += elem_csec*fe_values.shape_value(j,k)*fe_values.shape_value(i,k)*fe_values.JxW(k);
            }
        }

        ls_dt->set_values(ndofs, (int *)dof_indices, ndofs, (int *)dof_indices, local_mass_matrix, local_rhs);
    }
}




void TransportDG::assemble_stiffness_matrix()
{
	assemble_volume_integrals<1>(dof_handler1d, fe1d);
	assemble_fluxes_boundary<1>(dof_handler1d, 0, fe1d, 0);
	assemble_fluxes_element_element<1>(dof_handler1d, 0, fe1d, 0);
	assemble_fluxes_element_side<1>(dof_handler1d, 0, fe1d, 0);

	assemble_volume_integrals<2>(dof_handler2d, fe2d);
	assemble_fluxes_boundary<2>(dof_handler2d, dof_handler1d, fe2d, fe1d);
	assemble_fluxes_element_element<2>(dof_handler2d, dof_handler1d, fe2d, fe1d);
	assemble_fluxes_element_side<2>(dof_handler2d, dof_handler1d, fe2d, fe1d);

	assemble_volume_integrals<3>(dof_handler3d, fe3d);
    assemble_fluxes_boundary<3>(dof_handler3d, dof_handler2d, fe3d, fe2d);
    assemble_fluxes_element_element<3>(dof_handler3d, dof_handler2d, fe3d, fe2d);
    assemble_fluxes_element_side<3>(dof_handler3d, dof_handler2d, fe3d, fe2d);
}



template<unsigned int dim>
void TransportDG::assemble_volume_integrals(DOFHandler<dim,3> *dh, FiniteElement<dim,3> *fe)
{
    MappingP1<dim,3> map;
    QGauss<dim> q(2);
    FE_RT0<dim,3> fe_rt;
    FEValues<dim,3> fv_rt(map, q, fe_rt, update_values | update_gradients);
    FEValues<dim,3> fe_values(map, q, *fe, update_values | update_gradients | update_JxW_values | update_quadrature_points);
    typename DOFHandler<dim,3>::CellIterator cell = dh->begin_cell();
    vector<mat33> K;
    vector<vec3> velocity;
    vector<double> divergence;
    const unsigned int ndofs = fe->n_dofs();
    unsigned int dof_indices[ndofs];
    PetscScalar local_matrix[ndofs*ndofs];

	// assemble integral over elements
    for (cell = dh->begin_cell(); cell != dh->end_cell(); ++cell)
    {
        if (cell->dim() != dim) continue;

        fe_values.reinit(cell);
        fv_rt.reinit(cell);
        
        calculate_velocity(cell, velocity, fv_rt);
        calculate_dispersivity_tensor(K, velocity);
        calculate_velocity_divergence(cell, divergence, fv_rt);

        double por_m = data.por_m.value(cell->centre(), cell->element_accessor());

        dh->get_dof_indices(cell, dof_indices);

        // assemble the local stiffness matrix
        for (int i=0; i<ndofs; i++)
        {
            for (int j=0; j<ndofs; j++)
            {
                local_matrix[i*ndofs+j] = 0;
                if (dof_indices[i] < distr->begin() || dof_indices[i] > distr->end()) continue;
                for (int k=0; k<q.size(); k++)
                {
                    local_matrix[i*ndofs+j] += (dot(K[k]*fe_values.shape_grad(j,k),fe_values.shape_grad(i,k))
                                               +dot(fe_values.shape_grad(j,k),velocity[k])*fe_values.shape_value(i,k)*advection/por_m
//                                               +divergence[k]*fe_values.shape_value(j,k)*fe_values.shape_value(i,k)*advection/por_m
                                               )*fe_values.JxW(k);
                }

            }
        }

        ls->mat_set_values(ndofs, (int *)dof_indices, ndofs, (int *)dof_indices, local_matrix);
    }
}




template<unsigned int dim>
void TransportDG::assemble_fluxes_element_element(DOFHandler<dim,3> *dh, DOFHandler<dim-1,3> *dh_sub, FiniteElement<dim,3> *fe, FiniteElement<dim-1,3> *fe_sub)
{
    MappingP1<dim,3> map;
    QGauss<dim-1> side_q(2);
    FE_RT0<dim,3> fe_rt;
    vector<FESideValues<dim,3>*> fe_values_side;
    FESideValues<dim,3> fsv_rt(map, side_q, fe_rt, update_values | update_normal_vectors | update_side_JxW_values);
    vector<FEValuesSpaceBase<3>*> fv_sb;
    typename DOFHandler<dim,3>::CellIterator cell = dh->begin_cell();
    const unsigned int ndofs = fe->n_dofs();
    vector<unsigned int*> side_dof_indices;
    PetscScalar local_matrix[ndofs*ndofs], local_rhs[ndofs];
    vector< vector<mat33> > side_K;
    vector< vector<vec3> > side_velocity;
    double gamma_l, omega[2], transport_flux;

    gamma.resize(mesh_->boundary_.size());

    // assemble integral over sides
    FOR_EDGES( mesh_, edg )
    {
        // We have to skip edges of wrong dimension
    	//ASSERT_SIZES(edg->side(0)->element()->dim(), dim);

        if (edg->n_sides < 2 || edg->side(0)->element()->dim() != dim) continue;

        fv_sb.resize(edg->n_sides);
        side_K.resize(edg->n_sides);
        side_velocity.resize(edg->n_sides);

        if (side_dof_indices.size() < edg->n_sides)
            for (int i=side_dof_indices.size(); i<edg->n_sides; i++)
                side_dof_indices.push_back(new unsigned int[ndofs]);

        if (fe_values_side.size() < edg->n_sides)
            for (int sid=fe_values_side.size(); sid<edg->n_sides; sid++)
                fe_values_side.push_back(new FESideValues<dim,3>(map, side_q, *fe, update_values | update_gradients | update_side_JxW_values | update_normal_vectors));

		for (int sid=0; sid<edg->n_sides; sid++)
		{
			cell = mesh_->element.full_iter(edg->side(sid)->element());
			dh->get_dof_indices(cell, side_dof_indices[sid]);
			fe_values_side[sid]->reinit(cell, edg->side(0));
			fsv_rt.reinit(cell, edg->side(0));
			calculate_velocity(cell, side_velocity[sid], fsv_rt);
			calculate_dispersivity_tensor(side_K[sid], side_velocity[sid]);
			fv_sb[sid] = fe_values_side[sid];
		}


        // fluxes and penalty
		for (int s1=0; s1<edg->n_sides; s1++)
		{
			for (int s2=s1+1; s2<edg->n_sides; s2++)
			{
				// vec3 nv = ( ! edg->side(s1)->valid() )?(-fv_sb[s2]->normal_vector(0)):fv_sb[s1]->normal_vector(0);
				ASSERT(edg->side(s1)->valid(), "Invalid side of edge.");
				vec3 nv = fv_sb[s1]->normal_vector(0);

				// set up the parameters for DG method
				set_DG_parameters(&(*edg), s1, s2, side_q.size(), side_K, -fv_sb[1]->normal_vector(0), alpha, advection, gamma_l, omega, transport_flux);

				int sd[2];
				sd[0] = s1;
				sd[1] = s2;

				// For selected pair of elements:
				for (int m=0; m<2; m++)
				{
					Element *elem = edg->side(sd[m])->element();
					double por_m = data.por_m.value(elem->centre(), elem->element_accessor());
					for (int n=0; n<2; n++)
					{
						for (int i=0; i<fv_sb[sd[n]]->n_dofs(); i++)
						{
							for (int j=0; j<fv_sb[sd[m]]->n_dofs(); j++)
							{
								int index = i*fv_sb[sd[m]]->n_dofs()+j;
								local_matrix[index] = 0;
								if (side_dof_indices[sd[n]][i] < distr->begin() || side_dof_indices[sd[n]][i] > distr->end()) continue;
								for (int k=0; k<side_q.size(); k++)
								{
									// flux due to transport (applied on interior edges) (average times jump)
									local_matrix[index] -= (m==0?1:-1)*advection / por_m
									                       *0.5*transport_flux
									                       *fv_sb[sd[m]]->shape_value(j,k)*fv_sb[sd[n]]->shape_value(i,k)*fv_sb[0]->JxW(k);

									// penalty enforcing continuity across edges (applied on interior and Dirichlet edges) (jump times jump)
									local_matrix[index] += (m==n?1:-1)*gamma_l*fv_sb[sd[m]]->shape_value(j,k)*fv_sb[sd[n]]->shape_value(i,k)*fv_sb[0]->JxW(k);

									// terms due to diffusion
									local_matrix[index] += ((n==0)?-1.:1.)*dot(omega[0]*side_K[sd[m]][k]*fv_sb[sd[m]]->shape_grad(j,k),nv)*fv_sb[sd[n]]->shape_value(i,k)*fv_sb[0]->JxW(k);
									local_matrix[index] += ((m==0)?-1.:1.)*dot(omega[1]*side_K[sd[n]][k]*fv_sb[sd[n]]->shape_grad(i,k),nv)*fv_sb[sd[m]]->shape_value(j,k)*fv_sb[0]->JxW(k);
								}
							}
						}
						ls->mat_set_values(fv_sb[sd[n]]->n_dofs(), (int *)side_dof_indices[sd[n]], fv_sb[sd[m]]->n_dofs(), (int *)side_dof_indices[sd[m]], local_matrix);
					}
				}

			}
		}
    }

    for (int i=0; i<fe_values_side.size(); i++)
        delete fe_values_side[i];

    for (int i=0; i<side_dof_indices.size(); i++)
        delete[] side_dof_indices[i];
}


template<unsigned int dim>
void TransportDG::assemble_fluxes_boundary(DOFHandler<dim,3> *dh, DOFHandler<dim-1,3> *dh_sub, FiniteElement<dim,3> *fe, FiniteElement<dim-1,3> *fe_sub)
{
    MappingP1<dim,3> map;
    MappingP1<dim-1,3> map_vb;
    QGauss<dim-1> side_q(2);
    FE_RT0<dim,3> fe_rt;
    FESideValues<dim,3> fe_values_side(map, side_q, *fe, update_values | update_gradients | update_side_JxW_values | update_normal_vectors);
    FESideValues<dim,3> fsv_rt(map, side_q, fe_rt, update_values | update_normal_vectors | update_side_JxW_values);
    typename DOFHandler<dim,3>::CellIterator cell = dh->begin_cell();
    const unsigned int ndofs = fe->n_dofs();
    unsigned int side_dof_indices[ndofs];
    PetscScalar local_matrix[ndofs*ndofs], local_rhs[ndofs];
    vector< vector<mat33> > side_K;
    vector< vector<vec3> > side_velocity;
    double gamma_l, omega[2], transport_flux;

    gamma.resize(mesh_->boundary_.size());

    // assemble boundary integral
    FOR_EDGES(mesh_, edge)
    {
        if (edge->n_sides != 1 || edge->side(0)->dim() != dim-1) continue;

        double elem_flux = 0;
        for (int i=0; i<edge->side(0)->element()->n_sides(); i++)
        	elem_flux += fabs( mh_dh->side_flux( *(edge->side(0)->element()->side(i)) ) );

        // skip Neumann boundaries
        // Constant 1e-6 stabilizes switching Dirichlet to Neumann boundary condition.
        // TODO: Define as a constant. Better determination of its magnitude. See also other places with same constant.
        // ?? set the constant from precision of the flow solver ??
        // stil there are elements with  elme_flux and BC flux in order 1e-7 ~ tolerance of linear solver
        // then this condition doesn't work, we either has to set zero water fluxes or make this dependent on tolerance in flow solver
        // temporary solution: check zero Neuman BC in water
        //
        // Solution:
        // 1) postprocessing of flow solution, set flux DOFs to zero on Neuman boundaries (DO NOT HELP - e.g. in case
        //    of Dirichlet boundary condition but flow parallel to the boundary
        // 2) modify following condition to select Neuman BC in transport if flux is smaller then tolerance of lin. solver * some const,
        //    or matrix diagonal (think carefully)
        //
        if (edge->side(0)->cond() == 0 || mh_dh->side_flux( *(edge->side(0)) ) >= -tol_switch_dirichlet_neumann*elem_flux) continue;
                // || (edge->side(0)->cond()->type == 2 /* Neuman*/ && edge->side(0)->cond()->flux == 0.0) )
                // NEED FIX

        side_K.resize(edge->n_sides);
        side_velocity.resize(edge->n_sides);

        cell = mesh().element.full_iter(edge->side(0)->element());
        dh->get_dof_indices(cell, side_dof_indices);
        fe_values_side.reinit(cell, edge->side(0));
        fsv_rt.reinit(cell, edge->side(0));

        calculate_velocity(cell, side_velocity[0], fsv_rt);
        calculate_dispersivity_tensor(side_K[0], side_velocity[0]);

        // set up the parameters for DG method
        set_DG_parameters_edge(&(*edge), side_q.size(), side_K, fe_values_side.normal_vector(0), alpha, advection, gamma_l, omega);
        if (edge->side(0)->cond() != 0) {
            gamma[edge->side(0)->cond_idx()] = gamma_l;
        }

        // fluxes and penalty
        for (int i=0; i<ndofs; i++)
        {
            for (int j=0; j<ndofs; j++)
            {
                local_matrix[i*ndofs+j] = 0;
                if (side_dof_indices[i] < distr->begin() || side_dof_indices[i] > distr->end()) continue;
                for (int k=0; k<side_q.size(); k++)
                {
                    // penalty enforcing continuity across edges (applied on interior and Dirichlet edges)
                    local_matrix[i*ndofs+j] += 2.*gamma_l*fe_values_side.shape_value(j,k)*fe_values_side.shape_value(i,k)*fe_values_side.JxW(k);
                }
            }
        }
        ls->mat_set_values(ndofs, (int *)side_dof_indices, ndofs, (int *)side_dof_indices, local_matrix);
    }
}



template<unsigned int dim>
void TransportDG::assemble_fluxes_element_side(DOFHandler<dim,3> *dh, DOFHandler<dim-1,3> *dh_sub, FiniteElement<dim,3> *fe, FiniteElement<dim-1,3> *fe_sub)
{
    MappingP1<dim,3> map;
    MappingP1<dim-1,3> map_vb;
    QGauss<dim-1> side_q(2);
    FEValues<dim-1,3> *fe_values_vb;
    vector<FESideValues<dim,3>*> fe_values_side;
    vector<FEValuesSpaceBase<3>*> fv_sb;
    typename DOFHandler<dim,3>::CellIterator cell = dh->begin_cell();
    const unsigned int ndofs = fe->n_dofs();
    vector<unsigned int*> side_dof_indices;
    PetscScalar local_matrix[ndofs*ndofs], local_rhs[ndofs];
    double transport_flux;

    if (dim > 1)
        fe_values_vb = new FEValues<dim-1,3>(map_vb, side_q, *fe_sub, update_values | update_gradients | update_JxW_values | update_quadrature_points);

    // assemble integral over sides
    FOR_NEIGHBOURS(mesh_ , nb)
    {
        // skip neighbours of different dimension
        // ASSERT(nb->element()->dim() == dim-1, "Element and side dimension mismatch.");
        if (nb->element()->dim() != dim-1) continue;

        int nb_sides = 2;   // = nb->n_sides;
        fv_sb.resize(nb_sides);

        if (side_dof_indices.size() < nb_sides)
            for (int i=side_dof_indices.size(); i<nb_sides; i++)
                side_dof_indices.push_back(new unsigned int[ndofs]);

        if (fe_values_side.size() < nb_sides)
            for (int sid=fe_values_side.size(); sid<nb_sides; sid++)
                fe_values_side.push_back(new FESideValues<dim,3>(map, side_q, *fe, update_values | update_gradients | update_side_JxW_values | update_normal_vectors));

		typename DOFHandler<dim-1,3>::CellIterator cell_sub = mesh_->element.full_iter(nb->element());
		dh_sub->get_dof_indices(cell_sub, side_dof_indices[0]);
		fe_values_vb->reinit(cell_sub);
		fv_sb[0] = fe_values_vb;

		cell = nb->side()->element();
		dh->get_dof_indices(cell, side_dof_indices[1]);
		fe_values_side[1]->reinit(cell, nb->side());
		fv_sb[1] = fe_values_side[1];


		// flux from the higher dimension to the lower one
		transport_flux = mh_dh->side_flux( *(nb->side()) );

		double por_m = data.por_m.value(nb->element()->centre(), nb->element()->element_accessor());

		// set transmission condition for dim-1
		for (int j=0; j<fv_sb[0]->n_dofs(); j++)
		{
			for (int i=0; i<fv_sb[0]->n_dofs(); i++)
			{
				int index = i*fv_sb[0]->n_dofs() + j;
				local_matrix[index] = 0;
				if (side_dof_indices[0][i] < distr->begin() || side_dof_indices[0][i] > distr->end()) continue;
				for (int k=0; k<side_q.size(); k++)
					local_matrix[index] += advection/por_m*(sigma+fabs(transport_flux))
					                       *fv_sb[0]->shape_value(j,k)*fv_sb[0]->shape_value(i,k)*fv_sb[0]->JxW(k);
			}
		}
		ls->mat_set_values(fv_sb[0]->n_dofs(), (int *)side_dof_indices[0], fv_sb[0]->n_dofs(), (int *)side_dof_indices[0], local_matrix);
		for (int j=0; j<fv_sb[1]->n_dofs(); j++)
		{
			for (int i=0; i<fv_sb[0]->n_dofs(); i++)
			{
				int index = i*fv_sb[1]->n_dofs() + j;
				local_matrix[index] = 0;
				if (side_dof_indices[0][i] < distr->begin() || side_dof_indices[0][i] > distr->end()) continue;
				for (int k=0; k<side_q.size(); k++)
					local_matrix[index] -= advection/por_m
					              *(sigma+fabs(transport_flux))*fv_sb[1]->shape_value(j,k)*fv_sb[0]->shape_value(i,k)*fv_sb[0]->JxW(k);
			}
		}
		ls->mat_set_values(fv_sb[0]->n_dofs(), (int *)side_dof_indices[0], fv_sb[1]->n_dofs(), (int *)side_dof_indices[1], local_matrix);

		// set transmission condition for dim
		for (int j=0; j<fv_sb[0]->n_dofs(); j++)
		{
			for (int i=0; i<fv_sb[1]->n_dofs(); i++)
			{
				int index = i*fv_sb[0]->n_dofs() + j;
				local_matrix[index] = 0;
				if (side_dof_indices[1][i] < distr->begin() || side_dof_indices[1][i] > distr->end()) continue;
				for (int k=0; k<side_q.size(); k++)
					local_matrix[index] += advection/por_m
					          *(-sigma+min(0.,transport_flux))*fv_sb[0]->shape_value(j,k)*fv_sb[1]->shape_value(i,k)*fv_sb[0]->JxW(k);
			}
		}
		ls->mat_set_values(fv_sb[1]->n_dofs(), (int *)side_dof_indices[1], fv_sb[0]->n_dofs(), (int *)side_dof_indices[0], local_matrix);
		for (int j=0; j<fv_sb[1]->n_dofs(); j++)
		{
			for (int i=0; i<fv_sb[1]->n_dofs(); i++)
			{
				int index = i*fv_sb[1]->n_dofs() + j;
				local_matrix[index] = 0;
				if (side_dof_indices[1][i] < distr->begin() || side_dof_indices[1][i] > distr->end()) continue;
				for (int k=0; k<side_q.size(); k++)
					local_matrix[index] -= advection/por_m
					*(-sigma+min(0.,transport_flux))*fv_sb[1]->shape_value(j,k)*fv_sb[1]->shape_value(i,k)*fv_sb[0]->JxW(k);
			}
		}
		ls->mat_set_values(fv_sb[1]->n_dofs(), (int *)side_dof_indices[1], fv_sb[1]->n_dofs(), (int *)side_dof_indices[1], local_matrix);
    }

    for (int i=0; i<fe_values_side.size(); i++)
        delete fe_values_side[i];

    for (int i=0; i<side_dof_indices.size(); i++)
        delete[] side_dof_indices[i];
}



void TransportDG::set_boundary_conditions()
{
	set_boundary_conditions(dof_handler1d, fe1d);
	set_boundary_conditions(dof_handler2d, fe2d);
	set_boundary_conditions(dof_handler3d, fe3d);
}


template<unsigned int dim>
void TransportDG::set_boundary_conditions(DOFHandler<dim,3> *dh, FiniteElement<dim,3> *fe)
{
    typename DOFHandler<dim,3>::CellIterator cell = dh->begin_cell();
    MappingP1<dim,3> map;
    QGauss<dim-1> side_q(2);
    FESideValues<dim,3> fe_values_side(map, side_q, *fe, update_values | update_side_JxW_values | update_quadrature_points);
    unsigned int side_dof_indices[fe->n_dofs()];
    double local_rhs[fe->n_dofs()];
    int rank;

    //TODO: distribution of BC elements
    // BC are now set only by process 0
    MPI_Comm_rank(PETSC_COMM_WORLD, &rank);
    if (rank != 0) return;

    for (unsigned int ib = 0; ib < mesh_->boundary_.size(); ib++)
    {
//    	if (ib < bc->distribution()->begin() || ib > bc->distribution()->end()) continue;

    	vector<Boundary>::iterator b=mesh_->boundary_.begin()+ib;
        ElementAccessor<3> ele_acc = b->element_accessor();
        cell = mesh_->element.full_iter(b->side()->element());

        if (cell->dim()!= dim) continue;

        // skip Neumann boundaries
        double elem_flux = 0;
        for (int i=0; i<b->side()->element()->n_sides(); i++)
        	elem_flux += fabs( mh_dh->side_flux( *(b->side()->element()->side(i)) ) );
        if (mh_dh->side_flux( *(b->side()) ) >= -tol_switch_dirichlet_neumann*elem_flux) continue;
                // || (b->type == 2 /* Neuman*/ && b->flux == 0.0) ) continue;
                // NEED FIX

        fe_values_side.reinit(cell, b->side());
        dh->get_dof_indices(cell, side_dof_indices);

        for (int i=0; i<fe->n_dofs(); i++)
        {
            local_rhs[i] = 0;
            for (int k=0; k<side_q.size(); k++)
            {
            	double value = data.bc_conc.value(fe_values_side.point(k), ele_acc)(0);
                local_rhs[i] += (
                        +2.*gamma[ib]*value
//                        +advection*min(mh_dh->side_flux(*(b->side())), 0.)*value
                                )*fe_values_side.shape_value(i,k)*fe_values_side.JxW(k);
            }
        }
        ls->rhs_set_values(fe->n_dofs(), (int *)side_dof_indices, local_rhs);
    }
}



template<unsigned int dim>
void TransportDG::calculate_velocity(typename DOFHandler<dim,3>::CellIterator cell, vector<vec3> &velocity, FEValuesBase<dim,3> &fv)
{
    std::map<const Node*, int> node_nums;
    for (int i=0; i<cell->n_nodes(); i++)
        node_nums[cell->node[i]] = i;

    velocity.resize(fv.n_points());

    for (int k=0; k<fv.n_points(); k++)
    {
        velocity[k].zeros();
        for (int sid=0; sid<cell->n_sides(); sid++)
        {
            if (cell->side(sid)->dim() != dim-1) continue;
            int num = dim*(dim+1)/2;
            for (int i=0; i<cell->side(sid)->n_nodes(); i++)
                num -= node_nums[cell->side(sid)->node(i)];
            velocity[k] += fv.shape_vector(num,k) * mh_dh->side_flux( *(cell->side(sid)) );
        }
    }
}


template<unsigned int dim>
void TransportDG::calculate_velocity_divergence(typename DOFHandler<dim,3>::CellIterator cell, vector<double> &divergence, FEValuesBase<dim,3> &fv)
{
    std::map<const Node*, int> node_nums;
    for (int i=0; i<cell->n_nodes(); i++)
        node_nums[cell->node[i]] = i;

    divergence.resize(fv.n_points());

    for (int k=0; k<fv.n_points(); k++)
    {
        divergence[k] = 0;
        for (int sid=0; sid<cell->n_sides(); sid++)
        {
            if (cell->side(sid)->dim() != dim-1) continue;
            int num = dim*(dim+1)/2;
            for (int i=0; i<cell->side(sid)->n_nodes(); i++)
                num -= node_nums[cell->side(sid)->node(i)];
            divergence[k] += trace(fv.shape_grad_vector(num,k)) * mh_dh->side_flux( *(cell->side(sid)) );
        }
    }
}




void TransportDG::calculate_dispersivity_tensor(vector<mat33> &K, vector<vec3> &velocity)
{
    double vnorm;

    K.resize(velocity.size());

    for (int k=0; k<velocity.size(); k++)
    {
        vnorm = norm(velocity[k], 2);

        if (fabs(vnorm)>1e-6)
            for (int i=0; i<3; i++)
                for (int j=0; j<3; j++)
                    K[k](i,j) = velocity[k][i]*velocity[k][j]/vnorm*(alphaL-alphaT);
        else
            K[k].zeros();

        K[k] += eye(3,3)*(Dm + alphaT*vnorm);
    }
}


void TransportDG::set_DG_parameters(const Edge *edg,
            const int s1,
            const int s2,
            const unsigned int n_points,
            const vector< vector<mat33> > &K,
            const vec3 &normal_vector,
            const double alpha,
            const double advection,
            double &gamma,
            double *omega,
            double &transport_flux)
{
    double delta[2];
    double h = 0;
    double fluxes[edg->n_sides];

    ASSERT(edg->side(s1)->valid(), "Invalid side of an edge.");
    SideIter s = edg->side(s1);

    // calculate the side diameter
    if (s->dim() == 0)
    {
        h = 1;
    }
    else
    {
        for (int i=0; i<s->n_nodes(); i++)
            for (int j=i+1; j<s->n_nodes(); j++)
                h = max(h, s->node(i)->distance(*s->node(j)));
    }

    // calculate the total in- and out-flux through the edge
    for (int i=0; i<edg->n_sides; i++) fluxes[i] = mh_dh->side_flux( *(edg->side(i)) );
    double pflux = 0, nflux = 0;
    for (int i=0; i<edg->n_sides; i++)
    {
        if (fluxes[i] > 0)
            pflux += fluxes[i];
        else
            nflux += fluxes[i];
    }

    // calculate the flux from s1 to s2
    if (fluxes[s2] > 0 && fluxes[s1] < 0 && s1 < s2)
    {
        transport_flux = fluxes[s1]*fabs(fluxes[s2]/pflux);
    }
    else if (fluxes[s2] < 0 && fluxes[s1] > 0 && s1 < s2)
    {
        transport_flux = fluxes[s1]*fabs(fluxes[s2]/nflux);
    }
    else if (s1==s2)
    {
        transport_flux = fluxes[s1];
    }
    else
    {
        transport_flux = 0;
    }

    gamma = 0.5*advection*fabs(transport_flux);

    if (s1 == s2)
    {
        omega[0] = 1;

        // delta is set to the average value of Kn.n on the side
        delta[0] = 0;
        for (int k=0; k<n_points; k++)
            delta[0] += dot(K[s1][k]*normal_vector,normal_vector);
        delta[0] /= n_points;

        gamma += alpha/h*delta[0];
    }
    else
    {
        delta[0] = 0;
        delta[1] = 0;
        for (int k=0; k<n_points; k++)
        {
            delta[0] += dot(K[s1][k]*normal_vector,normal_vector)/n_points;
            delta[1] += dot(K[s2][k]*normal_vector,normal_vector)/n_points;
        }

        double delta_sum = delta[0] + delta[1];

        if (delta_sum > 1e-15)
        {
            omega[0] = delta[1]/delta_sum;
            omega[1] = delta[0]/delta_sum;
            gamma += alpha*delta[0]*delta[1]/(delta_sum*h);
        }
        else
            for (int i=0; i<2; i++) omega[i] = 0;
    }
}







void TransportDG::set_DG_parameters_edge(const Edge *edge,
            const unsigned int n_points,
            const vector< vector<mat33> > &K,
            const vec3 &normal_vector,
            const double alpha,
            const double advection,
            double &gamma,
            double *omega)
{
    double delta[2];
    double h = 0;
    SideIter side = edge->side(0);

    // calculate the side diameter
    if (side->dim() == 0)
    {
        h = 1;
    }
    else
    {
        for (int i=0; i<side->n_nodes(); i++)
            for (int j=i+1; j<side->n_nodes(); j++)
                h = max(h, side->node(i)->distance( *side->node(j) ));
    }

    gamma = 0.5*advection*fabs( mh_dh->side_flux( *(side) ) );

	omega[0] = 1;

	// delta is set to the average value of Kn.n on the side
	delta[0] = 0;
	for (int k=0; k<n_points; k++)
		delta[0] += dot(K[0][k]*normal_vector,normal_vector);
	delta[0] /= n_points;

	gamma += alpha/h*delta[0];
}















void TransportDG::set_initial_condition()
{
    unsigned int maxndofs = max(fe1d->n_dofs(), max(fe2d->n_dofs(), fe3d->n_dofs()));
    unsigned int dof_indices[maxndofs], nid, ndofs;
    double values[maxndofs];

    FOR_ELEMENTS(mesh_, elem)
    {
    	ElementAccessor<3> ele_acc = mesh_->element_accessor(elem.index());

		switch (elem->dim())
		{
		case 1:
			dof_handler1d->get_dof_indices(elem, dof_indices);
			ndofs = fe1d->n_dofs();
			break;
		case 2:
			dof_handler2d->get_dof_indices(elem, dof_indices);
			ndofs = fe2d->n_dofs();
			break;
		case 3:
			dof_handler3d->get_dof_indices(elem, dof_indices);
			ndofs = fe3d->n_dofs();
			break;
		default:
			break;
		}
		double value = data.init_conc.value(elem->centre(), ele_acc)(0);
		FOR_ELEMENT_NODES(elem, nid)
		{
			values[nid] = value;
		}
		VecSetValues(ls->get_solution(), ndofs, (int *)dof_indices, values, INSERT_VALUES);
    }

    VecAssemblyBegin(ls->get_solution());
    VecAssemblyEnd(ls->get_solution());
}





