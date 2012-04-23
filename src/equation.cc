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
 * @brief Abstract base class for equation clasess.
 *
 *  @author Jan Brezina
 */

#include "equation.hh"
EquationBase::EquationBase()
: time_marks(NULL),
  mesh_(NULL),
  mat_base(NULL),
  time_(NULL),
  equation_mark_type_(time_marks->new_mark_type())
{};
EquationBase::EquationBase(TimeMarks &marks, Mesh &mesh, MaterialDatabase &mat_base)
: time_marks(&marks),
  mesh_(&mesh),
  mat_base(&mat_base),
  time_(NULL),
  equation_mark_type_(time_marks->new_mark_type())
{}

EquationBase::~EquationBase(){
    /**delete of all references to registered classes */
//       if (registered_vectors_ != NULL){
//           xprintf(Msg,"delete registered_classes_.\n");
//           delete registered_vectors_;
//       }
}

/*******************************************************************************************************/
/**** Checkpointing methods  ***************************************************************************/

std::string EquationBase::class_name(){
    return this->class_name_;
}

bool EquationBase::is_checkpointing_registered(){
    return checkpointing_registered_;
};

void EquationBase::save_state(){};//std::ofstream* out_stream //CheckpointingOutput* output

void EquationBase::restore_state(CheckpointingOutput* output){};

//void EquationBase::register_vectors(){};

void EquationBase::register_vector(Vec* vec, std::string vector_name){
//    if (!is_checkpointing_on()) return ;

    xprintf(Msg, "register_vector: %s\n", vector_name.c_str());

    RegisteredVector obj;
    obj.vec = vec;
    obj.vector_name = vector_name;

    registered_vectors_->push_back(obj);
};

/** \brief saves all registered vectors*/
void EquationBase::save_vectors(){
    for(RegisteredVectors::iterator it = registered_vectors_->begin(); it != registered_vectors_->end(); ++it){
        xprintf(Msg, "Saving vector: %s\n", it->vector_name.c_str());
    }

};

/** \brief restores all registered vectors*/
void EquationBase::restore_vectors(){
    for(RegisteredVectors::iterator it = registered_vectors_->begin(); it != registered_vectors_->end(); ++it){
        xprintf(Msg, "Restoring vector: %s\n", it->vector_name.c_str());
    }
};
