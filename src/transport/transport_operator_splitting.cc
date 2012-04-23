/*
 * transport_operator_splitting.cc
 *
 *  Created on: May 21, 2011
 *      Author: jiri
 */

#include "transport/transport_operator_splitting.hh"
#include <petscmat.h>
#include "system/sys_vector.hh"
#include <time_governor.hh>
#include <materials.hh>
#include "equation.hh"
#include "transport/transport.h"
#include "mesh/mesh.h"
#include "reaction/linear_reaction.hh"
#include "semchem/semchem_interface.hh"
#include "system/par_distribution.hh"
#include "io/output.h"

#include <boost/serialization/export.hpp>
//BOOST_CLASS_EXPORT_GUID(EquationBase, "EquationBase")
//BOOST_CLASS_EXPORT_GUID(ConvectionTransport, "ConvectionTransport")
//BOOST_CLASS_EXPORT_GUID(TransportSources, "TransportSources")
//BOOST_CLASS_EXPORT_GUID(Linear_reaction, "Linear_reaction")
//BOOST_CLASS_EXPORT_GUID(Semchem_interface, "Semchem_interface")

TransportOperatorSplitting::TransportOperatorSplitting(){};
TransportOperatorSplitting::TransportOperatorSplitting(TimeMarks &marks, Mesh &init_mesh, MaterialDatabase &material_database, CheckpointingManager* checkpointing_manager )
: TransportBase(marks, init_mesh, material_database)
{
    /**sets, that this class is under checkpointing */
    checkpointing_registered_ = false;
    this->class_name_ = "transport_reaction";

//    if(checkpointing_manager->is_checkpointing_on()){
//        checkpointing_manager->set_restore_output(class_name());
//    };

	Distribution *el_distribution;
	int *el_4_loc;

    double problem_save_step = OptGetDbl("Global", "Save_step", "1.0");
    double problem_stop_time = OptGetDbl("Global", "Stop_time", "1.0");

	convection = new ConvectionTransport(marks, *mesh_, *mat_base);
	convection->test_concentration_sources(*convection);

	// Chemistry initialization
	decayRad = new Linear_reaction(0.0, mesh_, convection->get_n_substances(), convection->get_dual_porosity());
	convection->get_par_info(el_4_loc, el_distribution);
	decayRad->set_concentration_matrix(convection->get_prev_concentration_matrix(), el_distribution, el_4_loc);
	Semchem_reactions = new Semchem_interface(0.0, mesh_, convection->get_n_substances(), convection->get_dual_porosity()); //(mesh->n_elements(),convection->get_concentration_matrix(), mesh);
	Semchem_reactions->set_el_4_loc(el_4_loc);
	Semchem_reactions->set_concentration_matrix(convection->get_prev_concentration_matrix(), el_distribution, el_4_loc);

	if(checkpointing_manager->is_checkpointing_on() && is_checkpointing_registered()){
	    checkpointing_manager->get_restore_output()->load_data(equation_mark_type_, "transport_mark_type");
	    checkpointing_manager->get_restore_output()->load_data(time_);
	}else{
	    equation_mark_type_ = marks.new_mark_type();
	    time_ = new TimeGovernor(0.0, problem_stop_time, *time_marks, this->mark_type());
	}

	output_mark_type = this->mark_type() | time_marks->type_fixed_time() | time_marks->type_output();

	if(checkpointing_manager->is_checkpointing_on() && is_checkpointing_registered()){
	        /**time_marks are globaly restored, so they mustn't to be added*/
//	    time_marks->add_time_marks(0.0, OptGetDbl("Global", "Save_step", "1.0"), time_->end_time(), output_mark_type );
    }else{
        time_marks->add_time_marks(0.0, OptGetDbl("Global", "Save_step", "1.0"), time_->end_time(), output_mark_type );
    }
	// TOdO: this has to be set after construction of transport matrix !!


	// register output vectors from convection
	double ***out_conc = convection->get_out_conc();
	char    **substance_name = convection->get_substance_names();

	string output_file = IONameHandler::get_instance()->get_output_file_name(OptGetFileName("Transport", "Transport_out", "\\"));
	field_output = new OutputTime(mesh_, output_file);

	/*
    transport_out_fname = IONameHandler::get_instance()->get_output_file_name(OptGetFileName("Transport", "Transport_out", "\\"));
    transport_out_im_fname = IONameHandler::get_instance()->get_output_file_name(OptGetFileName("Transport", "Transport_out_im", "\\"));
    transport_out_sorp_fname = IONameHandler::get_instance()->get_output_file_name(OptGetFileName("Transport", "Transport_out_sorp", "\\"));
    transport_out_im_sorp_fname = IONameHandler::get_instance()->get_output_file_name(OptGetFileName("Transport", "Transport_out_im_sorp", "\\"));
    */

    for(int subst_id=0; subst_id < convection->get_n_substances(); subst_id++) {
         // TODO: What about output also other "phases", IMMOBILE and so on.
         std::string subst_name = std::string(substance_name[subst_id]) + "_mobile";
         double *data = out_conc[MOBILE][subst_id];
         field_output->register_elem_data<double>(subst_name, "M/L^3", data , mesh_->n_elements());
    }
    // write initial condition
    convection->output_vector_gather();
    field_output->write_data(time_->t());

    registered_vectors_ = new RegisteredVectors();

    //    register_vector(vsources_corr, "vsources_corr");

}

TransportOperatorSplitting::~TransportOperatorSplitting()
{
    delete field_output;
    delete convection;
    delete decayRad;
    delete Semchem_reactions;
    delete time_;
}

void TransportOperatorSplitting::output_data(){

    if (time_->is_current(output_mark_type)) {
        convection->output_vector_gather();
        field_output->write_data(time_->t());
    }
}

void TransportOperatorSplitting::read_simulation_step(double sim_step) {
	time_->set_constrain(sim_step);
}


void TransportOperatorSplitting::update_solution() {


    time_->next_time();
	convection->set_target_time(time_->t());

	decayRad->set_time_step(convection->time().estimate_dt());
	//cout << "recent time step value is " << decayRad->get_time_step() << endl;
	// TODO: update Semchem time step here!!
	Semchem_reactions->set_timestep(convection->time().estimate_dt());

    xprintf( Msg, "t: %f (TOS)                  cfl_dt: %f ", convection->time().t(), convection->time().estimate_dt() );
    START_TIMER("transport_steps");
    int steps=0;
    while ( convection->time().lt(time_->t()) )
    {
        steps++;
	    // one internal step
	    //xprintf( Msg, "Time : %f\n", convection->time().t() );
	    convection->compute_one_step();
	    // Calling linear reactions and Semchem
	    decayRad->compute_one_step();
	    Semchem_reactions->compute_one_step();
	}
    END_TIMER("transport_steps");
    //DBGMSG("conv time: %f TOS time: %f\n", convection->time().t(), time_->t());
    xprintf( Msg, " steps: %d\n",steps);
}


void TransportOperatorSplitting::set_velocity_field(Vec &vec)
{
	convection->set_flow_field_vector(&vec);
};


void TransportOperatorSplitting::get_parallel_solution_vector(Vec &vec){
	convection->compute_one_step();
};

void TransportOperatorSplitting::get_solution_vector(double * &x, unsigned int &a){
	convection->compute_one_step();
};


//void TransportOperatorSplitting::save_state(CheckpointingOutput* output){//CheckpointingOutput* output
//    xprintf(Msg, "Saving state of TransportOperatorSplitting");
//    TimeMark::Type mark_type;
//    mark_type = this->mark_type()|time_->marks().type_checkpointing();
//
//    if(time_->is_current(mark_type)){
//        xprintf(Msg, " is CURRENT\n");
//    };
//
//    output->save_data(this->mark_type(), "transport_mark_type");
//    output->save_data(time_);
//
//    convection->save_state(output);
//    decayRad->save_state(output);
//    Semchem_reactions->save_state(output);
//
//}

void TransportOperatorSplitting::save_state(){//std::ofstream* out_stream //CheckpointingOutput* output
    xprintf(Msg, "Saving state of TransportOperatorSplitting");
    TimeMark::Type mark_type;
    mark_type = this->mark_type()|time_->marks().type_checkpointing();

    if(time_->is_current(mark_type)){
        xprintf(Msg, " is CURRENT\n");
    };
//
//    output->save_data(this->mark_type(), "transport_mark_type");
//    output->save_data(time_);
//
//    convection->save_state(output);
//    decayRad->save_state(output);
//    Semchem_reactions->save_state(output);

    convection->save_vectors();
//    decayRad->save_vectors();
//    Semchem_reactions->save_vectors();
}

void TransportOperatorSplitting::restore_state(CheckpointingOutput* output){
    xprintf(Msg, "Restoring state of TransportOperatorSplitting\n");

    output->load_data(equation_mark_type_, "transport_mark_type");
    output->load_data(time_);

//    convection->restore_state(output);
//    decayRad->restore_state(output);
//    Semchem_reactions->restore_state(output);

}
