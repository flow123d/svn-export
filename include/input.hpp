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

class Object_node;
class Vector_node;
class Value_node;

/*!
 * @brief Generic node - represents any JSON construct
 */
class Generic_node {
protected:
    Value_type           value_type_;
    static Generic_node  empty_node_generic_;  //prazdna instance
    static Object_node   empty_node_object_;  //prazdna instance
    static Vector_node   empty_node_vector_;  //prazdna instance
    static Value_node    empty_node_value_;  //prazdna instance
public:
    Generic_node() { value_type_ = type_generic; }

    Generic_node( Generic_node const & n ); //copy constructor - deep?

    Value_type get_type() { return value_type_; } //ziska typ nodu. To by mel asi umet kazdy

    virtual Generic_node & get_key( const string & key );
    virtual Generic_node & get_item( const int id );

    /* Implementace as_* tady nefunguje, je potreba znat presnou deklaraci tridy.
     * Forward deklarace nestaci (nevi, jake ma k dispozici metody apod.)
     */
    virtual Object_node & as_object( void );
    virtual Vector_node & as_vector( void );
    virtual Value_node & as_value( void );

    //Generic_node & operator=( Generic_node const & n );
    // data_typ = strom["klic"].get_data<typ>() bez defaultu, pri nepritomnosti spadne
    // data_typ = strom["klic"].get_data<typ>( typ & default ) probehne vzdy
    //template< typename T > T get_data();
    //template< typename T > T get_data( T & default_data );
    virtual ~Generic_node();
};

/*!
 * @brief Object node - represents JSON construct "{}"
 *        with pairs of "string" : any_node
 */
class Object_node: Generic_node {
    map< string, Generic_node & > object_;
public:
    Object_node() { value_type_ = type_object; }

    virtual Generic_node & get_item( const int id ) {
        //pristup jako do vektoru, ale jsme v objektu => vzdy vrati prazdnou instanci
        return empty_node_generic_;
    }

    virtual Generic_node & get_key( const string & key ) {
        map< string, Generic_node & >::iterator it;

        it = object_.find( key );
        if ( it == object_.end() )
        {
            //nenasli, vratime prazndou
            return empty_node_generic_;
        } else {
            return it->second;
        }
    }

    virtual Object_node & as_object( void ) { return (*this); }

    //pro pristup stylem    gen_node_ref = muj_strom["klic"]
    // nebo data_typ = strom["klic"].get_data<typ>() bez defaultu, pri nepritomnosti spadne
    //      data_typ = strom["klic"].get_data<typ>( typ & default ) probehne vzdy
    //Generic_node & operator[]( const string & key );
    virtual ~Object_node();
};

/*!
 * @brief Array node - represents JSON construct "[]",
 *        that is heterogeneous vector of any JSON construct
 */
class Vector_node : Generic_node {
    //vektor referenci nelze!
    vector< Generic_node * >  value_array_;
public:
    Vector_node () { value_type_ = type_vector; }

    virtual Generic_node & get_item( const int id ) {
        if (  id >= value_array_.size() )
        {
            //mimo rozsah pole, vratime prazndou
            return empty_node_generic_;
        } else {
            return *value_array_[id];
        }
    }

    virtual Generic_node & get_key( const string & key ) {
        //pristup jako do objektu, ale jsme ve vektoru => vzdy vrati prazdnou instanci
        return empty_node_generic_;
    }

    virtual Vector_node & as_vector( void ) { return (*this); }

    //Generic_node & operator[]( const int & id );
    // get_vector pres template, aby se snazil dodat vse v konkretnim typu ??
    //bool get_vector( vector<Generic_node> & ret_vector );
    virtual ~Vector_node();
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

    virtual Generic_node & get_item( const int id ) {
        //pristup jako do vektoru, ale jsme ve skalaru => vzdy vrati prazdnou instanci
        return empty_node_generic_;
    }
    virtual Generic_node & get_key( const string & key ) {
        //pristup jako do objektu, ale jsme ve skalaru => vzdy vrati prazdnou instanci
        return empty_node_generic_;
    }

    virtual Value_node & as_value( void ) { return (*this); }


    //pokazde kontrolovat, zda je to skutecne Value_node, a ne jen pretypovany jiny?

    //pro ziskani uz finalnich skalarnich hodnot, s defaultni hodnotou a bez
    //bez def: pri nemoznosti konverze vrati false
    //  s def: uspeje vzdy, protoze pri nemoznosti konverze pouzije default
    bool get_bool( bool & ret_value );
    bool get_bool( bool & ret_value, bool & default_value );
    bool get_string( string & ret_value );
    bool get_string( string & ret_value, string & default_value );
    bool get_double( double & ret_value );
    bool get_float( double & ret_value, double & default_value );
    bool get_int( int & ret_value );
    bool get_int( int & ret_value, int & default_value );

    virtual ~Value_node();
};

inline Object_node & Generic_node::as_object(void)
{
   if ( value_type_ == type_object ) {
       return * dynamic_cast < Object_node * > (this) ;
   } else {
       //zkousime pristoupit jako k objektu, ale neni objekt - vrat empty
       return empty_node_object_;
   }
}

inline Vector_node & Generic_node::as_vector(void)
{
    if ( value_type_ == type_vector ) {
        return * dynamic_cast < Vector_node * > (this);
    } else {
        //zkousime pristoupit jako k vektoru, ale neni vektor - vrat empty
        return empty_node_vector_;
    }
}

inline Value_node & Generic_node::as_value(void)
{
    if ( ( value_type_ == type_string ) || ( value_type_ == type_number ) ||
            ( value_type_ == type_bool ) || ( value_type_ == type_null ) ) {
        return * dynamic_cast < Value_node * > (this);
    } else {
        //zkousime pristoupit jako k value, ale neni value - vrat empty
        return empty_node_value_;
    }
}

#endif /* INPUT_HPP_ */
