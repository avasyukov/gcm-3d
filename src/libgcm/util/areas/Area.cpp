#include "libgcm/util/areas/Area.hpp"

using namespace gcm;

Area::~Area() {
}

bool Area::isInArea(Node* cur_node) {
    return isInArea(*cur_node);
}
