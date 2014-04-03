#include "mesh/MeshLoader.h"

// init constants
const string gcm::MeshLoader::PARAM_CALC          = "calc";
const string gcm::MeshLoader::PARAM_FILE          = "file";
const string gcm::MeshLoader::PARAM_ID            = "id";
const string gcm::MeshLoader::PARAM_VALUE_TRUE    = "true";
const string gcm::MeshLoader::PARAM_ZONES         = "zones";
const string gcm::MeshLoader::PARAM_GENERATE_FROM = "generate-from";

gcm::MeshLoader::~MeshLoader() {

}

void gcm::MeshLoader::setEngine(IEngine* engine) {
    this->engine = engine;
}
