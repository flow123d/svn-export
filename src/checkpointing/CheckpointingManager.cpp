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
    static_timemarks = true;

    if (static_timemarks){
        create_timemarks(marks);
    };

}

CheckpointingManager::~CheckpointingManager() {
    xprintf(Msg,"CheckpointingManager destructor.\n");

//    if (registered_classes != NULL){
//        delete registered_classes;
//    }
}


void CheckpointingManager::create_timemarks(TimeMarks* marks){
    double begin_time;
    double end_time;
    double number_of_marks;
    begin_time=0;
    end_time=2;
    number_of_marks=5;
    for (double t = begin_time; t < end_time; t += (end_time-begin_time)/number_of_marks) {
        xprintf(Msg,"Přidávám marku v:%f, typu:%i\n", t, marks->type_checkpointing());
        marks->add(TimeMark(t, marks->type_checkpointing()|marks->type_fixed_time()));
    }


};
