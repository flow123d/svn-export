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
 * $Id: CheckpointingManager.cpp $
 * $Revision: 0 $
 * $LastChangedBy: vojtech.wrnata $
 * $LastChangedDate: 2012-1-4 20:34:14 +0100 (So, 10 pro 2011) $
 *
 * @file    CheckpointingManager.cpp
 * @brief   Main class for Checkpointing.
 *
 */

#include "CheckpointingManager.h"
#include "transport/transport_operator_splitting.hh"
#include "transport/transport.h"

#include <boost/serialization/export.hpp>

BOOST_CLASS_EXPORT_GUID(EquationBase, "EquationBase")
BOOST_CLASS_EXPORT_GUID(TransportBase, "TransportBase")
BOOST_CLASS_EXPORT_GUID(TransportNothing, "TransportNothing")
BOOST_CLASS_EXPORT_GUID(TransportOperatorSplitting, "TransportOperatorSplitting")
BOOST_CLASS_EXPORT_GUID(ConvectionTransport, "ConvectionTransport")
//BOOST_CLASS_EXPORT_GUID(Linear_reaction, "Linear_reaction")
//BOOST_CLASS_EXPORT_GUID(Distribution, "Distribution")


CheckpointingManager::CheckpointingManager(TimeMarks* marks) {//
    xprintf(Msg,"CheckpointingManager constructor.\n");

    util = new CheckpointingUtil();

    blbost = 256.12345678901234567890123456789;
    this->marks_ = marks;

    time_marks_output_ = set_output("TimeMarks");

    //default checkpointing is disabled
    checkpointing_on_ = OptGetBool("Checkpointing", "Checkpointing_on", "0");
    if (!is_checkpointing_on()) return; /**Checkpointing is not enabled - pá pá*/

    registered_classes_ = new RegisteredClasses();


    //default output file format is TXT - text file format
    out_file_format_ =  parse_output_format( OptGetStr("Checkpointing", "Output_format", "TXT") );
    //default checkpoints type i FIXED, TimeMarks are in fixed time positions
    checkpoints_type_ = parse_checkpoints_type( OptGetStr("Checkpointing", "Checkpoints_type", "FIXED") );


    start_time_ = time(NULL);
    xprintf(Msg,"CheckpointingManager construction time %i\n", start_time_);

    max_checkpoints_ = 0;
    checkpoints_interval_ = 0;

    checkpoint_ = 0;
    if (checkpoints_type_ == CH_DYNAMIC){
        /**Dynamic Checkpoints are enabled */
        max_checkpoints_ = OptGetDbl("Checkpointing", "Max_checkpoints", "1");
        /**restriction for max 10 saved checkpoints*/
        if (max_checkpoints_ > 10){
            max_checkpoints_ = 10;
        };
        checkpoint_ = 1;

        checkpoints_interval_ = OptGetInt("Checkpointing", "Checkpoints_interval", "3600");//jednou za hodinu

    }

    //    out_stream_.open(" ");
}

CheckpointingManager::~CheckpointingManager() {
    xprintf(Msg,"CheckpointingManager destructor.\n");
    if (!is_checkpointing_on()) return;

    /**delete of all references to outputs */
    for(RegisteredClasses::iterator it = registered_classes_->begin(); it != registered_classes_->end(); ++it){
        xprintf(Msg, "Deleting output of %s class.\n", it->registered_class_name.c_str());
        //        delete it->output;
        //        if(it->out_stream != NULL) {
        //            it->out_stream->close();
        //        }
    }

    /**delete of all references to registered classes */
    if (registered_classes_ != NULL){
        xprintf(Msg,"delete registered_classes_.\n");
        delete registered_classes_;
    }

    delete time_marks_output_;

    //    if(out_stream_ != NULL) {
    //        out_stream_.close();
    //    }

    delete util;
}


void CheckpointingManager::set_restore_output(std::string class_name){
    restore_output_ = set_output(class_name);
};

CheckpointingOutput* CheckpointingManager::get_restore_output(){
    return restore_output_;
};

void CheckpointingManager::register_class(EquationBase* ch, std::string class_name){
    xprintf(Msg, "CheckpointingManager obj:%p, name:%s \n", ch, class_name.c_str());
    if (!is_checkpointing_on()) return ;
    xprintf(Msg, "CheckpointingManager is_checkpointing_on \n");

    RegisteredClass obj;// = new RegisteredClass();
    obj.registered_class = ch;
    obj.registered_class_name = ch->class_name();
    //    obj.out_stream = set_out_stream(class_name);
    //    obj.output = set_output(class_name);
    registered_classes_->push_back(obj);

}

void CheckpointingManager::save_state(){
    if(!is_checkpointing_on()) return;

    last_checkpointing_time_ = time(NULL);
    xprintf(Msg, "Rozdíl: %i\n", (start_time_-last_checkpointing_time_));

    std::string id = boost::lexical_cast<string>(checkpoint_);
    std::string file_name;

    std::ofstream  out_stream;

    xprintf(Msg, "Názvy souborů: %s, %s\n", id.c_str(), file_name.c_str());

    /** Saving global TimeMarks instance, they are global for all other instances */
    save_time_marks(id);


    for(RegisteredClasses::iterator it = registered_classes_->begin(); it != registered_classes_->end(); ++it){
        xprintf(Msg, "Saving state of object: %s\n", it->registered_class_name.c_str());
        TimeMark::Type checkpointing_mark;
        checkpointing_mark = marks_->type_checkpointing()|marks_->type_fixed_time();//|it->obj->mark_type();

        /** for each class its own stream and Boost archive*/
        file_name = util->full_file_name_id(it->registered_class->class_name(), id);
        xprintf(Msg, "Názvy souborů: %s, %s\n", id.c_str(), file_name.c_str());
        out_stream.open(file_name.c_str());

        INPUT_CHECK(out_stream.is_open() , "Can not open output file: %s\n", file_name.c_str() );

        boost::archive::text_oarchive oa((out_stream));//xml_oarchive oa(ofs);
//        boost::archive::xml_oarchive oa((out_stream));//xml_oarchive oa(ofs);
        oa << it->registered_class;
        if(out_stream!=NULL){
            out_stream.close();
        }

        output_ = set_output(it->registered_class_name);

        /**Petsc vectors can not be serialized, they have its own methods for saving/restoring*/
        /**TODO serialize Petsc vectors*/
        it->registered_class->save_state(output_);

        delete output_;

    }

    /**
     * TODO checkpoints management
     */
    checkpoint_++;

};

//void CheckpointingManager::save_time_marks(std::ofstream out_stream){
//    boost::archive::text_oarchive oa((out_stream));//xml_oarchive oa(ofs);
//    oa << marks_;
//}

void CheckpointingManager::save_time_marks(std::string id){
    std::ofstream out_stream;
    std::string file_name;
    std::string class_name = "TimeMarks";

    file_name = util->full_file_name_id(class_name, id);

    out_stream.open(file_name.c_str());

    INPUT_CHECK(out_stream.is_open() , "Can not open output file: %s\n", file_name.c_str() );

    boost::archive::text_oarchive oa((out_stream));//xml_oarchive oa(ofs);
    oa << marks_;
    if(out_stream!=NULL){
        out_stream.close();
    }

}


void CheckpointingManager::restore_time_marks(){
    xprintf(Msg,"CheckpointingManager::restore_time_marks:\n");
    /**works with marks_ pointer */
    if (!is_checkpointing_on()) return;
    //    time_marks_output_ = set_output("TimeMarks");
    time_marks_output_->load_data(marks_);
    //    delete time_marks_output_;
}


void CheckpointingManager::restore_state(){
    if(!is_checkpointing_on()) return;

    for(RegisteredClasses::iterator it = registered_classes_->begin(); it != registered_classes_->end(); ++it){
        xprintf(Msg, "Restoring state of object: %s\n", it->registered_class_name.c_str());

        it->registered_class->restore_state(it->output);//checkpoint_
    }

};


//void CheckpointingManager::create_timemarks(){//TimeMarks* marks
//    if (!is_checkpointing_on()) return ;
//
//
//};

void CheckpointingManager::create_fixed_timemarks(){
    if (!is_checkpointing_on()) return;
    if (checkpoints_type_ == CH_FIXED){
        double begin_time;
        double end_time;
        double number_of_marks;

        begin_time=0;

        /**end_time se buď dá získat z .ini, nebo je v každém TG*/
        end_time=OptGetDbl("Global", "Stop_time", "1.0");
        number_of_marks=OptGetInt("Checkpointing", "Number_of_checkpoints", "1");;

        TimeMark::Type checkpointing_mark;
        checkpointing_mark = marks_->type_checkpointing()|marks_->type_fixed_time();
        for(RegisteredClasses::iterator it = registered_classes_->begin(); it != registered_classes_->end(); ++it)
            checkpointing_mark |= it->registered_class->mark_type();
        for (double t = begin_time; t < end_time; t += (end_time-begin_time)/number_of_marks) {
            xprintf(Msg,"Přidávám marku v:%f, typu:%i\n", t, checkpointing_mark);
            marks_->add(TimeMark(t, checkpointing_mark));//marks->type_checkpointing()|marks->type_fixed_time()
        }
    }

};

void CheckpointingManager::create_dynamic_timemark(){
    if (true) return;
    if (!is_checkpointing_on()) return;
    if (checkpoints_type_ == CH_DYNAMIC){
        time_t current_time;
        current_time = time(NULL);
        xprintf(Msg,"Times: start:%i, last:%i, current:%i, interval:%i\n", start_time_, last_checkpointing_time_, current_time, checkpoints_interval_);
        if(current_time - last_checkpointing_time_ >= checkpoints_interval_){
            /**time since last checkpoint was saved, exceeds checkpoints_interval_ --> creating new Checkpointing TimeMark*/
        }

        //        double begin_time;
        //        double end_time;
        //        double number_of_marks;
        //
        //        begin_time=0;
        //
        //        /**end_time se buď dá získat z .ini, nebo je v každém TG*/
        //        end_time=OptGetDbl("Global", "Stop_time", "1.0");
        //        number_of_marks=OptGetInt("Checkpointing", "Number_of_checkpoints", "1");;
        //
        //        TimeMark::Type checkpointing_mark;
        //        checkpointing_mark = marks_->type_checkpointing()|marks_->type_fixed_time();
        //        for(RegisteredClasses::iterator it = registered_classes_->begin(); it != registered_classes_->end(); ++it)
        //            checkpointing_mark |= it->registered_class->mark_type();
        //        for (double t = begin_time; t < end_time; t += (end_time-begin_time)/number_of_marks) {
        //            xprintf(Msg,"Přidávám marku v:%f, typu:%i\n", t, checkpointing_mark);
        //            marks_->add(TimeMark(t, checkpointing_mark));//marks->type_checkpointing()|marks->type_fixed_time()
        //        }
    }

};


int CheckpointingManager::checkpoint(){
    return checkpoint_;
};

void CheckpointingManager::next_checkpoint(){
    checkpoint_++;
};

/******************************************************************************************************/
/*** CheckpointingBase  *******************************************************************************/

CheckpointingOutput* CheckpointingManager::set_output(std::string class_name){
    xprintf(Msg,"CheckpointingBase::set_output - %s.\n", class_name.c_str());

    CheckpointingOutput* output;

    switch (out_file_format_) {
    case CH_OUTPUT_TXT: {
        xprintf(Msg,"Output TXT.\n");
        output = new CheckpointingOutputTxt(class_name);
    }
    break;
    case CH_OUTPUT_BIN: {
        xprintf(Msg,"Output BIN.\n");
        output = new CheckpointingOutputBin(class_name);
    }
    break;
    //    case CH_OUTPUT_JSON: output = new CheckpointingOutputJSON(fileName);
    //    break;
    default: output = new CheckpointingOutputTxt(class_name);
    break;
    }

    return output;
};

std::ofstream* CheckpointingManager::set_out_stream(std::string class_name, std::string checkpoint_number){
    xprintf(Msg,"CheckpointingManager::set_out_stream - %s, ch#: %s.\n", class_name.c_str(), checkpoint_number.c_str());

//    util = new CheckpointingUtil();
    std::ofstream* out_stream;
    std::string out_stream_path;
    out_stream_path = util->full_file_name_id(class_name, checkpoint_number);
    out_stream->open(out_stream_path.c_str());
    INPUT_CHECK(out_stream->is_open() , "Can not open output file: %s\n", out_stream_path.c_str() );

//    delete util;
    return out_stream;
};

bool CheckpointingManager::is_checkpointing_on(){
    return checkpointing_on_;
};

/*************************************************************************************************/
/**Supplementary methods *************************************************************************/
CheckpointingOutFileFormat CheckpointingManager::parse_output_format(char* format_name){
    if(strcmp(format_name,"TXT") == 0) {
        return CH_OUTPUT_TXT;
    } else if(strcmp(format_name,"BIN") == 0) {
        return CH_OUTPUT_BIN;
    } else if(strcmp(format_name,"JSON") == 0) {
        return CH_OUTPUT_JSON;
    } else {
        xprintf(Warn,"Unknown output file format: %s.\n", format_name );
        return (CH_OUTPUT_TXT);
    }
}

CheckpointsType CheckpointingManager::parse_checkpoints_type(char* format_name){
    if(strcmp(format_name,"FIXED") == 0) {
        return CH_FIXED;
    } else if(strcmp(format_name,"DYNAMIC") == 0) {
        return CH_DYNAMIC;
    } else {
        xprintf(Warn,"Unknown output file format: %s.\n", format_name );
        return (CH_FIXED);
    }
}
