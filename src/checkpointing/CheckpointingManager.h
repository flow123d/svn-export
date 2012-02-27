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
 * $Id: CheckpointingManager.h 1458 2011-12-10 19:34:14Z vojtech.wrnata $
 * $Revision: 1458 $
 * $LastChangedBy: vojtech.wrnata $
 * $LastChangedDate: 2011-12-10 20:34:14 +0100 (So, 10 pro 2011) $
 *
 * @file    CheckpointingManager.h
 * @brief   Header: The functions for CheckpointingManager class.
 *
 */

#ifndef CHECKPOINTINGMANAGER_H_
#define CHECKPOINTINGMANAGER_H_

#include <time.h>
//#include <time_marks.hh>
//#include "CheckpointingUtil.h"
#include "CheckpointingBase.h"

/**\brief Definition of registered class structure */
typedef struct RegisteredClass{
    /** pointer to registered class */
    CheckpointingBase*  registered_class;
    /**name of registered class*/
    std::string         registered_class_name;
}_RegisteredClass;

/**\brief Definition of registered classes vector */
typedef std::vector<RegisteredClass> RegisteredClasses;

/** Checkpointing manager class.
 */
class CheckpointingManager {
public:
    CheckpointingManager(TimeMarks* marks);//TimeMarks* marks
    ~CheckpointingManager();

    void register_class(CheckpointingBase* ch, std::string class_name);

    void save_state();

    //    void create_timemarks(TimeMarks* marks, double begin_time, double end_time, double number_of_marks);

    /**\ brief creates checkpointing marks for all registered classes
     * it has to be called after all classes are registered
     */
    void create_timemarks();//TimeMarks* marks

    /**\brief public getter to checkpoint_ value*/
    int checkpoint();

private:
    RegisteredClasses *registered_classes_;
    bool static_timemarks;

    /**\brief pointer to main_time_marks for internal use*/
    TimeMarks* marks_;

    /** \brief time when simulation started (CheckpointingManager was constructed)
     * it's used for computing dynamic TimeMarks
     * it's in seconds since January 1, 1970!
     */
    time_t start_time_;

    /** \brief time when last checkpointing was saved (save_state method was called)
     * it is used for computing dynamic TimeMarks   */
    time_t last_checkpointing_time_;

    /**\brief max number of stored checkpointing states
     * @default value = 1
     * can be setup in flow.ini*/
    int max_checkpoints_;

    /**\brief next checkpointing state
     * value is used in each file name, where values are saved, regardless of data type (txt, bin, ...)*/
    int checkpoint_;

    void next_checkpoint();

};

#endif /* CHECKPOINTINGMANAGER_H_ */
