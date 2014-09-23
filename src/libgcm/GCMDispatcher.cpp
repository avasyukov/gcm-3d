#include "libgcm/GCMDispatcher.hpp"

using namespace gcm;

GCMDispatcher::GCMDispatcher() {
    INIT_LOGGER("gcm.Dispatcher");
    dX = 0;
    dY = 0;
    dZ = 0;
}

GCMDispatcher::~GCMDispatcher() {
}
