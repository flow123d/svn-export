#include "Keyconnector.hpp"

namespace flow {

Key_connector::~Key_connector() {
    declared_ = false;
    if ( default_node_ )
        default_node_->~Generic_node();
}

} /* namespace flow */
