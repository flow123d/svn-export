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
 * $Id: CheckpointingOutputTxt.h $
 * $Revision: 0 $
 * $LastChangedBy: vojtech.wrnata $
 * $LastChangedDate: 2012-1-4 20:34:14 +0100 (So, 10 pro 2011) $
 *
 * @file    CheckpointingOutputTxt.h
 * @brief   Header: The class for CheckpointingOutputTxt.
 *
 */

#ifndef CHECKPOINTINGOUTPUTTXT_H_
#define CHECKPOINTINGOUTPUTTXT_H_

#include "CheckpointingOutput.h"

class CheckpointingOutputTxt: public CheckpointingOutput {
public:
    CheckpointingOutputTxt(string file_name);
    virtual ~CheckpointingOutputTxt();

    /**
     * \brief General method for storing petsc Vector
     * \param[in] Vec - petsc Vector to be stored
     * \param[in] vecName - name of the stored vector     */
    void save_data(Vec vec, std::string vec_name);

    /**
     * \brief General method for loading petsc Vector
     * \param[out] Vec - petsc Vector to be loaded
     * \param[in] vecName - name of the loaded vector     */
    void load_data(Vec vec, std::string vec_name);

    /**
     * \brief General method for storing petsc Matrix
     * \param Mat - petsc Matrix to be stored
     * \param matName - name of the stored matrix     */
    void save_data(Mat mat, std::string mat_name);

    /**
     * \brief General method for loading petsc Matrix
     * \param Mat - petsc Matrix to be stored
     * \param matName - name of the stored matrix     */
    void load_data(Mat mat, std::string mat_name);

    /**
     * \brief General method for storing TimeGovernor object
     * !DOES NOT store TimeMarks
     * \param TimeGovernor - pointer to TimeGovernor instance */
    void save_data(TimeGovernor* tg);

    /**
     * \brief General method for loading/restoring TimeGovernor object
     * \param TimeGovernor - pointer to TimeGovernor instance */
    void load_data(TimeGovernor* tg);

    void save_data(TimeMarks* time_marks);
    void load_data(TimeMarks* time_marks);
    void save_data(double& data);
    void load_data(double& data);

    void save_data(int& data);
    void load_data(int& data);


//    void save_data(TimeMark* time_mark);



private:
    void save_vec_txt(Vec vec, std::string vec_name);
    void load_vec_txt(Vec vec, std::string vec_name);
    void save_mat_txt(Mat mat, std::string mat_name);
    void load_mat_txt(Mat mat, std::string mat_name);

    void save_timegovernor_txt(TimeGovernor* tg);
    void load_timegovernor_txt(TimeGovernor* tg);
    void save_timemarks_txt(TimeMarks* time_marks);
    void load_timemarks_txt(TimeMarks* time_marks);
    void save_timemark_txt(const TimeMark &time_mark, ofstream& out_stream);
    void load_timemark_txt(TimeMark* time_mark, ifstream& in_stream);
    void save_double_txt(double& data);
    void load_double_txt(double& data);
    void save_int_txt(int& data);
    void load_int_txt(int& data);
};

#endif /* CHECKPOINTINGOUTPUTTXT_H_ */
