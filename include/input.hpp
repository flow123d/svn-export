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

#ifndef INPUT_HPP_
#define INPUT_HPP_

#include <map>
#include <string>
#include <vector>
#include <iostream>
using namespace std;

enum Value_type { type_generic, type_string, type_number, type_object, type_vector, type_bool, type_null };

/*!
 * @brief Generic node - represents any JSON construct
 */
class Generic_node {
protected:
    Value_type               value_type_;
public:
    Generic_node() { value_type_ = type_generic; }
    Generic_node( Generic_node const & n ); //copy constructor - deep?

    //ziska typ nodu. To by mel asi umet kazdy
    Value_type get_type() { return value_type_; }
    // hierarchicky projde cely podstrom do hloubky a vypise ho?
    // asi taky uzitecne pro vsechny...
    ostream& operator<<( ostream& output ) { return output; }

    Generic_node & operator=( Generic_node const & n );


    // data_typ = strom["klic"].get_data<typ>() bez defaultu, pri nepritomnosti spadne
    // data_typ = strom["klic"].get_data<typ>( typ & default ) probehne vzdy
    template< typename T > T get_data();
    template< typename T > T get_data( T & default_data );
};

/*!
 * @brief Object node - represents JSON construct "{}"
 *        with pairs of "string" : any_node
 */
class Object_node: Generic_node {
    map< string, Generic_node > object_;
public:
    Object_node() { value_type_ = type_object; }

    //pro pristup stylem    gen_node_ref = muj_strom["klic"]
    // nebo data_typ = strom["klic"].get_data<typ>() bez defaultu, pri nepritomnosti spadne
    //      data_typ = strom["klic"].get_data<typ>( typ & default ) probehne vzdy
    Generic_node & operator[]( const string & key );
};

/*!
 * @brief Array node - represents JSON construct "[]",
 *        that is heterogeneous vector of any JSON construct
 */
class Vector_node : Generic_node {
    vector< Generic_node >  value_array_;
public:
    Vector_node () { value_type_ = type_vector; }

    bool get_vector( vector<Generic_node> & ret_vector );
    // get_vector pres template, aby se snazil dodat vse v konkretnim typu ??
};

/*!
 * @brief Scalar node - represents JSON construct "value",
 *        that can be "number, bool, string, null"
 */
class Value_node : Generic_node {
    string              value_string_;
    double              value_number_;
    bool                value_bool_;
public:
    Value_node() { value_type_ = type_null; }

    bool get_bool( bool & ret_value );
    bool get_bool( bool & ret_value, bool & default_value );
    bool get_string( string & ret_value );
    bool get_string( string & ret_value, string & default_value );
    bool get_double( double & ret_value );
    bool get_float( double & ret_value, double & default_value );
    bool get_int( int & ret_value );
    bool get_int( int & ret_value, int & default_value );
};


#endif /* INPUT_HPP_ */
