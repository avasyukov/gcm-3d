#ifndef _GCM_TETR_MESH_H
#define _GCM_TETR_MESH_H  1

#include "Mesh.h"
#include "../datatypes/ElasticNode.h"
#include "../datatypes/Tetrahedron.h"

class TetrMesh : public Mesh
{
public:
	TetrMesh();
	~TetrMesh();
	virtual Tetrahedron* find_owner_tetr(float x, float y, float z, ElasticNode* node) = 0;
	virtual int interpolate(ElasticNode* node, Tetrahedron* tetr) = 0;
	vector<ElasticNode> nodes;
private:
	vector<Element> tetrs;
};

#include "TetrMesh.inl"

#endif
