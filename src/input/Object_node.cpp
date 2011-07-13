#include "Object_node.hpp"

namespace flow {

Generic_node & Object_node::get_key(const string & key) {
    map<string, Generic_node &>::iterator it;

    it = object_.find(key);
    if (it == object_.end()) {
        //nenasli, vratime prazdnou
        return *empty_node_generic_;
    } else {
        return it->second;
    }
}

Generic_node & Object_node::get_key(const string & key, Generic_node & default_tree) {
    map<string, Generic_node &>::iterator it;

    it = object_.find(key);
    if (it == object_.end()) {
        //nenasli, vratime default
        return default_tree;
    } else {
        return it->second;
    }
}

ostream & operator <<(ostream & stream, const Object_node & node) {
    stream << "Node begin: Object: ";
    stream << "type " << node.get_type() << ", ";
    stream << "\"" << node.get_type_str() << "\". ";
    stream << "Node end.";
    return stream;
}

Generic_node & Object_node::get_key_check(const string & key, int & err_code) {
    map<string, Generic_node &>::iterator it;

    it = object_.find(key);
    if (it == object_.end()) {
        //nenasli, vratime default
        err_code = 1;
        return *empty_node_generic_;
    } else {
        err_code = 0;
        return it->second;
    }
}

Object_node::~Object_node() {
    //deep destructor?
    // TODO Auto-generated destructor stub
}

}
