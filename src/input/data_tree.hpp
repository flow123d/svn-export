#ifndef DATA_TREE_HPP_
#define DATA_TREE_HPP_

#include <iostream>
#include <string>

#include "Generic_node.hpp"
#include "Record_node.hpp"
#include "Vector_node.hpp"
#include "Value_node.hpp"

#include "json_spirit/json_spirit.h"


namespace flow {

class Data_tree {
protected:
    //use json_spirit mValue (using map) and not Value (vector)
    //vector is exponentially slower for large data
    json_spirit::mValue json_root;
    Record_node node_head;

private:
    Data_tree():err(false) {}; //no default constructor
    Generic_node * new_node( const json_spirit::mValue json_node );
    bool tree_build( const json_spirit::mValue json_root, Generic_node & head_node );
    bool tree_build_recurse( json_spirit::mValue json_root, Generic_node & node );

public:
    bool err;

    Data_tree( const std::string& s ); //< build tree from JSON in string
    Data_tree( std::istream& is );     //< build tree from JSON in stream

    //dump loaded JSON
    void tree_dump_json( void ) { if ( !err ) { cout << json_spirit::write(json_root); } }
    friend ostream & operator<<( ostream & stream, Data_tree & tree );

    Generic_node & get_head() { return node_head; }

};


}

#endif /* DATA_TREE_HPP_ */
