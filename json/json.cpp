//          Copyright John W. Wilkinson 2007 - 2009.
// Distributed under the MIT License, see accompanying file LICENSE.txt

// json spirit version 4.03

// This demo shows you how to read and write JSON objects and arrays.
// In this demo objects are stored as a map of names to values.

#include "json_spirit/json_spirit.h"
#include <cassert>
#include <fstream>

using namespace std;
//using namespace json_spirit;

const json_spirit::mValue& find_value( const json_spirit::mObject& obj, const string& name  )
{
    json_spirit::mObject::const_iterator i = obj.find( name );

    assert( i != obj.end() );
    assert( i->first == name );

    return i->second;
}

int main()
{
    //variables
    ifstream in_s("flow_schur2.json");
    json_spirit::mValue tree_root;

    //read
    json_spirit::read(in_s, tree_root);
    in_s.close();

    //print all
    cout << json_spirit::write(tree_root) << endl << endl;

    //root object, or array??
    switch ( tree_root.type() ) {
    case json_spirit::obj_type:
        cout << "nalezen Object" << endl;
        break;
    case json_spirit::array_type:
        cout << "nalezeno Array" << endl;
        break;
    default:
        cout << "wtf?" << endl;
    }

    //print just some
    //get root object - flow ini ma jako root Object, muzem si to dovolit bez testovani
    const json_spirit::mObject &root = tree_root.get_obj();

    //with helper function
    //direct type request
    cout << "verze: \"" << find_value( root, "flow_ini_version" ).get_str() << "\"" << endl;
    //template request
    cout << "verze: \"" << find_value( root, "flow_ini_version" ).get_value<string>() << "\"" << endl;

    //with built-in
    cout << "komentar: \"" << root.find("comment")->second.get_str() << "\"" << endl;
    cout << "komentar: \"" << root.find("comment")->second.get_value<string>() << "\"" << endl;

    //deeper in hierarchy
    cout << "ini-global-description: \"" << root.find("global")->second.get_obj().find("description")->second.get_str() << "\"" << endl;
    cout << "ini-global-description: \"" << root.find("global")->second.get_obj().find("description")->second.get_value<string>() << "\"" << endl;

    return 0;
}
;
