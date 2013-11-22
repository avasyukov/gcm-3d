#include "Material.h"

gcm::Material::Material(string _id) {
	id = _id;
	la = -1;
	mu = -1;
	rho = -1;
}

gcm::Material::~Material() {
	
}

void gcm::Material::setRho(float _rho) {
	rho = _rho;
}

void gcm::Material::setCrackThreshold(float _ct) {
        crackThreshold = _ct;
}

void gcm::Material::setAdhesionThreshold(float _at) {
        adhesionThreshold = _at;
}

void gcm::Material::setLame(float _la, float _mu) {
	la = _la;
	mu = _mu;
}

string gcm::Material::getId() {
	return id;
}

float gcm::Material::getLambda() {
	return la;
}

float gcm::Material::getMu() {
	return mu;
}

float gcm::Material::getRho() {
	return rho;
}

float gcm::Material::getCrackThreshold() {
        return crackThreshold;
}

float gcm::Material::getAdhesionThreshold() {
        return adhesionThreshold;
}

