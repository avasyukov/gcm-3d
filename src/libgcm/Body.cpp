#include "libgcm/Body.hpp"

string gcm::Body::getId() {
    return id;
}

gcm::Body::Body(string id) {
    this->id = id;
    rheoCalcType = "DummyRheologyCalculator";
    INIT_LOGGER("gcm.Body");
    LOG_INFO("Body '" << id << "' created");
}

gcm::Body::~Body() {
    // clear memory
    for(auto& m: meshes)
        delete m;
    LOG_INFO("Body destroyed");
}

void gcm::Body::attachMesh(Mesh* mesh) {
    meshes.push_back(mesh);
}

Mesh* gcm::Body::getMeshes() {
    return meshes.size() ?  meshes[0] : NULL;
}

Mesh* gcm::Body::getMesh(string id) {
    for(auto& mesh: meshes)
        if (mesh->getId() == id)
            return mesh;
    return NULL;
}

void gcm::Body::setInitialState(Area* area, float values[9]) {
    for( unsigned int i = 0; i < meshes.size(); i++ )
        meshes[i]->setInitialState(area, values);
}

void gcm::Body::setBorderCondition(Area* area, unsigned int num) {
    for( unsigned int i = 0; i < meshes.size(); i++ )
        meshes[i]->setBorderCondition(area, num);
}

void gcm::Body::setContactCondition(Area* area, unsigned int num) {
    for( unsigned int i = 0; i < meshes.size(); i++ )
        meshes[i]->setContactCondition(area, num);
}

void gcm::Body::setRheologyCalculatorType(string calcType) {
    rheoCalcType = calcType;
}

string gcm::Body::getRheologyCalculatorType() {
    return rheoCalcType;
}
        
const vector<Mesh*>& gcm::Body::getMeshesVector()
{
    return meshes;
}
