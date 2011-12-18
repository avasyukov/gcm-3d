#ifndef _GCM_TETR_MESH_H
#define _GCM_TETR_MESH_H  1

#include "Mesh.h"
#include "../datatypes/ElasticNode.h"
#include "../datatypes/Tetrahedron.h"
#include "../methods/TetrNumericalMethod.h"
class TetrNumericalMethod;

class TetrMesh : public Mesh
{
public:
	TetrMesh();
	~TetrMesh();
	void attach(Logger* new_logger);
	void attach(TetrNumericalMethod* new_numerical_method);
	void attach(RheologyCalculator* new_rheology);
	void attach(Stresser* new_stresser);
	void attach(DataBus* new_data_bus);

	void translate(float x, float y, float z);

	virtual bool point_in_tetr(int base_node_index, float dx, float dy, float dz, Tetrahedron* tetr) = 0;
	virtual Tetrahedron* find_owner_tetr(ElasticNode* node, float dx, float dy, float dz) = 0;
	virtual int interpolate(ElasticNode* node, Tetrahedron* tetr) = 0;
	virtual Tetrahedron* get_tetrahedron(int index) = 0;
	virtual ElasticNode* get_node(int index) = 0;
	virtual int find_border_node_normal(int border_node_index, float* x, float* y, float* z) = 0;

	vector<ElasticNode> nodes;

	TetrNumericalMethod* method;
protected:
};

#include "TetrMesh.inl"

#endif
