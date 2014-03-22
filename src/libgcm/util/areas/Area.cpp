#include "util/areas/Area.h"

gcm::Area::~Area() {
}

bool gcm::Area::isInArea(Node* cur_node) {
    return isInArea(*cur_node);
}