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

CheckpointingManager::CheckpointingManager() {
    xprintf(Msg,"CheckpointingManager constructor.\n");

}

CheckpointingManager::~CheckpointingManager() {
    xprintf(Msg,"CheckpointingManager destructor.\n");

    delete registered_classes;
}


void CheckpointingManager::create_timemarks(TimeMarks* marks, double begin_time, double end_time, double number_of_marks){

    for (double t = begin_time; t < end_time; t += (end_time-begin_time)/number_of_marks) {
        marks->add(TimeMark(t, marks->type_checkpointing()));
    }


};
