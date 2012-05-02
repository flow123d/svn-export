/** @brief class Linear_reaction is used to enable simulation of simple chemical reactions
 *
 * Class in this file makes it possible to realize  simulation of reaction of the first order by simple matrix multiplication.
 * One step of the linear reaction is represented as a product of a matrix containing concentrations of observed speciesin elements in rows multiplied by so called
 * reaction_matrix. Through this way radioactive decay can bee also realized and that was exactly what we did at the begining of journey. :-)
 * Matrix containing concentrations has a dimension Nxn, where N is a number of elements in mesh and n denotes a number of transported chemical species.
 * The reaction_matrix is a square matrix and it has a dimension nxn.
 *
 */
#ifndef REACT
#define REACT

#include "equation.hh"
//class Mesh;
class Distribution;

/*const int Linear_react = 0;
const int Linear_react_Pade = 1;
const int General_react_Semch = 2;*/

enum Reaction_type {No_reaction, Linear_react, Linear_react_Pade, General_react_Semch};

class Reaction: public EquationBase
{
	public:
        /**
         *  Constructor with parameter for initialization of a new declared class member
         *  TODO: parameter description
         */
		Reaction(TimeMarks &marks, Mesh &init_mesh, MaterialDatabase &material_database); //(double timeStep, Mesh * mesh, int nrOfSpecies, bool dualPorosity); //(double time_step, int nrOfElements, double ***ConcentrationMatrix);
		/**
		*	Destructor.
		*/
		~Reaction(void);
		/**
		*	For simulation of chemical raection in just one element either inside of MOBILE or IMMOBILE pores.
		*/
		virtual double **compute_reaction(double **concentrations, int loc_el);
		/**
		*	Prepared to compute simple chemical reactions inside all of considered elements. It calls compute_reaction(...) for all the elements controled by concrete processor, when the computation is paralelized.
		*/
		virtual void compute_one_step(void);
		/**
		* 	It returns current time step used for first order reactions.
		*/
		double get_time_step(void);
		/**
		*	It enables to set a number of transported species to set a size of reaction matrix.
		*/
		void set_nr_of_species(int n_substances);
		/**
		*	It returns a number of decays defined in input-file.
		*/
		int get_nr_of_decays(void);
		/**
		*	It returns a number of first order reactions defined in input-file.
		*/
		int get_nr_of_FoR(void);
		/**
		*	This method enables to change a data source the program is working with, during simulation.
		*/
		void set_concentration_matrix(double ***ConcentrationMatrix, Distribution *conc_distr, int *el_4_loc);
		/**
		*	This method enables to change the timestep for computation of simple chemical reactions. Such a change is conected together with creating of a new reaction matrix necessity.
		*/
		virtual void set_time_step(double new_timestep);
		/**
		* Folowing method enabels the timestep for chemistry to have the value written in ini-file.
		*/
		virtual void set_time_step(void);
		/**
		* Two virtual methods to be implemented in ancestors.
		*/
		void update_solution(void);
		void choose_next_time(void);
		void set_time_step_constrain(double dt);
		void get_parallel_solution_vector(Vec &vc);
		void get_solution_vector(double* &vector, unsigned int &size);
	protected:
		/**
		*	This method disables to use constructor without parameters.
		*/
		Reaction();
		/**
		*	This method enables to change total number of elements contained in mesh.
		*/
		void set_nr_of_elements(int nrOfElements);
		/**
		*	This method transfer pointer to mesh between a transport and reactive part of a program.
		*/
		void set_mesh_(Mesh *mesh);
		/**
		*
		*/
		void set_dual_porosity(void);
		/**
		*	This method reads from ini-file an information how many radioactive decays are under consideration.
		*/
		void set_nr_of_decays(void);
		/**
		*	This method reads from ini-file an information how many first order reactions are under consideration.
		*/
		void set_nr_of_FoR(void);
		/**
		*	Enables to compute factorial k!.
		*/
		int faktorial(int k);
		/**
		*	Contains number of transported chemical species.
		*/
		int nr_of_species;
		/**
		*	Boolean which enables to compute reactions also in immobile pores.
		*/
		bool dual_porosity_on;
		/**
		*	Holds the double describing time step for radioactive decay or first order reactions simulations.
		*/
		double time_step;
		/**
		*	Containes information about total number of elements.
		*/
		int nr_of_elements;
		/**
		*	Informs how many decay chains are under consideration. It means a number of [Decay_i] sections in ini-file.
		*/
		int nr_of_decays;
		/**
		*	Informs how many firts order reactions of the type A -> B are under consideration. It is a number of [FoReaction_i] in ini-file.
		*/
		int nr_of_FoR;
		/**
		*	Pointer to threedimensional array[mobile/immobile][species][elements] containing concentrations.
		*/
		double ***concentration_matrix;
		/**
		*	Pointer to reference to distribution of elements between processors.
		*/
		Distribution *distribution;
		/**
		*	Pointer to reference previous concentration array used in compute_reaction().
		*/
		double *prev_conc;
		/**
		*	Integer which informs about the order of a polynomial term in nominator of Pade approximant rational term.
		*/
		int nom_pol_deg;
		/**
		*	Integer which informs about the order of a polynomial term in denominator of Pade approximant rational term.
		*/
		int den_pol_deg;
		/**
		* Number of further species in Semchem, which can never be exhausted.
		*/
		int nr_of_further_species;
};

#endif
