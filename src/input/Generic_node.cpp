#include <string>
#include "Generic_node.h"
#include "Object_node.h"
#include "Vector_node.h"
#include "Value_node.h"

namespace flow {

//inicializace class dat
Object_node   * Generic_node::empty_node_object_  = new Object_node();  //prazdna instance
Vector_node   * Generic_node::empty_node_vector_  = new Vector_node();  //prazdna instance
Value_node    * Generic_node::empty_node_value_   = new Value_node();   //prazdna instance
Generic_node  * Generic_node::empty_node_generic_ = new Generic_node(); //prazdna instance

string & Generic_node::get_type_str( void ) const
{
    static string names[10];
    static bool init = false;

    if ( !init ) {
        names[type_generic] = "type_generic";
        names[type_string] = "type_string";
        names[type_number] = "type_number";
        names[type_object] = "type_object";
        names[type_vector] = "type_vector";
        names[type_bool] = "type_bool";
        names[type_null] = "type_null";
        init = true;
    }

    return names[this->value_type_];
}

Generic_node & Generic_node::get_item(const size_t id)
{
    //v generic nic nemuze byt, vracime prazdnou
    return *empty_node_generic_;
}

Generic_node & Generic_node::get_key(const string & key)
{
    //v generic nic nemuze byt, vracime prazdnou
    return *empty_node_generic_;
}

Generic_node & Generic_node::get_item(const size_t id, Generic_node & default_tree)
{
    //v generic nic nemuze byt, vracime default
    return default_tree;
}

Generic_node & Generic_node::get_key(const string & key, Generic_node & default_tree)
{
    //v generic nic nemuze byt, vracime default
    return default_tree;
}

Object_node & Generic_node::as_object(void)
{
   if ( value_type_ == type_object ) {
       return * dynamic_cast < Object_node * > (this) ;
   } else {
       //zkousime pristoupit jako k objektu, ale neni objekt - vrat empty
       return *empty_node_object_;
   }
}

Vector_node & Generic_node::as_vector(void)
{
    if ( value_type_ == type_vector ) {
        return * dynamic_cast < Vector_node * > (this);
    } else {
        //zkousime pristoupit jako k vektoru, ale neni vektor - vrat empty
        return *empty_node_vector_;
    }
}

Value_node & Generic_node::as_value(void)
{
    if ( ( value_type_ == type_string ) || ( value_type_ == type_number ) ||
            ( value_type_ == type_bool ) || ( value_type_ == type_null ) ) {
        return * dynamic_cast < Value_node * > (this);
    } else {
        //zkousime pristoupit jako k value, ale neni value - vrat empty
        return *empty_node_value_;
    }
}

ostream & operator<<(ostream & stream, const Generic_node & node)
{
    stream << "Node begin: Generic: ";
    stream << "type " << node.get_type() << ", ";
    stream << node.get_type_str() << ". ";
    stream << "Node end.";
    return stream;
}

Generic_node::~Generic_node()
{
}

}
