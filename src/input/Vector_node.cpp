#include "Vector_node.h"

namespace flow {

Generic_node & Vector_node::get_item(const size_t id) {
    if (id >= value_array_.size()) {
        //mimo rozsah pole, vratime prazndou
        return *empty_node_generic_;
    } else {
        return *value_array_[id];
    }
}

Generic_node & Vector_node::get_item(const size_t id, Generic_node & default_tree)
{
    if (id >= value_array_.size()) {
        //mimo rozsah pole, vratime default
        return default_tree;
    } else {
        return *value_array_[id];
    }
}

ostream & operator <<(ostream & stream, const Vector_node & node) {
    stream << "Node begin: Vector: ";
    stream << "type " << node.get_type() << ", ";
    stream << node.get_type_str() << ". ";
    stream << "Node end.";
    return stream;
}

Vector_node::~Vector_node() {
    //deep destructor?
    // TODO Auto-generated destructor stub
}

}
