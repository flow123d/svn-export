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
 * $Id: checkpointing_base.h 1458 2011-12-10 19:34:14Z vojtech.wrnata $
 * $Revision: 1458 $
 * $LastChangedBy: vojtech.wrnata $
 * $LastChangedDate: 2011-12-10 20:34:14 +0100 (So, 10 pro 2011) $
 *
 * @file    checkpointing_base.h
 * @brief   Header: The functions for checkpointing_base class.
 *
 */

#ifndef CHECKPOINING_BASE_H_
#define CHECKPOINING_BASE_H_

#include "system/system.hh"
#include "CheckpointingUtil.h"
#include "CheckpointingOutput.h"
#include "CheckpointingOutputTxt.h"
#include "CheckpointingOutputBin.h"
//#include <time.h>

//#include <time_marks.hh>
//#include <time_governor.hh>


class Checkpointing {
public:
    /** \brief Default constructor for Checkpointing class */
    Checkpointing(string fileName);//string fileName

    /** \brief Default copy constructor for Checkpointing class */
    Checkpointing(const Checkpointing* ch);

    /** \brief Default destructor for Checkpointing class */
    virtual ~Checkpointing();

    /** \brief public getter for output_ */
    CheckpointingOutput* get_output() const;

    /** \brief public getter for outFileFormat */
    CheckpointingOutFileFormat get_out_file_format();

    /** \brief public getter for CheckpointingOn */
    bool is_checkpointing_on();

private:
    CheckpointingOutput* output_;


    /** \brief Parse values from .ini file to OutFielFormat\n
     * parameters are stored in section-[Checkpointing], key-Output_format
     * \param ini value TXT --> CH_OUTPUT_TXT
     * \param ini value BIN --> CH_OUTPUT_BIN
     * \param ini value JSON --> CH_OUTPUT_JSON  */
    CheckpointingOutFileFormat parse_output_format(char* format_name);

    /** \brief Initialize output object via outFileFormat\n
     * CH_OUTPUT_TXT --> CheckpointingOutputTxt()
     * CH_OUTPUT_BIN --> CheckpointingOutputBin()
     * \param CheckpointingOutput* output - output object to be initialized */
    CheckpointingOutput* set_output();

    /** \brief Current output file format*/
    CheckpointingOutFileFormat out_file_format_;

    /** \brief Checkpointing enabled/disabled */
    bool checkpointing_on_;

    /** \brief Base name of the file for storing Vectors, Matrixes TimeGovernor and other variables\n
     * Usually it should be name of the class, where Vectors, Matrixes TimeGovernor and other variables belongs to.\
     * it is used for generating file name (util->fullFileName()) */
    std::string file_name_;


protected:

};


#endif /* CHECKPOINING_BASE_H_ */

