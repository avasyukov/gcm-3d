TestCubicMesh::TestCubicMesh()
{
	mesh_type.assign("Test cubic mesh");
};

int TestCubicMesh::create(int points_per_edge, float h)
{
	int index;
	_points_per_edge = points_per_edge;
	for(int i = 0; i < points_per_edge; i++)
		for(int j = 0; j < points_per_edge; j++)
			for(int k = 0; k < points_per_edge; k++)
			{
				nodes.push_back(ElasticNode());
				index = get_point_index(i,j,k);
				nodes.at(index).local_num = index;
				nodes.at(index).coords[0] = nodes.at(index).fixed_coords[0] = i*h;
				nodes.at(index).coords[1] = nodes.at(index).fixed_coords[1] = j*h;
				nodes.at(index).coords[2] = nodes.at(index).fixed_coords[2] = k*h;
			}
	return 0;
};

int TestCubicMesh::get_point_index(int i, int j, int k)
{
	return _points_per_edge*_points_per_edge*i + _points_per_edge*j + k;
};
