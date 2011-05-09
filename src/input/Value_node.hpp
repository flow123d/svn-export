#ifndef VALUE_NODE_H_
#define VALUE_NODE_H_

#include "Generic_node.h"

namespace flow {

/*!
 * @brief Scalar node - represents JSON construct "value",
 *        that can be "number, bool, string, null"
 */
class Value_node : public Generic_node {
    string              value_string_;
    double              value_number_;
    bool                value_bool_;
public:
    Value_node() {
        value_type_   = type_null;
        value_number_ = 0.0;
        value_bool_   = false;
    }

    virtual Generic_node & get_item( const int id ) {
        //pristup jako do vektoru, ale jsme ve skalaru => vzdy vrati prazdnou instanci
        return *empty_node_generic_;
    }
    virtual Generic_node & get_key( const string & key ) {
        //pristup jako do objektu, ale jsme ve skalaru => vzdy vrati prazdnou instanci
        return *empty_node_generic_;
    }
    virtual Generic_node & get_item( const size_t id, Generic_node & default_tree ) {
        //pristup jako do vektoru, ale jsme ve skalaru => vzdy vrati prazdnou instanci
        return *empty_node_generic_;
    }
    virtual Generic_node & get_key( const string & key, Generic_node & default_tree ) {
        //pristup jako do objektu, ale jsme ve skalaru => vzdy vrati prazdnou instanci
        return *empty_node_generic_;
    }

    virtual Value_node & as_value( void ) { return (*this); }
    friend ostream & operator<<( ostream & stream, const Value_node & node );

    //TODO: pokazde kontrolovat, zda je to skutecne Value_node, a ne jen pretypovany jiny?

    //pro ziskani uz finalnich skalarnich hodnot, s defaultni hodnotou a bez
    //bez def: pri nemoznosti konverze vrati false
    //  s def: uspeje vzdy, protoze pri nemoznosti konverze pouzije default
    //  s err: uspeje vzdy, pri neuspechu vyplni err, jako hodnotu vrati 0, null, nebo ekvivalent
    bool get_bool( bool & ret_value );
    bool get_bool( bool & ret_value, bool & default_value );
    bool get_string( string & ret_value );
    bool get_string( string & ret_value, string & default_value );
    bool get_double( double & ret_value );
    bool get_float( double & ret_value, double & default_value );
    bool get_int( int & ret_value );
    bool get_int( int & ret_value, const int & default_value );

    virtual ~Value_node();
};

}

#endif /* VALUE_NODE_H_ */
