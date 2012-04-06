/*
 * CHeckpointingUtil.cpp
 *
 *  Created on: 24.1.2012
 *      Author: wojta
 */

#include "CheckpointingUtil.h"

const std::string CheckpointingUtil::tg_file_name = "tg";

CheckpointingUtil::CheckpointingUtil() {
    //default prefix is empty
       file_prefix_ = OptGetStr("Checkpointing", "Output_file_prefix", "");
       //default suffix is .dat
       file_suffix_ = OptGetStr("Checkpointing", "Output_file_suffix", ".dat");

}
CheckpointingUtil::CheckpointingUtil(std::string file_name) {
    //default prefix is empty
       file_prefix_ = OptGetStr("Checkpointing", "Output_file_prefix", "");
       //default suffix is .dat
       file_suffix_ = OptGetStr("Checkpointing", "Output_file_suffix", ".dat");

       this->file_name_ = file_name;
}

CheckpointingUtil::~CheckpointingUtil() {
    // TODO Auto-generated destructor stub
}


void CheckpointingUtil::set_file_name(std::string file_name){
    this->file_name_ = file_name;
};

std::string CheckpointingUtil::time_for_file_name(){
    std::string nic = "";
    time_t cas;
    cas = time(0);
    struct tm *tajm = gmtime(&cas);
    char txt[100];
    strftime(txt, 100, "%Y%m%d%H%M%S", tajm);

    return nic.append(txt);
};


std::string CheckpointingUtil::file_name_path(std::string name){
    return IONameHandler::get_instance()->get_output_file_name(name.c_str());
};

std::string CheckpointingUtil::full_file_name(std::string name){
    std::string time_mark;
    std::string full_name;
    std::string full_name_path;

    time_mark = time_for_file_name();
    full_name = file_prefix_ + name + file_suffix_; // + "_" + timeMark
    full_name_path = file_name_path(full_name);

    return full_name_path;
}
std::string CheckpointingUtil::full_file_name_id(std::string name, std::string id){
    std::string time_mark;
    std::string full_name;
    std::string full_name_path;

    time_mark = time_for_file_name();
    full_name = file_prefix_ + name + "_" + id + file_suffix_; // + "_" + timeMark
    full_name_path = file_name_path(full_name);

    return full_name_path;
}
std::string CheckpointingUtil::full_file_name_vec_mat(std::string name){
    std::string time_mark;
    std::string full_name;
    std::string full_name_path;

    time_mark = time_for_file_name();
    full_name = file_prefix_ + this->file_name_ + "_" + name + file_suffix_; // + "_" + timeMark
    full_name_path = file_name_path(full_name);

    return full_name_path;
}

std::string CheckpointingUtil::full_tg_file_name(){
    return full_file_name(tg_file_name);
}
