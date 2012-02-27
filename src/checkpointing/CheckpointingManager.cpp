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

CheckpointingManager::CheckpointingManager(TimeMarks* marks) {//TimeMarks* marks
    xprintf(Msg,"CheckpointingManager constructor.\n");

    this->marks_ = marks;

    registered_classes_ = new RegisteredClasses();

    start_time_ = time(NULL);
    xprintf(Msg,"CheckpointingManager construction time %i\n", start_time_);

    static_timemarks = true;

    if (static_timemarks){
        create_timemarks();//marks
    };

    max_checkpoints_ = OptGetDbl("Checkpointing", "Max_checkpoints", "1");
    /**restriction for max 10 saved checkpoints*/
    if (max_checkpoints_ > 10){
        max_checkpoints_ = 10;
    };
    checkpoint_ = 1;
}

CheckpointingManager::~CheckpointingManager() {
    xprintf(Msg,"CheckpointingManager destructor.\n");

    if (registered_classes_ != NULL){
        xprintf(Msg,"delete registered_classes_.\n");
        delete registered_classes_;
    }
}

void CheckpointingManager::register_class(CheckpointingBase* ch, std::string class_name){
    xprintf(Msg, "obj:%p, name: \n", ch);
    RegisteredClass obj;// = new RegisteredClass();
    obj.registered_class = ch;
    obj.registered_class_name = class_name;
    registered_classes_->push_back(obj);

}

void CheckpointingManager::create_timemarks(){//TimeMarks* marks
    double begin_time;
    double end_time;
    double number_of_marks;
    begin_time=0;

    /**end_time se buď dá získat z .ini, nebo je v každém TG*/
    end_time=OptGetDbl("Global", "Stop_time", "1.0");
    number_of_marks=OptGetInt("Checkpointing", "Number_of_static_marks", "1");;

    TimeMark::Type checkpointing_mark;
    checkpointing_mark = marks_->type_checkpointing()|marks_->type_fixed_time();
    for(RegisteredClasses::iterator it = registered_classes_->begin(); it != registered_classes_->end(); ++it)
        checkpointing_mark |= it->registered_class->mark_type();
    for (double t = begin_time; t < end_time; t += (end_time-begin_time)/number_of_marks) {
        xprintf(Msg,"Přidávám marku v:%f, typu:%i\n", t, checkpointing_mark);
        marks_->add(TimeMark(t, checkpointing_mark));//marks->type_checkpointing()|marks->type_fixed_time()
    }

};

void CheckpointingManager::save_state(){
    last_checkpointing_time_ = time(NULL);
    xprintf(Msg, "Rozdíl: %i\n", (start_time_-last_checkpointing_time_));

    /**
     * TODO tady by se asi měly ukládat TimeMarks - protože jsou globální pro všechny třídy
     * */

    for(RegisteredClasses::iterator it = registered_classes_->begin(); it != registered_classes_->end(); ++it){
        xprintf(Msg, "Saving state of object: %s\n", it->registered_class_name.c_str());
        TimeMark::Type checkpointing_mark;
        checkpointing_mark = marks_->type_checkpointing()|marks_->type_fixed_time();//|it->obj->mark_type();
//        if(marks_->is_current(it->registered_class->time(), checkpointing_mark)){
            it->registered_class->save_state();//checkpoint_
//        }
    }

};

int CheckpointingManager::checkpoint(){
    return checkpoint_;
};

void CheckpointingManager::next_checkpoint(){
    checkpoint_++;
};
