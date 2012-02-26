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
 * $Id: checkpointing_base.c 1458 2011-12-10 19:34:14Z vojtech.wrnata $
 * $Revision: 1458 $
 * $LastChangedBy: vojtech.wrnata $
 * $LastChangedDate: 2011-12-10 20:34:14 +0100 (So, 10 pro 2011) $
 *
 * @file    CheckpointingBase.c
 * @brief   Header: The functions for checkpointing_base class.
 *
 */

#include "CheckpointingBase.h"

CheckpointingBase::CheckpointingBase(TimeMarks &marks, Mesh &mesh, MaterialDatabase &mat_base, std::string class_name)
: EquationBase(marks, mesh, mat_base)
  {//
    xprintf(Msg,"CheckpointingBase constructor.\n");

    this->class_name_ = class_name;
    //    this->fullFileName = fullFileName(fileName);

    //default checkpointing is disabled
    checkpointing_on_ = OptGetBool("Checkpointing", "Checkpointing_on", "0");
    //default output file format is TXT - text file format
    out_file_format_ =  parse_output_format( OptGetStr("Checkpointing", "Output_format", "TXT") );

    //sets output_ via "Output_format" read from .ini
    output_ = set_output();

//    output_ = new CheckpointingOutputTxt("xxx");
    if (output_!=NULL){
        xprintf(Msg,"output_!=NULL.\n");
    }

    xprintf(Msg,"Checkpointing constructor - constructed.\n");
};

//CheckpointingBase::CheckpointingBase(const CheckpointingBase* ch){
//
//};

CheckpointingBase::~CheckpointingBase() {
    delete output_;
};

std::string CheckpointingBase::get_class_name(){
    return class_name_;
}

CheckpointingOutput* CheckpointingBase::get_output() const{
    return output_;
}

CheckpointingOutput* CheckpointingBase::set_output(){
    xprintf(Msg,"CheckpointingBase::set_output - %s.\n", class_name_.c_str());

    CheckpointingOutput* output;

    switch (out_file_format_) {
    case CH_OUTPUT_TXT: {
        xprintf(Msg,"Output TXT.\n");
        output = new CheckpointingOutputTxt(class_name_);
    }
    break;
    case CH_OUTPUT_BIN: {
        xprintf(Msg,"Output BIN.\n");
        output = new CheckpointingOutputBin(class_name_);
    }
    break;
//    case CH_OUTPUT_JSON: output = new CheckpointingOutputJSON(fileName);
//    break;
    default: output = new CheckpointingOutputTxt(class_name_);
    break;
    }

    return output;
};

bool CheckpointingBase::is_checkpointing_on(){
    return checkpointing_on_;
};


//TimeMark::Type CheckpointingBase::mark_type(){
//    return 1;
//};

void CheckpointingBase::save_state(){};

void CheckpointingBase::restore_state(){};

/*************************************************************************************************/
/**Supplementary methods *************************************************************************/
CheckpointingOutFileFormat CheckpointingBase::parse_output_format(char* format_name)
{
    if(strcmp(format_name,"TXT") == 0) {
        return CH_OUTPUT_TXT;
    } else if(strcmp(format_name,"BIN") == 0) {
        return CH_OUTPUT_BIN;
    } else if(strcmp(format_name,"JSON") == 0) {
        return CH_OUTPUT_JSON;
    } else {
        xprintf(Warn,"Unknown output file format: %s.\n", format_name );
        return (CH_OUTPUT_TXT);
    }
}

