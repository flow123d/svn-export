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

#include "equation.hh"
#include <time.h>
//#include <time_marks.hh>
#include "CheckpointingUtil.h"
#include "CheckpointingOutput.h"
#include "CheckpointingOutputTxt.h"
#include "CheckpointingOutputBin.h"

/**\brief Definition of registered class structure */
typedef struct RegisteredClass{
    /** pointer to registered class */
    EquationBase*         registered_class;
    /** pointer to output class */
    CheckpointingOutput*  output;
    /**name of registered class*/
    std::string           registered_class_name;
}_RegisteredClass;

/**\brief Definition of registered classes vector */
typedef std::vector<RegisteredClass> RegisteredClasses;

/** Checkpointing manager class.
 */
class CheckpointingManager {
public:
    CheckpointingManager(TimeMarks* marks);//TimeMarks* marks
    ~CheckpointingManager();

    void register_class(EquationBase* ch, std::string class_name);

    void save_state();

    /**\brief restores global TimeMarks from saved state*/
    void restore_time_marks();


    //    void create_timemarks(TimeMarks* marks, double begin_time, double end_time, double number_of_marks);

    /**\ brief creates FIXED checkpointing marks for all registered classes
     * it has to be called after all classes are registered    */
    void create_fixed_timemarks();//TimeMarks* marks

    /**\ brief creates DYNAMIC checkpointing mark for all registered classes
     * this mark is based on start_time_, last_checkpointing_time_, Checkpoints_interval and current time*/
    void create_dynamic_timemark();

    /**\brief public getter to checkpoint_ value*/
    int checkpoint();

    /******************************************************************************************************/
    /*** CheckpointingBase  *******************************************************************************/

    /** \brief public getter for CheckpointingOn */
    bool is_checkpointing_on();


private:
    RegisteredClasses *registered_classes_;
//    bool static_timemarks;

    /**\brief pointer to main_time_marks for internal use*/
    TimeMarks* marks_;

    /**\brief output for TimeMarks
     * initialized by constructor*/
    CheckpointingOutput*  time_marks_output_;

    /**\brief saves global TimeMarks*/
    void save_time_marks();



    /** \brief time when simulation started (CheckpointingManager was constructed)
     * it's used for computing DYNAMIC TimeMarks
     * it's in seconds since January 1, 1970!
     */
    time_t start_time_;

    /** \brief time when last checkpointing was saved (save_state method was called)
     * it is used for computing dynamic TimeMarks   */
    time_t last_checkpointing_time_;

    /**\brief max number of stored checkpointing states, when DYNAMIC checkpoints are selected
     * @default value = 1
     * can be setup in flow.ini*/
    int max_checkpoints_;

    /**\brief Time interval for Checkpoints. How often is checkpoint stored.[seconds]
     * @default value = 3600
     * can be setup in flow.ini*/
    int checkpoints_interval_;


    /**\brief next checkpointing state
     * value is used in each file name, where values are saved, regardless of data type (txt, bin, ...)*/
    int checkpoint_;

    void next_checkpoint();

    /******************************************************************************************************/
    /*** CheckpointingBase  *******************************************************************************/
    CheckpointingOutput* output_;


    /** \brief Parse values from .ini file to OutFielFormat\n
     * parameters are stored in section-[CheckpointingBase], key-Output_format
     * \param ini value TXT --> CH_OUTPUT_TXT
     * \param ini value BIN --> CH_OUTPUT_BIN
     * \param ini value JSON --> CH_OUTPUT_JSON  */
    CheckpointingOutFileFormat parse_output_format(char* format_name);

    /** \brief Parse values from .ini file to CheckpointsType\n
     * parameters are stored in section-[Checkpointing], key-Checkpoints_type
     * \param ini value FIXED --> CH_FIXED
     * \param ini value DYNAMIC --> CH_DYNAMIC*/
    CheckpointsType parse_checkpoints_type(char* format_name);

    /** \brief Initialize output object via outFileFormat\n
     * CH_OUTPUT_TXT --> CheckpointingOutputTxt()
     * CH_OUTPUT_BIN --> CheckpointingOutputBin()
     * \param CheckpointingOutput* output - output object to be initialized */
    CheckpointingOutput* set_output(std::string class_name);

    /** \brief Current output file format*/
    CheckpointingOutFileFormat out_file_format_;

    /** \brief Type of checkpoints*/
    CheckpointsType checkpoints_type_;

    /** \brief CheckpointingBase enabled/disabled */
    bool checkpointing_on_;

};

#endif /* CHECKPOINTINGMANAGER_H_ */
