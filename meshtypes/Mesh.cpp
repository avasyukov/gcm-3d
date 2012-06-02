#include "Mesh.h"

Mesh::Mesh()
{
	rheology = NULL;
	stresser = NULL;
	mesh_type.assign("Generic mesh");
	current_time = 0;
};

Mesh::~Mesh() { };

void Mesh::attach(Stresser* new_stresser)
{
	stresser = new_stresser;
	if(stresser != NULL)
		*logger << "Attached stresser. Type: " < *(stresser->get_stresser_type());
};

void Mesh::attach(RheologyCalculator* new_rheology)
{
	rheology = new_rheology;
	if(rheology != NULL)
		*logger << "Attached rheology calculator. Type: " < *rheology->get_rheology_type();
};

string* Mesh::get_mesh_type()
{
	return &mesh_type;
};

float Mesh::get_current_time()
{
	return current_time;
};
