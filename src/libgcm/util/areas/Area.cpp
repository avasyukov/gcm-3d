#include "libgcm/util/areas/Area.hpp"

using namespace gcm;

Area::~Area() {
}

bool Area::isInArea(const Node* cur_node) const {
    return isInArea(*cur_node);
}
