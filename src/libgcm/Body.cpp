#include "libgcm/Body.hpp"

using namespace gcm;

using std::string;
using std::vector;

string Body::getId() {
    return id;
}

Body::Body(string id) {
    this->id = id;
    rheoCalcType = "DummyRheologyCalculator";
    INIT_LOGGER("gcm.Body");
    LOG_INFO("Body '" << id << "' created");
}

Body::~Body() {
    // clear memory
    for(auto& m: meshes)
        delete m;
    LOG_INFO("Body destroyed");
}

void Body::attachMesh(Mesh* mesh) {
    meshes.push_back(mesh);
}

Mesh* Body::getMeshes() {
    return meshes.size() ?  meshes[0] : NULL;
}

Mesh* Body::getMesh(string id) {
    for(auto& mesh: meshes)
        if (mesh->getId() == id)
            return mesh;
    return NULL;
}

void Body::setInitialState(Area* area, float values[9]) {
    for( unsigned int i = 0; i < meshes.size(); i++ )
        meshes[i]->setInitialState(area, values);
}

void Body::setInitialState(Area* area, std::function<void(CalcNode&)> setter) {
    for( unsigned int i = 0; i < meshes.size(); i++ )
        meshes[i]->setInitialState(area, setter);
}

void Body::setBorderCondition(Area* area, unsigned int num) {
    for( unsigned int i = 0; i < meshes.size(); i++ )
        meshes[i]->setBorderCondition(area, num);
}

void Body::setContactCondition(Area* area, unsigned int num) {
    for( unsigned int i = 0; i < meshes.size(); i++ )
        meshes[i]->setContactCondition(area, num);
}

void Body::setRheologyCalculatorType(string calcType) {
    rheoCalcType = calcType;
}

string Body::getRheologyCalculatorType() {
    return rheoCalcType;
}
        
const vector<Mesh*>& Body::getMeshesVector()
{
    return meshes;
}
