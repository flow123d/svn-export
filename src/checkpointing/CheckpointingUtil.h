/*
 * CHeckpointingUtil.h
 *
 *  Created on: 24.1.2012
 *      Author: wojta
 */

#ifndef CHECKPOINTINGUTIL_H_
#define CHECKPOINTINGUTIL_H_

#include "system/system.hh"

/** \typedef Output file formats */
    typedef enum {
        CH_OUTPUT_TXT = 1,/*!< output file format is text */
        CH_OUTPUT_BIN = 2,/*!< output file format is binary */
        CH_OUTPUT_JSON = 3,/*!< output file format is JSON */
    } CheckpointingOutFileFormat;


class CheckpointingUtil {
public:
    CheckpointingUtil();
    CheckpointingUtil(std::string fileName);
    virtual ~CheckpointingUtil();



    ///File name for TimeGovernor to be stored
    static const std::string tg_file_name;

    /** \brief Returns file name with path, uses IONameHandler::get_instance()->get_output_file_name
     * \param  string name - name of the file*/
    std::string file_name_path(std::string name);

    /** \brief Returns file name with prefix and suffix
     * \param  string name - base name of the file*/
    std::string full_file_name(std::string name);

    /** \brief Returns file name with prefix and suffix
     * \param  string name - name of the file*/
    std::string full_file_name_vec_mat(std::string name);

    /** \brief Returns file name with prefix and suffix for TimeGovernor */
    std::string full_tg_file_name();

    /**\brief public setter for fileName */
    void set_file_name(std::string file_name);

private:
    /// Base name for file to strore values
    std::string file_name_;

    /// Prefix for files storing Vectors, Matrixes, TimeGovernor and other variables
    std::string file_prefix_;
    /// Suffix for files storing Vectors, Matrixes TimeGovernor and other variables
    std::string file_suffix_;

    std::string time_for_file_name();

};

#endif /* CHECKPOINTINGUTIL_H_ */
