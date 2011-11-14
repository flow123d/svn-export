#ifndef KEYCONNECTOR_HPP_
#define KEYCONNECTOR_HPP_

#include <string>
#include "Generic_node.hpp"

using namespace std;

namespace flow {

class Key_connector {
    bool declared_ ;
    Generic_node * default_node_;
    string description_;
public:
    Key_connector()
    {
        declared_ = false;
        default_node_ = NULL;
    }
    Key_connector( string & desc ) {
        declared_ = true;
        description_ = desc;
        default_node_ = NULL;
    }
    Key_connector( string & desc, Generic_node & default_node ) {
        declared_ = true;
        description_ = desc;
        default_node_ = &default_node;
    }

    bool is_declared() {
        return declared_;
    }

    string & get_description() {
        return description_;
    }

    Generic_node * get_default() {
        return default_node_;
    }

    void set_declared(bool declared) {
        declared_ = declared;
    }

    void set_description(string & description) {
        description_ = description;
    }
    virtual ~Key_connector();
};


} /* namespace flow */
#endif /* KEYCONNECTOR_HPP_ */
