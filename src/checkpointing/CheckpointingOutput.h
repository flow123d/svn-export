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
 * $Id: CheckpointingOutput.h 1458 2011-12-10 19:34:14Z vojtech.wrnata $
 * $Revision: 1458 $
 * $LastChangedBy: vojtech.wrnata $
 * $LastChangedDate: 2011-12-10 20:34:14 +0100 (So, 10 pro 2011) $
 *
 * @file    checkpointing_base.h
 * @brief   Header: Base output functions class.
 *
 */

#ifndef CHECKPOINTINGOUTPUT_H_
#define CHECKPOINTINGOUTPUT_H_

#include "CheckpointingUtil.h"
#include "system/system.hh"
#include <fstream>
//#include <time_marks.hh>
#include <time_governor.hh>

class CheckpointingOutput {
public:
    /** \brief Default constructor for CheckpointingOutput class
     * \param string fileName - name of file for storing */
    CheckpointingOutput(string file_name);

    /** \brief Default destructor for CheckpointingOutput class */
    virtual ~CheckpointingOutput();

    /**
     * \brief General method for storing petsc Vector
     * \param[in] Vec - petsc Vector to be stored
     * \param[in] vecName - name of the stored vector     */
    virtual void save_data(Vec vec, std::string vecName);

    /**
     * \brief General method for loading petsc Vector
     * \param[out] Vec - petsc Vector to be loaded
     * \param[in] vecName - name of the loaded vector     */
    virtual void load_data(Vec vec, std::string vecName);

    /**
     * \brief General method for storing petsc Matrix
     * \param Mat - petsc Matrix to be stored
     * \param matName - name of the stored matrix     */
    virtual void save_data(Mat mat, std::string matName);

    /**
     * \brief General method for loading petsc Matrix
     * \param Mat - petsc Matrix to be stored
     * \param matName - name of the stored matrix     */
    virtual void load_data(Mat mat, std::string matName);

    /**
     * \brief General method for storing TimeGovernor object
     * \param TimeGovernor - pointer to TimeGovernor instance */
    virtual void save_data(TimeGovernor* tg);

    /**
     * \brief General method for loading/restoring TimeGovernor object
     * \param TimeGovernor - pointer to TimeGovernor instance */
    virtual void load_data(TimeGovernor* tg);

    /**
     * \brief General method for storing TimeMarks vector
     * \param TimeMarks - pointer to TimeMarks instance */
    virtual void save_data(TimeMarks* time_marks);

    /**
     * \brief General method for loading/restoring TimeMarks vector
     * \param TimeMarks - pointer to TimeMarks instance */
    virtual void load_data(TimeMarks* time_marks);

    /**
     * \brief General method for storing double data
     * \param double - pointer to double instance */
    virtual void save_data(double& data, std::string name);

    /**
     * \brief General method for loading/restoring double data
     * \param double - pointer to double instance */
    virtual void load_data(double& data, std::string name);

    /**
     * \brief General method for storing int data
     * \param int - pointer to int instance */
    virtual void save_data(int& data, std::string name);

    /**
     * \brief General method for loading/restoring int data
     * \param int - pointer to int instance */
    virtual void load_data(int& data, std::string name);

    /**
     * \brief General method for storing int data
     * \param int - pointer to int instance */
    virtual void save_data(const TimeMark::Type& data, std::string name);

    /**
     * \brief General method for loading/restoring int data
     * \param int - pointer to int instance */
    virtual void load_data(TimeMark::Type& data, std::string name);



    /**
     * \brief General template method for storing values\n
     * there has to be explicit specialization for each used data type
     * \param &data - value to be stored */
//    template <typename T>
//    void save_data(T &data);//, std::string name
//
//
//    virtual void save_data(double &data);



protected:
    CheckpointingUtil* util;

    /** \brief Base name of the file for storing Vectors, Matrixes TimeGovernor and other variables\n
     * Usually it should be name of the class, where Vectors, Matrixes TimeGovernor and other variables belongs to.\
     * it is used for generating file name (util->fullFileName()) */
    std::string file_name;

    /** \brief default output file stream\n
     * not used for Vec, Mat or TimeGovernor */
    ofstream    out_stream;

    /** \brief default output file stream\n
     * not used for Vec, Mat or TimeGovernor */
    ifstream    in_stream;

private:


};

#endif /* CHECKPOINTINGOUTPUT_H_ */
