#include "TetrMesh.h"
#include "../system/TetrMeshSet.h"

TetrMesh::TetrMesh()
{
	mesh_type.assign("Basic tetrahedron mesh");
	mesh_set = TetrMeshSet::getInstance();
};

TetrMesh::~TetrMesh() { };

void TetrMesh::attach(TetrNumericalMethod* new_numerical_method)
{
	method = new_numerical_method;
	if(method != NULL)
		*logger << "Attached numerical method. Type: " < *(method->get_num_method_type());
};

void TetrMesh::attach(RheologyCalculator* new_rheology)
{
	Mesh::attach(new_rheology);
};

void TetrMesh::attach(Stresser* new_stresser)
{
	Mesh::attach(new_stresser);
};

void TetrMesh::translate(float x, float y, float z)
{
	for(int i = 0; i < nodes.size(); i++) {
		nodes[i].coords[0] += x;
		nodes[i].coords[1] += y;
		nodes[i].coords[2] += z;
		nodes[i].fixed_coords[0] += x;
		nodes[i].fixed_coords[1] += y;
		nodes[i].fixed_coords[2] += z;
	}
	outline.min_coords[0] += x;
	outline.min_coords[1] += y;
	outline.min_coords[2] += z;
	outline.max_coords[0] += x;
	outline.max_coords[1] += y;
	outline.max_coords[2] += z;
};

void TetrMesh::clear_contact_data(ElasticNode* node)
{
	if(node->contact_data != NULL) {
		node->contact_data->axis_plus[0] = -1;
		node->contact_data->axis_plus[1] = -1;
		node->contact_data->axis_plus[2] = -1;
		node->contact_data->axis_minus[0] = -1;
		node->contact_data->axis_minus[1] = -1;
		node->contact_data->axis_minus[2] = -1;
	}
};