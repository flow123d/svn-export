#ifndef VALUE_NODE_HPP_
#define VALUE_NODE_HPP_

#include "Generic_node.hpp"

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
        value_string_ = "";
    }

    //TODO: Value node constructory s hodnotou


    virtual Generic_node & get_item( const int id ) {
        //pristup jako do vektoru, ale jsme ve skalaru => vzdy vrati prazdnou instanci
        return *empty_node_generic_;
    }
    virtual Generic_node & get_key( const string & key ) {
        //pristup jako do objektu, ale jsme ve skalaru => vzdy vrati prazdnou instanci
        return *empty_node_generic_;
    }
    virtual Generic_node & get_key_check( const string & key, int & err_code ) {
        //pristup jako do objektu, ale jsme ve skalaru => vzdy vrati prazdnou instanci
        //s chybou
        err_code = 0;
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
    virtual Generic_node & get_item_check( const size_t id, int & err_code ) {
        //pristup jako do vektoru, ale jsme ve skalaru => vzdy vrati prazdnou instanci
        //s chybou
        err_code = 0;
        return *empty_node_generic_;
    }

    virtual Value_node & as_value( void ) { return (*this); }
    friend ostream & operator<<( ostream & stream, const Value_node & node );

    //TODO: pokazde kontrolovat, zda je to skutecne Value_node, a ne jen pretypovany jiny?
    virtual bool get_bool( void );
    virtual bool get_bool( const bool & default_value );
    virtual bool get_bool_check( int & err_code );

    virtual int get_int( void );
    virtual int get_int( const int & default_value );
    virtual int get_int_check( int & err_code );

    virtual float get_float( void );
    virtual float get_float( const float & default_value );
    virtual float get_float_check( int & err_code );

    virtual double get_double( void );
    virtual double get_double( const double & default_value );
    virtual double get_double_check( int & err_code );

    virtual string get_string( void );
    virtual string get_string( const string & default_value );
    virtual string get_string_check( int & err_code );

    virtual ~Value_node();
};

}

#endif /* VALUE_NODE_HPP_ */
