#ifndef _GCM_TEST_CUBIC_MESH_H
#define _GCM_TEST_CUBIC_MESH_H  1

#include "Mesh.h"

class TestCubicMesh : public Mesh
{
public:
	TestCubicMesh();
	~TestCubicMesh();
	int create(int points_per_edge, float h);
private:
	int get_point_index(int i, int j, int k);
	int _points_per_edge;
	vector<ElasticNode> nodes;
};

#include "TestCubicMesh.inl"

#endif
