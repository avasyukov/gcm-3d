#include "libgcm/util/areas/Area.hpp"

gcm::Area::~Area() {
}

bool gcm::Area::isInArea(Node* cur_node) {
    return isInArea(*cur_node);
}