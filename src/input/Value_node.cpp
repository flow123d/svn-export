#include "Value_node.h"

namespace flow {

ostream & operator <<(ostream & stream, const Value_node & node)
{
    stream << "Node begin: Value: ";
    stream << "type " << node.get_type() << ", ";
    stream << node.get_type_str() << ". ";
    stream << "Node end.";
    return stream;
}

bool Value_node::get_int(int & ret_value)
{
    //TODO: dodelat
    ret_value = 0;
    return true;
}

bool Value_node::get_int(int & ret_value, const int & default_value)
{
    //TODO: dodelat
    ret_value = 0;
    return true;
}

Value_node::~Value_node() {
    // TODO Auto-generated destructor stub
    //deep destructor?
}

}
