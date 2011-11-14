#include <ostream>
#include <string>
#include "data_tree.hpp"
#include "Generic_node.hpp"
#include "Record_node.hpp"
#include "Vector_node.hpp"
#include "Value_node.hpp"
#include "json_spirit/json_spirit.h"

namespace flow {

Data_tree::Data_tree( const std::string& s )
{
    err = false;

    //load JSON from string
    if ( !json_spirit::read(s,json_root) )
    {
        err = true;
        return;
    }

    //build reference tree
    if ( !tree_build( json_root, node_head ) )
    {
        err = true;
        return;
    }
}

Data_tree::Data_tree( std::istream& is )
{
    err = false;

    //load JSON from string
    if ( !json_spirit::read(is,json_root) )
    {
        err = true;
        return;
    }

    //build reference tree
    if ( !tree_build( json_root, node_head ) )
    {
        err = true;
        return;
    }
}

Generic_node * Data_tree::new_node( const json_spirit::mValue json_node )
{
    Generic_node * gnp = NULL;

    switch (json_node.type()) {
    case json_spirit::obj_type: {
        //cout << "TYPE: record" << endl;
        gnp = new Record_node();
    }
        break;
    case json_spirit::array_type: {
        //cout << "TYPE: array" << endl;
        gnp = new Vector_node();
    }
        break;
    case json_spirit::str_type: {
        //cout << "TYPE: string: " << json_node.get_str() << endl;
        string s = json_node.get_str();
        gnp = new Value_node(s);
    }
        break;
    case json_spirit::bool_type: {
        //cout << "TYPE: bool: " << json_node.get_bool() << endl;
        gnp = new Value_node(json_node.get_bool());
    }
        break;
    case json_spirit::int_type: {
        //cout << "TYPE: int: " << json_node.get_int() << endl;
        gnp = new Value_node(json_node.get_int());
    }
        break;
    case json_spirit::real_type: {
        //cout << "TYPE: real: " << json_node.get_real() << endl;
        gnp = new Value_node(json_node.get_real());
    }
        break;
    case json_spirit::null_type: {
        //cout << "TYPE: null" << endl;
        gnp = new Value_node();
    }
        break;
    default: {
        cout << "TYPE: FUUU - neznamy typ json nodu" << endl;
    }
        break;
    }

    return gnp;
}

bool Data_tree::tree_build_recurse( json_spirit::mValue json_root, Generic_node & node )
{
    switch ( json_root.type() ) {
    case json_spirit::obj_type:
        {
            json_spirit::mObject::iterator it;
            Record_node & o_node = node.as_record();

            for( it = json_root.get_obj().begin(); it != json_root.get_obj().end(); ++it )
            {
                Generic_node * gnp = new_node(it->second);
                o_node.insert_key( it->first, *gnp );

                switch (it->second.type()) {
                case json_spirit::obj_type: //object and array need recursive build
                {
                    //cout << "KEY: " << it->first << " ";
                    //cout << "Record: going deep..." << endl;
                    if (!tree_build_recurse(it->second, *gnp)) {
                        cout << "recursive tree build failed at node: " << it->first << endl;
                        return false;
                    }
                    //cout << "Record: going up..." << endl;
                }
                break;
                case json_spirit::array_type: //record and array need recursive build
                {
                    //cout << "KEY: " << it->first << " ";
                    //cout << "Array: going deep..." << endl;
                    if (!tree_build_recurse(it->second, *gnp)) {
                        cout << "recursive tree build failed at node: " << it->first << endl;
                        return false;
                    }
                    //cout << "Array: going up..." << endl;
                }
                break;
                default:
                    //cout << "KEY: " << it->first << " ";
                break;
                }
            }
        }
        break;
    case json_spirit::array_type:
        {
            Vector_node & v_node = node.as_vector();
            for( unsigned int i = 0; i < json_root.get_array().size(); ++i )
            {
                Generic_node * gnp = new_node(json_root.get_array().at(i));
                v_node.insert_item( i, *gnp );

                switch (json_root.get_array().at(i).type()) {
                case json_spirit::obj_type: //record and array need recursive build
                {
                    //cout << "ID: " << i << " ";
                    //cout << "Record: going deep..." << endl;
                    if (!tree_build_recurse(json_root.get_array().at(i), *gnp)) {
                        cout << "recursive tree build failed at ID: " << i << endl;
                        return false;
                    }
                    //cout << "Record: going up..." << endl;
                }
                break;
                case json_spirit::array_type: //record and array need recursive build
                {
                    //cout << "ID: " << i << " ";
                    //cout << "Array: going deep..." << endl;
                    if (!tree_build_recurse(json_root.get_array().at(i), *gnp)) {
                        cout << "recursive tree build failed at ID: " << i << endl;
                        return false;
                    }
                    //cout << "Array: going up..." << endl;
                }
                break;
                default:
                    //cout << "ID: " << i << " ";
                break;
                }
            }
        }
        break;
    case json_spirit::str_type:
        break;
    case json_spirit::bool_type:
        break;
    case json_spirit::int_type:
        break;
    case json_spirit::real_type:
        break;
    case json_spirit::null_type:
        break;
    default:
        cout << "WTF? unknown json_spirit node type..." << endl;
        return false;
        break;
    }

    return true;
}

bool Data_tree::tree_build( const json_spirit::mValue json_root, Generic_node & head_node )
{
    //JSON not read ok
    if ( err )
        return false;

    //on highest level JSON can contain: 1 record or 1 array, nothing else
    // [...,...,...]
    // {...}

    switch ( json_root.type() ) {
    case json_spirit::obj_type:
        {
            cout << "Nacten root=Record, OK." << endl;
            return tree_build_recurse( json_root, head_node );
        }
        break;
    case json_spirit::array_type:
        {
            cout << "Nacten root=Array - pro Flow to nemuze nastat (tzn. CHYBA)." << endl;
            return false;
        }
        break;
    default:
        cout << "WTF? JSON root neni ani obj_type, ani array_type..." << endl;
        return false;
        break;
    }

    return true;
}

ostream & operator<<(ostream & stream, Data_tree & tree )
{
    stream << json_spirit::write(tree.json_root);
    return stream;
}

}
