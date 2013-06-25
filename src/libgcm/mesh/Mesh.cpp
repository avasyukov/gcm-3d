#include "Mesh.h"

gcm::Mesh::Mesh() {
	calc = false;
	current_time = 0;
}

gcm::Mesh::~Mesh() {
	
}

string gcm::Mesh::getType() {
	return type;
}

void gcm::Mesh::setId(string id) {
	this->id = id;
}
string gcm::Mesh::getId() {
	return id;
}

void gcm::Mesh::setCalc(bool calc) {
	this->calc = calc;
}

bool gcm::Mesh::getCalc() {
	return calc;
}

void gcm::Mesh::setNodeFactory(NodeFactory* nodeFactory) {
	this->nodeFactory = nodeFactory;
}

void gcm::Mesh::setBody(IBody* body) {
	this->body = body;
}

IBody* gcm::Mesh::getBody() {
	return body;
}

void gcm::Mesh::update_current_time(float time_step)
{
	current_time += time_step;
};

float gcm::Mesh::get_current_time()
{
	return current_time;
};