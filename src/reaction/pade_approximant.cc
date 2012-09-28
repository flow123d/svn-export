#include <iostream>
#include <cstring>
#include <stdlib.h>
#include <math.h>
#include "reaction/reaction.hh"
#include "reaction/linear_reaction.hh"
#include "reaction/pade_approximant.hh"
#include "system/system.hh"
#include "materials.hh"
#include "transport/transport.h"
//#include "system/par_distribution.hh"
#include "la/distribution.hh"
#include "mesh/mesh.h"

Input::Type::Record & Pade_approximant::get_one_decay_substep()
{
	using namespace Input::Type;
	static Record rec("Substep", "Equation for reading information about radioactive decays.");

	if(!rec.is_finished()){
		rec.declare_key("parent", String(), Default::obligatory(),
				"Identifier of an isotope.");
        rec.declare_key("half_life", Double(), Default::optional(),
                "Half life of the parent substance.");
        rec.declare_key("kinetic", Double(), Default::optional(),
                "Kinetic constants describing first order reactions.");
		rec.declare_key("products", Array(String()), Default::obligatory(),
				"Identifies isotopes which decays parental atom to.");
		rec.declare_key("branch_ratios", Array(Double()), Default("1.0"),   // default is one product, with ratio == 1.0
				"Decay chain branching percentage.");
		rec.finish();
	}
	return rec;
}

Input::Type::Record & Pade_approximant::get_input_type()
{
	using namespace Input::Type;
	static Record rec("Approximant", "Information about pade approximant parameters.");

	if (!rec.is_finished()) {
	    rec.derive_from( Reaction::get_input_type() );
        rec.declare_key("decays", Array( Pade_approximant::get_one_decay_substep() ), Default::obligatory(),
                "Description of particular decay chain substeps.");
		rec.declare_key("nom_pol_deg", Integer(), Default("0"),
				"Polynomial degree of the nominator of Pade approximant.");
		rec.declare_key("den_pol_deg", Integer(), Default("0"),
				"Polynomial degree of the nominator of Pade approximant");
		rec.finish();
	}
	return rec;
}

using namespace std;

Pade_approximant::Pade_approximant(TimeMarks &marks, Mesh &init_mesh, MaterialDatabase &material_database, Input::Record in_rec, vector<string> &names) //(double timeStep, Mesh * mesh, int nrOfSpecies, bool dualPorosity) //(double timestep, int nrOfElements, double ***ConvectionMatrix)
			:Linear_reaction(marks, init_mesh, material_database, in_rec, names)
{
	nom_pol_deg = in_rec.val<int>("nom_pol_deg");
	den_pol_deg = in_rec.val<int>("den_pol_deg");
	if((nom_pol_deg + den_pol_deg) < 0){
		cout << "You did not specify Pade approximant required polynomial degrees." << endl;
		//This occasion should cause an error.
		//break;
	}
	cout << "Pade_approximant constructor is running." << endl;
}

Pade_approximant::~Pade_approximant()
{
	/*int i, rows, n_subst;

	if(half_lives != NULL){
		free(half_lives);
		half_lives = NULL;
	}

	if(substance_ids != NULL){
		free(substance_ids);
		substance_ids = NULL;
	}

	release_reaction_matrix();*/

	cout << "Pade approximant destructor is running."  << endl;
}



double **Pade_approximant::modify_reaction_matrix(void)
{
	Mat Denominator;
	Mat Nominator;
	//Mat Reaction_matrix;
	Mat Pade_approximant;
	MatFactorInfo matfact;
	PC Precond;
	IS rperm, cperm;
	Vec tmp1; //contains the information about concentrations of all the species in one particular element
	Vec tmp2; //the same as tmp1
	PetscInt n, m = 2;
	PetscScalar nominator_coef[nom_pol_deg];
	PetscScalar denominator_coef[den_pol_deg];
	PetscScalar Hlp_mat[1];
	PetscScalar *Array_hlp;
	const PetscScalar *Reaction_matrix_row;
	char dec_name[30];
	int rows, cols, dec_nr, dec_name_nr = 1, index, prev_index, i, j;

	//create the matrix Reaction_matrix
	MatCreate(PETSC_COMM_SELF, &Reaction_matrix);
	MatSetSizes(Reaction_matrix, PETSC_DECIDE, PETSC_DECIDE, n_substances(), n_substances()); //should be probably multiplied by 2 (which is the value of m)
	MatSetType(Reaction_matrix, MATAIJ);
	MatAssemblyBegin(Reaction_matrix, MAT_FINAL_ASSEMBLY);
	MatAssemblyEnd(Reaction_matrix, MAT_FINAL_ASSEMBLY);

	//create the matrix N
	MatDuplicate(Reaction_matrix, MAT_COPY_VALUES, &Nominator);

	//create the matrix D
	MatDuplicate(Reaction_matrix, MAT_COPY_VALUES, &Denominator);

	//create the matrix pade
	MatDuplicate(Reaction_matrix, MAT_COPY_VALUES, &Pade_approximant);
	MatAssemblyBegin(Pade_approximant, MAT_FINAL_ASSEMBLY);
	MatAssemblyEnd(Pade_approximant, MAT_FINAL_ASSEMBLY);

	MatAssemblyBegin(Reaction_matrix, MAT_FINAL_ASSEMBLY);
	MatAssemblyEnd(Reaction_matrix, MAT_FINAL_ASSEMBLY);
	MatView(Reaction_matrix,PETSC_VIEWER_STDOUT_SELF);

	//Computation of nominator in pade approximant follows
	MatZeroEntries(Nominator);
	MatAssemblyBegin(Nominator, MAT_FINAL_ASSEMBLY);
	MatAssemblyEnd(Nominator, MAT_FINAL_ASSEMBLY);
	for(j = nom_pol_deg; j >= 0; j--)
	{
		nominator_coef[j] = (PetscScalar) (faktorial(nom_pol_deg + den_pol_deg - j) * faktorial(nom_pol_deg)) / (faktorial(nom_pol_deg + den_pol_deg) * faktorial(j) * faktorial(nom_pol_deg - j));
	}
	evaluate_matrix_polynomial(&Nominator, &Reaction_matrix, nominator_coef);
	MatView(Nominator,PETSC_VIEWER_STDOUT_WORLD);

	//Computation of denominator in pade approximant follows
	MatZeroEntries(Denominator);
	MatAssemblyBegin(Denominator, MAT_FINAL_ASSEMBLY);
	MatAssemblyEnd(Denominator, MAT_FINAL_ASSEMBLY);
	for(i = den_pol_deg; i >= 0; i--)
	{
		denominator_coef[i] = (PetscScalar) pow(-1.0,i) * faktorial(nom_pol_deg + den_pol_deg - i) * faktorial(den_pol_deg) / (faktorial(nom_pol_deg + den_pol_deg) * faktorial(i) * faktorial(den_pol_deg - i));
	}
	evaluate_matrix_polynomial(&Denominator, &Reaction_matrix, denominator_coef);
	MatView(Denominator, PETSC_VIEWER_STDOUT_WORLD);

	PCCreate(PETSC_COMM_WORLD, &Precond);
	PCSetType(Precond, PCLU);
	PCSetOperators(Precond, Denominator, Denominator, DIFFERENT_NONZERO_PATTERN);
	//PCFactorSetMatOrderingType(Precond, MATORDERINGNATURAL);
	PCFactorSetMatOrderingType(Precond, MATORDERINGRCM);
	PCSetUp(Precond);

	VecCreate(PETSC_COMM_WORLD, &tmp1);
	VecSetSizes(tmp1, PETSC_DECIDE, n_substances());
	VecSetFromOptions(tmp1);
	VecDuplicate(tmp1, &tmp2);

	for(rows = 0; rows < n_substances(); rows++){
		MatGetColumnVector(Nominator, tmp1, rows);
		//VecView(tmp1, PETSC_VIEWER_STDOUT_SELF);
		PCApply(Precond, tmp1, tmp2);
		PCView(Precond, PETSC_VIEWER_STDOUT_WORLD);
		//VecView(tmp2, PETSC_VIEWER_STDOUT_SELF);
		VecGetArray(tmp2, &Array_hlp);
		for(cols = 0; cols < n_substances(); cols++)
		{
			MatSetValue(Pade_approximant, rows, cols, Array_hlp[cols], ADD_VALUES);
		}
	}
	MatAssemblyBegin(Pade_approximant, MAT_FINAL_ASSEMBLY);
	MatAssemblyEnd(Pade_approximant, MAT_FINAL_ASSEMBLY);

	//pade assembled to reaction_matrix
	for(rows = 0; rows < n_substances(); rows++)
	{
		for(cols = 0; cols < n_substances(); cols++)
		{
			MatGetValues(Pade_approximant, 1, &rows, 1, &cols, Hlp_mat); //&Hlp_mat[n_substances()*rows + cols]);
			reaction_matrix[rows][cols] = (double) (Hlp_mat[0]);
		}
	}

	print_reaction_matrix(); //for visual control of equality of reaction_matrix in comparison with pade aproximant*/

	VecDestroy(&tmp1);
	VecDestroy(&tmp2);
	PCDestroy(&Precond);
	MatDestroy(&Denominator);
	MatDestroy(&Nominator);
	//MatDestroy(Reaction_matrix);
	MatDestroy(&Pade_approximant);

	return reaction_matrix;
}

void Pade_approximant::evaluate_matrix_polynomial(Mat *Polynomial, Mat *Reaction_matrix, PetscScalar *coef)
{
	Mat Identity;

	//create Identity matrix
	MatCreate(PETSC_COMM_SELF, &Identity);
	MatSetSizes(Identity, PETSC_DECIDE, PETSC_DECIDE, n_substances(), n_substances()); //should be probably multiplied by 2 (which is the value of m)
	MatSetType(Identity, MATAIJ);
	MatAssemblyBegin(Identity, MAT_FINAL_ASSEMBLY);
	MatAssemblyEnd(Identity, MAT_FINAL_ASSEMBLY);
	MatShift(Identity, 1.0);

	for(int i = den_pol_deg; i >= 0; i--)
		{
			MatMatMult(*Polynomial, *Reaction_matrix, MAT_INITIAL_MATRIX, PETSC_DEFAULT, Polynomial);
			MatAXPY(*Polynomial, coef[i], Identity, DIFFERENT_NONZERO_PATTERN);
		}

	MatDestroy(&Identity);

	return;
}

void Pade_approximant::set_time_step(double new_timestep)
{
	time_step = new_timestep;
	release_reaction_matrix();
	allocate_reaction_matrix();
	modify_reaction_matrix();
	//static_cast<Pade_approximant *> (this)->modify_reaction_matrix();
	return;
}

void Pade_approximant::prepare_inputs(Input::Record in_rec)
{
    unsigned int idx;

	Input::Array decay_array = in_rec.val<Input::Array>("decays");

	substance_ids.resize( decay_array.size() );
	half_lives.resize( decay_array.size() );
	bifurcation.resize( decay_array.size() );

	int i_decay=0;
	for (Input::Iterator<Input::Record> dec_it = decay_array.begin<Input::Record>(); dec_it != decay_array.end(); ++dec_it, ++i_decay)
	{
		//half-lives determining part
		Input::Iterator<double> it_hl = dec_it->find<double>("half_life");
		if (it_hl) {
		   half_lives[i_decay] = *it_hl;
		} else {
		   it_hl = dec_it->find<double>("kinetic");
		   if (it_hl) {
			   half_lives[i_decay] = log(2)/(*it_hl);
		   } else {
		    xprintf(UsrErr, "Missing half-life or kinetic in the %d-th reaction.\n", i_decay);
		  }
		}

		//indices determining part
		string parent_name = dec_it->val<string>("parent");
		Input::Array product_array = dec_it->val<Input::Array>("products");
		Input::Array ratio_array = dec_it->val<Input::Array>("branch_ratios"); // has default value [ 1.0 ]

		// substance_ids contains also parent id
		if (product_array.size() > 0)   substance_ids[i_decay].resize( product_array.size()+1 );
		else			xprintf(UsrErr,"Empty array of products in the %d-th reaction.\n", i_decay);


		// set parent index
		idx = find_subst_name(parent_name);
		if (idx < n_substances())	substance_ids[i_decay][0] = idx;
		else                		xprintf(UsrErr,"Wrong name of parent substance in the %d-th reaction.\n", i_decay);

		// set products
		unsigned int i_product = 1;
		for(Input::Iterator<string> product_it = product_array.begin<string>(); product_it != product_array.end(); ++product_it, i_product++)
		{
			idx = find_subst_name(*product_it);
			if (idx < n_substances())   substance_ids[i_decay][i_product] = idx;
			else                    	xprintf(Msg,"Wrong name of %d-th product in the %d-th reaction.\n", i_product-1 , i_decay);
		}

		//bifurcation determining part
        if (ratio_array.size() == product_array.size() )   ratio_array.copy_to( bifurcation[i_decay] );
        else            xprintf(UsrErr,"Number of branches %d has to match number of products %d in the %d-th reaction.\n",
                                       ratio_array.size(), product_array.size(), i_decay);

	}
}

double **Pade_approximant::allocate_reaction_matrix(void) //reaction matrix initialization
{
	int rows, cols;

	cout << "We are going to allocate reaction matrix" << endl;
	if (reaction_matrix == NULL) reaction_matrix = (double **)xmalloc(n_substances() * sizeof(double*));//allocation section
	for(rows = 0; rows < n_substances(); rows++){
		reaction_matrix[rows] = (double *)xmalloc(n_substances() * sizeof(double));
	}
	for(rows = 0; rows < n_substances();rows++){
	 for(cols = 0; cols < n_substances(); cols++){
		 if(rows == cols)   reaction_matrix[rows][cols] = 1.0;
		 else           	reaction_matrix[rows][cols] = 0.0;
	 }
	}
	print_reaction_matrix();
	return reaction_matrix;
}

void Pade_approximant::print_half_lives(int nr_of_substances) {
    int i;

    xprintf(Msg, "\nHalf-lives are defined as:");
    for (i = 0; i < (nr_of_substances - 1); i++) {
        if (i < (nr_of_substances - 2)) //cout << " " << half_lives[i] <<",";
            xprintf(Msg, " %f", half_lives[i]);
        if (i == (nr_of_substances - 2)) //cout << " " << half_lives[i] <<"\n";
            xprintf(Msg, " %f\n", this->half_lives[i]);
    }
}

void Pade_approximant::print_reaction_matrix(void)
{
	int cols,rows;

	if(reaction_matrix != NULL){
		xprintf(Msg,"\ntime_step %f,Reaction matrix looks as follows:\n",time_step);
		for(rows = 0; rows < n_substances(); rows++){
			for(cols = 0; cols < n_substances(); cols++){
					xprintf(Msg,"%f\t",reaction_matrix[rows][cols]);
			}
			xprintf(Msg,"\n");
		}
	}else{
		xprintf(Msg,"\nReaction matrix needs to be allocated.\n");
	}
	return;
}

void Pade_approximant::release_reaction_matrix(void)
{
	int i;
	if(reaction_matrix != NULL)
	{
		for(i = 0; i < n_substances(); i++)
		{
			if(reaction_matrix[i] != NULL)
			{
				free(reaction_matrix[i]);
				reaction_matrix[i] = NULL;
			}
		}
		free(reaction_matrix);
		reaction_matrix = NULL;
	}
}
