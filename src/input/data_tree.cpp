#include <ostream>
#include <string>
#include "data_tree.hpp"
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

bool Data_tree::tree_build( const json_spirit::mValue, Generic_node & head_node )
{
    //JSON not read ok
    if ( err )
        return false;

    //on highest level JSON can contain: 1 object or 1 array, nothing else
    // [...,...,...]
    // {...}

    switch ( json_root.type() ) {
    case json_spirit::obj_type:
        {
            cout << "Nacten root=Object, OK." << endl;
            json_spirit::mObject::iterator it;

            for( it = json_root.get_obj().begin(); it != json_root.get_obj().end(); ++it )
            {
                //cout << it->first << " => " << it->second << endl;
                cout << "KEY: " << it->first << " ";
                cout << "TYPE: " << it->second.type() << " ";
                //cout << "VAL: " << it->second.get_str();
                cout << endl;
            }
        }
        break;
    case json_spirit::array_type:
        {
            cout << "Nacten root=Array - zatim ignorovano." << endl;
            return false;
        }
        break;
    default:
        cout << "wtf?" << endl;
        return false;
        break;
    }

    return true;
}

ostream & operator<<(ostream & stream, Data_tree & tree )
{
    stream << "HU" << json_spirit::write(tree.json_root);
    return stream;
}

}
