#ifndef TRANSPORT_OPERATOR_SPLITTING_HH_
#define TRANSPORT_OPERATOR_SPLITTING_HH_

#include "coupling/equation.hh"

#include <limits>
#include "io/output.h"
#include "flow/mh_dofhandler.hh"
#include "fields/field_base.hh"
#include "fields/field_values.hh"
#include "transport/mass_balance.hh"


/// external types:
//class LinSys;
//struct Solver;
class Mesh;
//class SchurComplement;
//class Distribution;
//class SparseGraph;

class Reaction;
class Linear_reaction;
//class Pade_approximant;
class Sorption;
//class Dual_por_exchange;
class Semchem_interface;
class ConvectionTransport;


/**
 * @brief Specification of transport model interface.
 *
 * Here one has to specify methods for setting or getting data particular to
 * transport equations.
 */
class TransportBase : public EquationBase, public EquationForMassBalance {
public:

    /**
     * Class with fields that are common to all transport models.
     */
	class TransportEqData : public EqDataBase {
	public:

		TransportEqData(const std::string& eq_name);
		inline virtual ~TransportEqData() {};

		/// Initial concentrations.
		Field<3, FieldValue<3>::Vector> init_conc;
		/// Mobile porosity
		Field<3, FieldValue<3>::Scalar> por_m;

		/**
		 * Boundary conditions (Dirichlet) for concentrations.
		 * They are applied only on water inflow part of the boundary.
		 */
		BCField<3, FieldValue<3>::Vector> bc_conc;

		/// Pointer to DarcyFlow field cross_section
		Field<3, FieldValue<3>::Scalar > *cross_section;

		/// Concentration sources - density of substance source, only positive part is used.
		Field<3, FieldValue<3>::Vector> sources_density;
		/// Concentration sources - Robin type, in_flux = sources_sigma * (sources_conc - mobile_conc)
		Field<3, FieldValue<3>::Vector> sources_sigma;
		Field<3, FieldValue<3>::Vector> sources_conc;

	};

	/// Common specification of the Transport input record.
    static Input::Type::AbstractRecord input_type;
    /**
     * Specification of the output record. Need not to be used by all transport models, but they should
     * allow output of similar fields.
     */
    static Input::Type::Record input_type_output_record;

    TransportBase(Mesh &mesh, const Input::Record in_rec);
    virtual ~TransportBase();


    /**
     * This method takes sequential PETSc vector of side velocities and update
     * transport matrix. The ordering is same as ordering of sides in the mesh.
     * We just keep the pointer, but do not destroy the object.
     *
     * TODO: We should pass whole velocity field object (description of base functions and dof numbering) and vector.
     */
    virtual void set_velocity_field(const MH_DofHandler &dh) {
        mh_dh=&dh;
    }

    /**
     * @brief Sets pointer to data of other equations.
     * TODO: there should be also passed the sigma parameter between dimensions
     * @param cross_section is pointer to cross_section data of Darcy flow equation
     */
    //virtual void set_cross_section_field(Field<3, FieldValue<3>::Scalar > *cross_section) =0;

    /**
     * @brief Write computed fields.
     */
    virtual void output_data() =0;

    /**
     * Getter for mass balance class
     */
    MassBalance *mass_balance() { return mass_balance_; }

    /// Returns number of trnasported substances.
    inline unsigned int n_substances() { return n_subst_; }

    /// Returns reference to the vector of substnace names.
    inline vector<string> &substance_names() { return subst_names_; }


protected:

    /// Returns the region database.
    const RegionDB *region_db() { return &mesh_->region_db(); }

    /// Number of transported substances.
    unsigned int n_subst_;

    /// Names of transported substances.
    std::vector<string> subst_names_;

    /**
     * Temporary solution how to pass velocity field form the flow model.
     * TODO: introduce FieldDiscrete -containing true DOFHandler and data vector and pass such object together with other
     * data. Possibly make more general set_data method, allowing setting data given by name. needs support from EqDataBase.
     */
    const MH_DofHandler *mh_dh;

    /**
     * Mark type mask that is true for time marks of output points of the transport model.
     * E.g. for TransportOperatorSplitting this is same as the output points of its transport sub-model.
     */
    TimeMark::Type output_mark_type;

    /// object for calculation and writing the mass balance to file.
    MassBalance *mass_balance_;
};



/**
 * @brief Empty transport class.
 */
class TransportNothing : public TransportBase {
public:
    inline TransportNothing(Mesh &mesh_in)
    : TransportBase(mesh_in, Input::Record() )
    {
        // make module solved for ever
        time_=new TimeGovernor();
        time_->next_time();
    };

    inline virtual ~TransportNothing()
    {}

    inline virtual void get_solution_vector(double * &vector, unsigned int &size) {
        ASSERT( 0 , "Empty transport class do not provide solution!");
    }

    virtual void get_parallel_solution_vector(Vec &vector) {
        ASSERT( 0 , "Empty transport class do not provide solution!");
    };

    inline virtual void output_data() {};

    TimeIntegrationScheme time_scheme() { return TimeIntegrationScheme::none; }

private:

    inline void calc_fluxes(vector<vector<double> > &bcd_balance, vector<vector<double> > &bcd_plus_balance, vector<vector<double> > &bcd_minus_balance) {};
    inline void calc_elem_sources(vector<vector<double> > &mass, vector<vector<double> > &src_balance) {};

};



/**
 * @brief Coupling of a transport model with a reaction model by operator splitting.
 *
 * Outline:
 * Transport model is any descendant of TransportBase (even TransportOperatorSplitting itself). This
 * should perform the transport possibly with diffusion and usually without coupling between substances and phases.
 *
 * Reaction is any descendant of the ReactionBase class. This represents reactions in general way of any coupling that
 * happens between substances and phases on one element or more generally on one DoF.
 */

class TransportOperatorSplitting : public TransportBase {
public:

    /**
     * @brief Declare input record type for the equation TransportOperatorSplittiong.
     *
     * TODO: The question is if this should be a general coupling class
     * (e.g. allow coupling TranportDG with reactions even if it is not good idea for numerical reasons.)
     * To make this a coupling class we should modify all main input files for transport problems.
     */
    static Input::Type::Record input_type;

    /// Constructor.
    TransportOperatorSplitting(Mesh &init_mesh, const Input::Record &in_rec);
    /// Destructor.
    virtual ~TransportOperatorSplitting();


    virtual void set_velocity_field(const MH_DofHandler &dh);
    virtual void update_solution();
    //virtual void compute_one_step();
    //virtual void compute_until();
    virtual void get_parallel_solution_vector(Vec &vc);
    virtual void get_solution_vector(double* &vector, unsigned int &size);

    void compute_until_save_time();
    void compute_internal_step();
    void output_data();

   
    /**
     * @brief Sets pointer to data of other equations.
     * TODO: there should be also passed the sigma parameter between dimensions
     * @param cross_section is pointer to cross_section data of Darcy flow equation
     */
    void set_eq_data(Field<3, FieldValue<3>::Scalar > *cross_section);

    TimeIntegrationScheme time_scheme() { return TimeIntegrationScheme::none; }


private:
    /**
     * Implements the virtual method EquationForMassBalance::calc_fluxes().
     */
    void calc_fluxes(vector<vector<double> > &bcd_balance, vector<vector<double> > &bcd_plus_balance, vector<vector<double> > &bcd_minus_balance);
    /**
     * Implements the virtual method EquationForMassBalance::calc_elem_sources().
     */
    void calc_elem_sources(vector<vector<double> > &mass, vector<vector<double> > &src_balance);

    ConvectionTransport *convection;
    Reaction *decayRad; //Linear_reaction *decayRad; //Reaction *decayRad;
    Sorption *sorptions;
    Sorption *sorptions_immob;
    //Dual_por_exchange *dual_por_exchange;
    Semchem_interface *Semchem_reactions;
    //int steps;


};





#endif // TRANSPORT_OPERATOR_SPLITTING_HH_
