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
 * $Id: CheckpointingOutputTxt.cpp $
 * $Revision: 0 $
 * $LastChangedBy: vojtech.wrnata $
 * $LastChangedDate: 2012-1-4 20:34:14 +0100 (So, 10 pro 2011) $
 *
 * @file    CheckpointingOutputTxt.c
 * @brief   Header: The class for CheckpointingOutputTxt.
 *
 */

#include "CheckpointingOutputTxt.h"

CheckpointingOutputTxt::CheckpointingOutputTxt(string file_name):
CheckpointingOutput(file_name){
    xprintf(Msg,"CheckpointingOutputTxt::CheckpointingOutputTxt.\n");
    out_stream.open(util->full_file_name(file_name).c_str());
    INPUT_CHECK( out_stream.is_open() , "Can not open output file: %s\n", util->full_file_name(file_name).c_str() );
    xprintf(Msg,"CheckpointingOutputTxt::CheckpointingOutputTxt - constructed.\n");

}

CheckpointingOutputTxt::~CheckpointingOutputTxt() {
    if(out_stream != NULL) {
        out_stream.close();
        //        delete outStream;
    }
}

/**
 * \brief General method for storing petsc Vector
 * \param[out] Vec - petsc Vector to be loaded
 * \param[in] vecName - name of the loaded vector     */
void CheckpointingOutputTxt::save_vec(Vec vec, std::string vec_name){
    save_vec_txt(vec, vec_name);
};

/**
 * \brief General method for loading petsc Vector
 * \param[out] Vec - petsc Vector to be loaded
 * \param[in] vecName - name of the loaded vector     */
void CheckpointingOutputTxt::load_vec(Vec vec, std::string vec_name){
    load_vec_txt(vec, vec_name);
};

/**
 * \brief General method for storing petsc Matrix
 * \param Mat - petsc Matrix to be stored
 * \param matName - name of the stored matrix     */
void CheckpointingOutputTxt::save_mat(Mat mat, std::string mat_name){
    save_mat_txt(mat, mat_name);
};

/**
 * \brief General method for loading petsc Matrix
 * \param Mat - petsc Matrix to be stored
 * \param matName - name of the stored matrix     */
void CheckpointingOutputTxt::load_mat(Mat mat, std::string mat_name){
    load_mat_txt(mat, mat_name);
};

/**
 * \brief General method for storing TimeGovernor object
 * \param TimeGovernor - pointer to TimeGovernor instance */
void CheckpointingOutputTxt::save_timegovernor(TimeGovernor* tg){
    xprintf(Msg, "CheckpointingOutputTxt::save_timegovernor");
    save_timegovernor_txt(tg);
};

/**
 * \brief General method for loading/restoring TimeGovernor object
 * \param TimeGovernor - pointer to TimeGovernor instance */
void CheckpointingOutputTxt::load_timegovernor(TimeGovernor* tg){
    load_timegovernor_txt(tg);
};

template <typename T>
    void CheckpointingOutputTxt::save_data(T &data){//, std::string name
    xprintf(Msg, "CheckpointingOutputTxt::save_data(T &data, ..) - Až to tady bude, tak to uložím :-)\n");
};

void CheckpointingOutputTxt::save_data(TimeGovernor* tg){
    save_timegovernor_txt(tg);
};
void CheckpointingOutputTxt::save_data(TimeMark* time_mark){
    save_timemark_txt(time_mark);
};
void CheckpointingOutputTxt::save_data(TimeMarks* time_marks){
    save_timemarks_txt(time_marks);
};
void CheckpointingOutputTxt::save_data(double &data){
    xprintf(Msg, "netemplejtovaná CheckpointingOutputTxt::save_data<double>(double &data, ..) -Až to tady bude, tak to uložím :-)\n");
    save_double_txt(data);
};


/******************************************************************************************/
/**Private methods for physical saving/loading ********************************************/
void CheckpointingOutputTxt::save_vec_txt(Vec vec, std::string vec_name){};
void CheckpointingOutputTxt::load_vec_txt(Vec vec, std::string vec_name){};
void CheckpointingOutputTxt::save_mat_txt(Mat mat, std::string mat_name){};
void CheckpointingOutputTxt::load_mat_txt(Mat mat, std::string mat_name){};
void CheckpointingOutputTxt::save_timegovernor_txt(TimeGovernor* tg){};
void CheckpointingOutputTxt::load_timegovernor_txt(TimeGovernor* tg){};
void CheckpointingOutputTxt::save_timemarks_txt(TimeMarks* time_marks){};
void CheckpointingOutputTxt::load_timemarks_txt(TimeMarks* time_marks){};
void CheckpointingOutputTxt::save_timemark_txt(TimeMark* time_mark){};
void CheckpointingOutputTxt::load_timemark_txt(TimeMark* time_mark){};
void CheckpointingOutputTxt::save_double_txt(double& data){};
void CheckpointingOutputTxt::load_double_txt(double& data){};


//template void save_data<double>(double);


