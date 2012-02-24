/*
 * CheckpointingBin.h
 *
 *  Created on: 18.1.2012
 *      Author: wojta
 */

#ifndef CHECKPOINTINGOUTPUTBIN_H_
#define CHECKPOINTINGOUTPUTBIN_H_

#include "CheckpointingOutput.h"

class CheckpointingOutputBin: public CheckpointingOutput {
public:
    CheckpointingOutputBin(string fileName);
    virtual ~CheckpointingOutputBin();

    /**
     * \brief General method for storing petsc Vector
     * \param[in] Vec - petsc Vector to be stored
     * \param[in] vec_name - name of the stored vector     */
    void save_data(Vec vec, std::string vec_name);

    /**
     * \brief General method for loading petsc Vector
     * \param[out] Vec - petsc Vector to be loaded
     * \param[in] vec_name - name of the loaded vector     */
    void load_data(Vec vec, std::string vec_name);

    /**
     * \brief General method for storing petsc Matrix
     * \param Mat - petsc Matrix to be stored
     * \param mat_name - name of the stored matrix     */
    void save_data(Mat mat, std::string mat_name);

    /**
     * \brief General method for loading petsc Matrix
     * \param Mat - petsc Matrix to be stored
     * \param mat_name - name of the stored matrix     */
    void load_data(Mat mat, std::string mat_name);

    /**
     * \brief General method for stroing TimeGovernor object
     * \param TimeGovernor - pointer to TimeGovernor instance */
    void save_data(TimeGovernor* tg);

    /**
     * \brief General method for loading/restoring TimeGovernor object
     * \param TimeGovernor - pointer to TimeGovernor instance */
    void load_data(TimeGovernor* tg);


//    void save_data(TimeMark* time_mark);
//    void load_data(TimeMark* time_mark);
    void save_data(TimeMarks* time_marks);
    void load_data(TimeMarks* time_marks);
    void save_data(double &data);
    void load_data(double &data);
    void save_data(int &data);
    void load_data(int &data);

//    template <typename T>
//       void save_data(T &data);//, std::string name

private:
    void save_vec_binary(Vec vec, std::string vec_name);
    void load_vec_binary(Vec vec, std::string vec_name);
    void save_mat_binary(Mat mat, std::string mat_name);
    void load_mat_binary(Mat mat, std::string mat_name);

    void save_timegovernor_binary(TimeGovernor* tg);
    void load_timegovernor_binary(TimeGovernor* tg);
    void save_timemarks_binary(TimeMarks& time_marks, ofstream& out_stream);
    void load_timemarks_binary(TimeMarks* time_marks);
    void save_timemark_binary(TimeMark* time_mark, ofstream& out_stream);
    void load_timemark_binary(TimeMark* time_mark, ifstream& in_stream);

    void save_double_binary(double& data);
    void load_double_binary(double& data);
};

#endif /* CHECKPOINTINGOUTPUTBIN_H_ */
