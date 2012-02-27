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

    /**sets output precision for double to 10 digits*/
    out_stream.precision(std::numeric_limits<double>::digits10);


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
void CheckpointingOutputTxt::save_data(Vec vec, std::string vec_name){
    save_vec_txt(vec, vec_name);
};

/**
 * \brief General method for loading petsc Vector
 * \param[out] Vec - petsc Vector to be loaded
 * \param[in] vecName - name of the loaded vector     */
void CheckpointingOutputTxt::load_data(Vec vec, std::string vec_name){
    load_vec_txt(vec, vec_name);
};

/**
 * \brief General method for storing petsc Matrix
 * \param Mat - petsc Matrix to be stored
 * \param matName - name of the stored matrix     */
void CheckpointingOutputTxt::save_data(Mat mat, std::string mat_name){
    save_mat_txt(mat, mat_name);
};

/**
 * \brief General method for loading petsc Matrix
 * \param Mat - petsc Matrix to be stored
 * \param matName - name of the stored matrix     */
void CheckpointingOutputTxt::load_data(Mat mat, std::string mat_name){
    load_mat_txt(mat, mat_name);
};

/**
 * \brief General method for storing TimeGovernor object
 * \param TimeGovernor - pointer to TimeGovernor instance */
void CheckpointingOutputTxt::save_data(TimeGovernor* tg){
    xprintf(Msg, "CheckpointingOutputTxt::save_timegovernor");
    save_timegovernor_txt(tg);
};

/**
 * \brief General method for loading/restoring TimeGovernor object
 * \param TimeGovernor - pointer to TimeGovernor instance */
void CheckpointingOutputTxt::load_data(TimeGovernor* tg){
    load_timegovernor_txt(tg);
};


void CheckpointingOutputTxt::save_data(TimeMarks* time_marks){
    save_timemarks_txt(time_marks);
};
void CheckpointingOutputTxt::load_data(TimeMarks* time_marks){
    //    load_timemarks_txt(time_marks);
};
void CheckpointingOutputTxt::save_data(double &data){
    xprintf(Msg, "netemplejtovaná CheckpointingOutputTxt::save_data<double>(double &data, ..) -Až to tady bude, tak to uložím :-)\n");
    save_double_txt(data);
};
void CheckpointingOutputTxt::load_data(double &data){
    xprintf(Msg, "netemplejtovaná CheckpointingOutputTxt::save_data<double>(double &data, ..) -Až to tady bude, tak to uložím :-)\n");
    //    load_double_txt(data);
};
void CheckpointingOutputTxt::save_data(int &data){
    xprintf(Msg, "netemplejtovaná CheckpointingOutputTxt::save_data<int>(int &data, ..) -Až to tady bude, tak to uložím :-)\n");
    //    save_int_txt(data);
};
void CheckpointingOutputTxt::load_data(int &data){
    xprintf(Msg, "netemplejtovaná CheckpointingOutputTxt::save_data<int>(int &data, ..) -Až to tady bude, tak to uložím :-)\n");
    //    load_int_txt(data);
};


//template <typename T>
//    void CheckpointingOutputTxt::save_data(T &data){//, std::string name
//    xprintf(Msg, "CheckpointingOutputTxt::save_data(T &data, ..) - Až to tady bude, tak to uložím :-)\n");
//};


/******************************************************************************************/
/**Private methods for physical saving/loading ********************************************/
void CheckpointingOutputTxt::save_vec_txt(Vec vec, std::string vec_name){
    PetscInt size;
    PetscViewer    viewer;
    std::string full_name;

    full_name = util->full_file_name_vec_mat(vec_name);
//    xprintf( Msg, "xxx full_name:%s\n", full_name.c_str());
    /**ulozeni vectoru do ACSII (Txt) souboru*/
    PetscViewerASCIIOpen(PETSC_COMM_WORLD,full_name.c_str(),&viewer);
    VecView(vec,viewer);

    PetscViewerDestroy(&viewer);
};
void CheckpointingOutputTxt::load_vec_txt(Vec vec, std::string vec_name){
    PetscViewer    viewer;
    Vec            u;

    std::string full_name;

    full_name = util->full_file_name_vec_mat(vec_name);
    /**nacteni vectoru z ASCII (Txt) souboru*/
    PetscViewerASCIIOpen(PETSC_COMM_WORLD,full_name.c_str(),&viewer);
    //        VecCreate(PETSC_COMM_WORLD,&vec);
    VecLoad(vec,viewer);
    PetscViewerDestroy(&viewer);
};
void CheckpointingOutputTxt::save_mat_txt(Mat mat, std::string mat_name){
    PetscInt size;
    PetscViewer    viewer;
    std::string full_name;

    full_name = util->full_file_name_vec_mat(mat_name);
//    xprintf( Msg, "xxx full_name:%s\n", full_name.c_str());
    /**ulozeni Mat do ACSII (Txt) souboru */
    PetscViewerASCIIOpen(PETSC_COMM_WORLD,full_name.c_str(),&viewer);
    MatView(mat,viewer);

    PetscViewerDestroy(&viewer);
};
void CheckpointingOutputTxt::load_mat_txt(Mat mat, std::string mat_name){
    PetscViewer    viewer;
    Mat            m;

    std::string full_name;

    full_name = util->full_file_name_vec_mat(mat_name);
    /**nacteni matice z ASCII (Txt) souboru*/
    PetscViewerASCIIOpen(PETSC_COMM_WORLD,full_name.c_str(),&viewer);
    //        VecCreate(PETSC_COMM_WORLD,&vec);
    MatLoad(mat,viewer);
    PetscViewerDestroy(&viewer);
};
void CheckpointingOutputTxt::save_timegovernor_txt(TimeGovernor* tg){
    xprintf(Msg,"CheckpointingOutputTxt::save_timegovernor_txt.\n");
       ofstream tg_out_stream;
       tg_out_stream.open(util->full_file_name_vec_mat("TimeGovernor").c_str());
       INPUT_CHECK( tg_out_stream.is_open() , "Can not open output file: %s\n", util->full_file_name_vec_mat("TimeGovernor").c_str() );

       /**sets output precision for double to 10 digits*/
       tg_out_stream.precision(std::numeric_limits<double>::digits10);

       tg_out_stream << "time_level: " << tg->tlevel() << endl;
       tg_out_stream << "time: " << tg->t() << endl;
       tg_out_stream << "end_of_fixed_dt_interval: " << tg->end_of_fixed_dt() << endl;
       tg_out_stream << "end_time_: " << tg->end_time() << endl;
       tg_out_stream << "time_step: " << tg->dt() << endl;
       tg_out_stream << "last_time_step: " << tg->last_dt() << endl;
       tg_out_stream << "fixed_dt: " << tg->get_fixed_dt() << endl;
       tg_out_stream << "dt_changed: " << tg->is_changed_dt() << endl;
       tg_out_stream << "time_step_constrain: " << tg->get_time_step_constrain() << endl;
       tg_out_stream << "max_time_step: " << tg->get_max_time_step() << endl;
       tg_out_stream << "min_time_step: " << tg->get_min_time_step() << endl;


       if(tg_out_stream != NULL) {
           tg_out_stream.close();
           //        delete outStream;
       }
};
void CheckpointingOutputTxt::load_timegovernor_txt(TimeGovernor* tg){};
void CheckpointingOutputTxt::save_timemarks_txt(TimeMarks* time_marks){
    xprintf(Msg,"CheckpointingOutputTxt::save_timemarks_txt.\n");
    ofstream tm_out_stream;
    tm_out_stream.open(util->full_file_name("TimeMarks").c_str());
    INPUT_CHECK( tm_out_stream.is_open() , "Can not open output file: %s\n", util->full_file_name("TimeMarks").c_str() );

    /**sets output precision for double to 10 digits*/
    tm_out_stream.precision(std::numeric_limits<double>::digits10);

    tm_out_stream << "type_fixed_time: 0x" << hex << time_marks->type_fixed_time() << dec << endl;
    tm_out_stream << "type_output: 0x" << hex << time_marks->type_output() << endl;
    tm_out_stream << "type_bc_change: 0x" << hex << time_marks->type_bc_change() << endl;
    tm_out_stream << "type_checkpointing: 0x" << hex << time_marks->type_checkpointing() << endl;
    tm_out_stream << "type_next_mark_type: 0x" << hex << time_marks->type_next_mark_type() << endl;

    for(vector<TimeMark>::const_iterator it = time_marks->get_marks().begin(); it != time_marks->get_marks().end(); ++it){
        save_timemark_txt(*it, tm_out_stream);//, tm_out_stream
    }

    if(tm_out_stream != NULL) {
        tm_out_stream.close();
        //        delete outStream;
    }

//    xprintf(Msg,"CheckpointingOutputTxt::CheckpointingOutputTxt - constructed.\n");
};
void CheckpointingOutputTxt::load_timemarks_txt(TimeMarks* time_marks){};
void CheckpointingOutputTxt::save_timemark_txt(const TimeMark &time_mark, ofstream& out_stream){
//    out_stream << time_mark << endl;
    out_stream << scientific << time_mark.time();
    out_stream << ": 0x" << hex << time_mark.mark_type() << dec << endl;

};
void CheckpointingOutputTxt::load_timemark_txt(TimeMark* time_mark, ifstream& in_stream){};
void CheckpointingOutputTxt::save_double_txt(double& data){
    out_stream << scientific << data << endl;
};
void CheckpointingOutputTxt::load_double_txt(double& data){};



