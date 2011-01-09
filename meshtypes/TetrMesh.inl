TetrMesh::TetrMesh()
{
	mesh_type.assign("Basic tetrahedron mesh");
};

TetrMesh::~TetrMesh()
{
	for(int i = 0; i < nodes.size(); i++)
		nodes[i].elements->clear();
	nodes.clear();
	tetrs.clear();
};
