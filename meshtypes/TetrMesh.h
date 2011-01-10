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
	virtual Tetrahedron* find_owner_tetr(float x, float y, float z, ElasticNode* node) = 0;
	virtual int interpolate(ElasticNode* node, Tetrahedron* tetr) = 0;
protected:
	TetrNumericalMethod* method;
};

#include "TetrMesh.inl"

#endif
