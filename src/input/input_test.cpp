#include <iostream>
#include <fstream>
#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <cfloat>

#include "json_spirit/json_spirit.h"
#include "data_tree.hpp"

using namespace std;
using namespace flow;
//using namespace json_spirit;

int main()
{
    cout << "===== JSON direct =====" << endl;

    //read from stream
    json_spirit::mValue tree_root;
    ifstream in_s("flow_mini.json");
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
        break;
    }

    //print just some
    //get root object - flow ini has as root Object, no need to test
    //(the other root type can be array)
    json_spirit::mObject &root = tree_root.get_obj();

    //with iterator helper
    json_spirit::mObject::iterator i;
    i = root.find("flow_ini_version");
    cout << "verze: \"" << i->second.get_str() << "\"" << endl;

    //template request
    cout << "verze: \"" << i->second.get_value<string>() << "\"" << endl;

    //with built-in
    cout << "komentar: \"" << root.find("comment")->second.get_str() << "\"" << endl;
    cout << "komentar: \"" << root.find("comment")->second.get_value<string>() << "\"" << endl;

    //deeper in hierarchy
    cout << "ini-global-description: \"" << root.find("global")->second.get_obj().find("description")->second.get_str() << "\"" << endl;
    cout << "ini-global-description: \"" << root.find("global")->second.get_obj().find("description")->second.get_value<string>() << "\"" << endl;

    //test Node library
    cout << endl << "===== NODE LIBRARY =====" << endl;
    {
        Generic_node gnode,gnode2,gnode3;
        Value_node vnode;
        Generic_node & gnode_r = vnode;
        Generic_node * gnode_p;
        Data_tree * tree;
        int default_int = 12321;
        int returned_int;

        gnode_p = new Value_node;

        //read from file
        ifstream in_s("flow_mini.json");
        tree = new Data_tree(in_s);
        in_s.close();

        cout << "Tree error: " << tree->err << endl;

        cout << "Dump: ";
        tree->tree_dump_json();
        cout << endl;

        cout << "Operator << ->" << (*tree) << "<-" << endl;

        Generic_node & nodes = tree->get_head();

        cout << "Vypis nodu pres <<:" << endl << nodes << "KONEC" << endl;

        //non-existent node example
        returned_int = gnode.get_key("foo").get_item(10).as_value().get_int(default_int);
        cout << "default_int=" << default_int << endl;
        cout << "returned_int=" << returned_int << endl;

        //access to instance as an ancestor
        std::cout << "gnode " << gnode << endl;
        std::cout << "vnode " << vnode << endl;
        std::cout << "gnode reference to vnode " << gnode_r << endl;
        std::cout << "gnode pointer to vnode " << (*gnode_p) << endl;

    }

    printf( "END\n" );
    return 0;
}
