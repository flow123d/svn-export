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
 * $Id: CheckpointingOutput.cpp $
 * $Revision: 0 $
 * $LastChangedBy: vojtech.wrnata $
 * $LastChangedDate: 2012-1-4 20:34:14 +0100 (So, 10 pro 2011) $
 *
 * @file    CheckpointingOutput.cpp
 * @brief   Output class for CheckpointingOutput.
 *
 */

#include "CheckpointingOutput.h"


CheckpointingOutput::CheckpointingOutput(string file_name) {
    xprintf(Msg,"CheckpointingOutput::CheckpointingOutput.\n");
    this->file_name = file_name;
    util = new CheckpointingUtil(file_name);

}

CheckpointingOutput::~CheckpointingOutput() {
    delete util;
}

void CheckpointingOutput::save_data(Vec vec, std::string vec_name){};
void CheckpointingOutput::load_data(Vec vec, std::string vec_Name){};
void CheckpointingOutput::save_data(Mat mat, std::string mat_name){};
void CheckpointingOutput::load_data(Mat mat, std::string mat_name){};
void CheckpointingOutput::save_data(TimeGovernor* tg){
    xprintf(Msg, "CheckpointingOutput::save_data timegovernor");
};
void CheckpointingOutput::load_data(TimeGovernor* tg){
    xprintf(Msg, "CheckpointingOutput::load_data timegovernor");
};
void CheckpointingOutput::save_data(TimeMarks* time_marks){//, std::string name
    xprintf(Msg, "CheckpointingOutput::save_data(TimeMarks* time_marks) - Až to tady bude, tak to uložím :-)\n");
};
void CheckpointingOutput::load_data(TimeMarks* time_marks){//, std::string name
    xprintf(Msg, "CheckpointingOutput::load_data(TimeMarks* time_marks) - Až to tady bude, tak to uložím :-)\n");
};

void CheckpointingOutput::save_data(double& data){//, std::string name
    xprintf(Msg, "CheckpointingOutput::save_data(double* data) - Až to tady bude, tak to uložím :-)\n");
};
void CheckpointingOutput::load_data(double& data){//, std::string name
    xprintf(Msg, "CheckpointingOutput::load_data(double* data) - Až to tady bude, tak to uložím :-)\n");
};
void CheckpointingOutput::save_data(int& data){//, std::string name
    xprintf(Msg, "CheckpointingOutput::save_data(int* data) - Až to tady bude, tak to uložím :-)\n");
};
void CheckpointingOutput::load_data(int& data){//, std::string name
    xprintf(Msg, "CheckpointingOutput::load_data(int* data) - Až to tady bude, tak to uložím :-)\n");
};


//template <typename T>
//void CheckpointingOutput::save_data(T &data){//, std::string name
//    xprintf(Msg, "CheckpointingOutput::save_data(T &data, ..) - Až to tady bude, tak to uložím :-)\n");
//};
//template void save_data<double>(double);//, std::string name

//template <>
//void CheckpointingOutput::save_data<double>(double &data){//, std::string name
//    xprintf(Msg, "CheckpointingOutput::save_data<double>(double &data, ..) - explicitní specializace -Až to tady bude, tak to uložím :-)\n");
//};




