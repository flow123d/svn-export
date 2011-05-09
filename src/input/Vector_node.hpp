#ifndef VECTOR_NODE_H_
#define VECTOR_NODE_H_

#include "Generic_node.h"

namespace flow {

/*!
 * @brief Array node - represents JSON construct "[]",
 *        that is heterogeneous vector of any JSON construct
 */
class Vector_node : public Generic_node {
    //vektor referenci nelze!
    vector< Generic_node * >  value_array_;
public:
    Vector_node () { value_type_ = type_vector; }

    virtual Generic_node & get_item( const size_t id );
    virtual Generic_node & get_item( const size_t id, Generic_node & default_tree );
    virtual Generic_node & get_key( const string & key ) {
        //pristup jako do objektu, ale jsme ve vektoru => vzdy vrati prazdnou instanci
        return *empty_node_generic_;
    }
    virtual Generic_node & get_key( const string & key, Generic_node & default_tree ) {
        //pristup jako do objektu, ale jsme ve vektoru => vzdy vrati default
        return default_tree;
    }

    virtual Vector_node & as_vector( void ) { return (*this); }
    friend ostream & operator<<( ostream & stream, const Vector_node & node );

    //Generic_node & operator[]( const int & id );
    // get_vector pres template, aby se snazil dodat vse v konkretnim typu ??
    //bool get_vector( vector<Generic_node> & ret_vector );
    virtual ~Vector_node();
};

}

#endif /* VECTOR_NODE_H_ */
