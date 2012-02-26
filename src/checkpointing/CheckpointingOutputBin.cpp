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
 * $Id: CheckpointingOutputBin.cpp $
 * $Revision: 0 $
 * $LastChangedBy: vojtech.wrnata $
 * $LastChangedDate: 2012-1-4 20:34:14 +0100 (So, 10 pro 2011) $
 *
 * @file    CheckpointingOutputBin.c
 * @brief   Header: The class for CheckpointingOutputBin.
 *
 */

#include "CheckpointingOutputBin.h"

CheckpointingOutputBin::CheckpointingOutputBin(string file_name)
:CheckpointingOutput(file_name) {
    xprintf(Msg,"CheckpointingOutputBin::CheckpointingOutputBin.\n");
    xprintf(Msg, "%s\n", util->tg_file_name.c_str());

    out_stream.open(util->full_file_name(file_name).c_str(), ios_base::out | ios_base::binary);
    INPUT_CHECK( out_stream.is_open() , "Can not open output file: %s\n", util->full_file_name(file_name).c_str() );
}

CheckpointingOutputBin::~CheckpointingOutputBin() {
    if(out_stream != NULL) {
        out_stream.close();
//        delete out_stream;
    }
}

void CheckpointingOutputBin::save_data(Vec vec, std::string vec_name){
    save_vec_binary(vec, vec_name);
};

/**
 * \brief General method for loading petsc Vector
 * \param[out] Vec - petsc Vector to be loaded
 * \param[in] vec_name - name of the loaded vector     */
void CheckpointingOutputBin::load_data(Vec vec, std::string vec_name){
    load_vec_binary(vec, vec_name);
};

/**
 * \brief General method for storing petsc Matrix
 * \param Mat - petsc Matrix to be stored
 * \param matName - name of the stored matrix     */
void CheckpointingOutputBin::save_data(Mat mat, std::string mat_name){
    save_mat_binary(mat, mat_name);
};

/**
 * \brief General method for loading petsc Matrix
 * \param Mat - petsc Matrix to be stored
 * \param matName - name of the stored matrix     */
void CheckpointingOutputBin::load_data(Mat mat, std::string mat_name){
    load_mat_binary(mat, mat_name);
};

/**
 * \brief General method for stroing TimeGovernor object
 * \param TimeGovernor - pointer to TimeGovernor instance */
void CheckpointingOutputBin::save_data(TimeGovernor* tg){
    save_timegovernor_binary(tg);
};

/**
 * \brief General method for loading/restoring TimeGovernor object
 * \param TimeGovernor - pointer to TimeGovernor instance */
void CheckpointingOutputBin::load_data(TimeGovernor* tg){
    xprintf(Msg, "CheckpointingOutputBin::save_timegovernor");
    load_timegovernor_binary(tg);
};

//void CheckpointingOutputBin::save_data(TimeMark* time_mark){
//    /**TODO out_stream je blbost ...*/
//    save_timemark_binary(time_mark, out_stream);
//};
void CheckpointingOutputBin::save_data(TimeMarks* time_marks){
    /**TODO in_stream je blbost ...*/
    save_timemarks_binary(*time_marks, out_stream);
};
void CheckpointingOutputBin::load_data(TimeMarks* time_marks){
    /**TODO in_stream je blbost ...*/
//    save_timemarks_binary(*time_marks, out_stream);
};
void CheckpointingOutputBin::save_data(double &data){
    save_double_binary(data);
};
void CheckpointingOutputBin::load_data(double &data){
//    save_double_binary(data);
};
void CheckpointingOutputBin::save_data(int &data){
//    save_double_binary(data);
};
void CheckpointingOutputBin::load_data(int &data){
//    save_double_binary(data);
};

/******************************************************************************************/
/**Private methods for physical saving/loading ********************************************/
void CheckpointingOutputBin::save_vec_binary(Vec vec, std::string vec_name){
    PetscInt size;
    PetscViewer    viewer;
    std::string full_name;

    full_name = util->full_file_name(vec_name);
    xprintf( Msg, "xxx full_name:%s\n", full_name.c_str());
    /**ulozeni vectoru binarne */
    PetscViewerBinaryOpen(PETSC_COMM_WORLD,full_name.c_str(),FILE_MODE_WRITE,&viewer);
    VecView(vec,viewer);

    PetscViewerDestroy(&viewer);
};
void CheckpointingOutputBin::load_vec_binary(Vec vec, std::string vec_name){
    PetscViewer    viewer;
    Vec            u;

    std::string full_name;

    full_name = util->full_file_name(vec_name);
    /**nacteni vectoru z binarniho souboru*/
    PetscViewerBinaryOpen(PETSC_COMM_WORLD,full_name.c_str(),FILE_MODE_READ,&viewer);
    //        VecCreate(PETSC_COMM_WORLD,&vec);
    VecLoad(vec,viewer);
    PetscViewerDestroy(&viewer);
    //    return u;
};
void CheckpointingOutputBin::save_mat_binary(Mat mat, std::string mat_name){
    PetscInt size;
    PetscViewer    viewer;
    std::string full_name;

    full_name = util->full_file_name(mat_name);
    xprintf( Msg, "xxx full_name:%s\n", full_name.c_str());
    /**ulozeni vectoru binarne */
    PetscViewerBinaryOpen(PETSC_COMM_WORLD,full_name.c_str(),FILE_MODE_WRITE,&viewer);
    MatView(mat,viewer);

    PetscViewerDestroy(&viewer);
};
void CheckpointingOutputBin::load_mat_binary(Mat mat, std::string mat_name){
    PetscViewer    viewer;
    Mat            m;

    std::string full_name;

    full_name = util->full_file_name(mat_name);
    /**loading matrix from binary file*/
    PetscViewerBinaryOpen(PETSC_COMM_WORLD,full_name.c_str(),FILE_MODE_READ,&viewer);
    //        VecCreate(PETSC_COMM_WORLD,&vec);
    MatLoad(mat,viewer);
    PetscViewerDestroy(&viewer);
};
void CheckpointingOutputBin::save_timegovernor_binary(TimeGovernor* tg){
    /**testovací metoda na ukládání hodnot*/
    std::string full_name;

    full_name = util->full_file_name(util->tg_file_name);
    xprintf( Msg, "****************************saveTimeGovernor full_name:%s\n", full_name.c_str());

    ofstream   file;
    file.open(full_name.c_str(), ios_base::out | ios_base::binary);
    INPUT_CHECK( file.is_open() , "Can not open output file: %s\n", full_name.c_str() );

    //tg->saveTGValuesBinary(file);

    if(file != NULL) {
        file.close();
        //        delete file;
    }
};
void CheckpointingOutputBin::load_timegovernor_binary(TimeGovernor* tg){
    /**testovací metoda na ukládání hodnot*/
    std::string full_name;

    full_name = util->full_file_name(util->tg_file_name);
    xprintf( Msg, "****************************loadTimeGovernor full_name:%s\n", full_name.c_str());

    ifstream   file;
    file.open(full_name.c_str(), ios_base::in | ios_base::binary);
    INPUT_CHECK( file.is_open() , "Can not open output file: %s\n", full_name.c_str() );

    //tg->loadTGValuesBinary(file);

    if(file != NULL) {
        file.close();
        //        delete file;
    }
};

//template <typename T>
//    void CheckpointingOutputBin::save_data(T &data){//, std::string name
//    xprintf(Msg, "CheckpointingOutputBin::save_data(T &data, ..) - Až to tady bude, tak to uložím :-)\n");
//};

void CheckpointingOutputBin::save_timemarks_binary(TimeMarks& time_marks, ofstream& out_stream){//, std::string name
    xprintf(Msg, "CheckpointingOutputBin::save_timemarks_binary(T &data, ..)\n");
//    for(vector<TimeMark>::const_iterator it = time_marks.get_marks().begin(); it != time_marks.get_marks().end(); ++it){
//        save_timemark_binary(it, out_stream);
//    }
};
void CheckpointingOutputBin::save_timemark_binary(TimeMark* time_mark, ofstream& out_stream){//, std::string name
    xprintf(Msg, "CheckpointingOutputBin::save_timemark_binary(T &data, ..)\n");
    double tm, tt;
    tm = time_mark->time();
    tt = time_mark->mark_type();
    out_stream.write((char*)&tm, sizeof(tm));
    out_stream.write((char*)&tt, sizeof(tt));
};
void CheckpointingOutputBin::load_timemark_binary(TimeMark* time_mark, ifstream& in_stream){//, std::string name
    xprintf(Msg, "CheckpointingOutputBin::load_timemark_binary(TimeMark* time_mark, ..))\n");
//    in_stream.read((char*)&time_mark->time(), sizeof(time_mark->time()));
//    in_stream.read((char*)&time_mark->mark_type(), sizeof(time_mark->mark_type()));
};
void CheckpointingOutputBin::save_double_binary(double &data){//, std::string name
    xprintf(Msg, "CheckpointingOutputBin::save_double_binary(double &data) - netemplejtovaná metoda - Až to tady bude, tak to uložím :-)\n");
    out_stream.write((char*)&data, sizeof(data));
};
void CheckpointingOutputBin::load_double_binary(double &data){//, std::string name
    xprintf(Msg, "CheckpointingOutputBin::load_double_binary(double &data) - netemplejtovaná metoda - Až to tady bude, tak to uložím :-)\n");
    in_stream.read((char *)&data, sizeof data);
};
