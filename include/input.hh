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
 * $Id$
 * $Revision$
 * $LastChangedBy$
 * $LastChangedDate$
 *
 * @file
 * @brief   Headers: Inputs from structured file format
 * @author  Jiri Jenicek
 *
 */

#ifndef INPUT_HH_
#define INPUT_HH_

#include <map>
#include <string>
#include <vector>
using namespace std;

enum Value_type { type_string, type_number, type_object, type_vector, type_bool, type_null };

//generic node
class Data_node {
    map< string, Data_node > object_;
protected:
    Value_type               value_type_;
public:
    Data_node() { value_type_ = type_object; }

    bool get_string( string & s ) { s.clear(); return false; }
    Value_type get_type() { return value_type_; }
};

//node with array
class Vector_node : Data_node {
    vector< Data_node >  value_array_;
public:
    Vector_node () { value_type_ = type_vector; }
};

//node with value(s)
class Value_node : Data_node {
    string              value_string_;
    double              value_number_;
    bool                value_bool_;
public:
    Value_node() { value_type_ = type_null; }

    bool   get_bool();
    string get_string();
    bool   get_string( string & s );
    double get_double();
    float  get_float();
    int    get_int();

};

/*
 *  Implementation
 */

inline bool Value_node::get_bool()
{
    //TODO: stub
    return false;
}



inline string Value_node::get_string()
{
    //TODO: stub
    return "";
}



inline bool Value_node::get_string(string & s)
{
    //TODO: stub
    s.clear();
    return false;
}



inline double Value_node::get_double()
{
    //TODO: stub
    return 0.0;
}



inline float Value_node::get_float()
{
    //TODO: stub
    return 0.0f;
}



inline int Value_node::get_int()
{
    //TODO: stub
    return 0;
}



#endif /* INPUT_HH_ */
