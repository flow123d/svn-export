#ifndef OBJECT_NODE_HPP_
#define OBJECT_NODE_HPP_

#include "Generic_node.hpp"

namespace flow {


/*!
 * @brief Object node - represents JSON construct "{}"
 *        with pairs of "string" : any_node
 */
class Object_node: public Generic_node {
    map< string, Generic_node & > object_;
public:
    Object_node() { value_type_ = type_object; }

    virtual Generic_node & get_item( const int id ) {
        //pristup jako do vektoru, ale jsme v objektu => vzdy vrati prazdnou instanci
        return *empty_node_generic_;
    }
    virtual Generic_node & get_item( const size_t id, Generic_node & default_tree ) {
        //pristup jako do vektoru, ale jsme v objektu => vzdy vrati default
        return default_tree;
    }
    virtual Generic_node & get_item_check( const size_t id, int & err_code ) {
        //pristup jako do vektoru, ale jsme v objektu => vzdy vrati prazdnou instanci
        //vyplni chybu
        err_code = 1;
        return *empty_node_generic_;
    }

    void insert_key( const string & key, Generic_node & node );

    virtual Generic_node & get_key( const string & key );
    virtual Generic_node & get_key( const string & key, Generic_node & default_tree );
    virtual Generic_node & get_key_check( const string & key, int & err_code );

    virtual Object_node & as_object( void ) { return (*this); }

    friend ostream & operator<<( ostream & stream, const Object_node & node );

    virtual ~Object_node();
};

}

#endif /* OBJECT_NODE_HPP_ */
