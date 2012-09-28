/*
 * pade_approximant.h
 *
 *  Created on: Apr 2, 2012
 *      Author: lukas
 */

#ifndef PADE_APPROXIMANT_H_
#define PADE_APPROXIMANT_H_

#include <vector>
#include <input/input_type.hh>
#include <input/accessors.hh>

#include "petscvec.h"
#include "petscmat.h"
#include "petscksp.h"

//#include "reaction/linear_reaction.hh"

class Mesh;
class Distribution;
class Reaction;

class Pade_approximant: public Linear_reaction
{
	public:
		/*
		* Static method for new input data types input
		*/
		static Input::Type::Record &get_input_type();
		/*
	 	* Static method gets information about particular decay step
		*/
		static Input::Type::Record & get_one_decay_substep();
        /**
         *  Constructor with parameter for initialization of a new declared class member
         *  TODO: parameter description
         */
		Pade_approximant(TimeMarks &marks, Mesh &mesh, MaterialDatabase &mat_base, Input::Record in_rec, vector<string> &names);

		/**
		*	Destructor.
		*/
		~Pade_approximant(void);

		/**
		*	For simulation of chemical reaction in just one element either inside of MOBILE or IMMOBILE pores.
		*/
		//double **compute_reaction(double **concentrations, int loc_el);
		/**
		*	Prepared to compute simple chemical reactions inside all of considered elements. It calls compute_reaction(...) for all the elements controled by concrete processor, when the computation is paralelized.
		*/
		//void compute_one_step(void);
		/**
		*	This method enables to change the timestep for computation of simple chemical reactions. Such a change is conected together with creating of a new reaction matrix necessity.
		*/
		//void set_time_step(double new_timestep);
		/**
		* Folowing method enabels the timestep for chemistry to have the value written in ini-file.
		*/
		//virtual
		void set_time_step(double time_step);
		/**
		* It enables to evaluate matrix nominator and denominator present in Pade approximant.
		*/
		void evaluate_matrix_polynomial(Mat *Polynomial, Mat *Reaction_matrix, PetscScalar *coef);
		/**
		*	Evaluates Pade approximant from Reaction_matrix.
		*/
		double **modify_reaction_matrix(void);
	protected:
		/**
		*	This method disables to use constructor without parameters.
		*/
		Pade_approximant();
		/**
		*
		*/
		double **allocate_reaction_matrix(void);
		/**
		*	Fuctions holds together setting of isotopes, bifurcations and substance indices.
		*/
		void prepare_inputs(Input::Record in_rec);
		/**
		*	For control printing of a matrix describing simple chemical raections.
		*/
		void print_reaction_matrix(void);
		/**
		*	For printing nr_of_isotopes identifies of isotopes in a current decay chain.
		*/
		//void print_indices(int dec_nr, int n_subst);
		/**
		* Following method releases reaction matrix to make it possible to set a new time step for chemistry.
		*/
		void release_reaction_matrix();
		/**
		*	For printing (nr_of_isotopes - 1) doubles containing half-lives belonging to particular isotopes on screen.
		*/
		void print_half_lives(int n_subst);
		/**
		* 	Boolean which indicates the use of Pade approximant of the matrix exponential.
		*/
		//bool matrix_exp_on;
		/**
		*	Small (nr_of_species x nr_of_species) square matrix for realization of radioactive decay and first order reactions simulation.
		*/
		double **reaction_matrix;
		/**
		*	Sequence of (nr_of_isotopes - 1) doubles containing half-lives belonging to particular isotopes.
		*/
		vector<double> half_lives;
		/**
		*	Sequence of integers describing an order of isotopes in decay chain or first order reaction.
		*/
		vector< vector <unsigned int> >substance_ids;
		/**
		*	Two dimensional array contains mass percentage of every single decay bifurcation on every single row.
		*/
		std::vector<std::vector<double> > bifurcation;
		/**
		*	Integer which informs about the order of a polynomial term in nominator of Pade approximant rational term.
		*/
		int nom_pol_deg;
		/**
		*	Integer which informs about the order of a polynomial term in denominator of Pade approximant rational term.
		*/
		int den_pol_deg;
		/**
		* PETSC format of a matrix describing linear chemical reaction.
		*/
		Mat Reaction_matrix;
};

#endif /* PADE_APPROXIMANT_H_ */
