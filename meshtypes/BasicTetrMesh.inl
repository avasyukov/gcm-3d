BasicTetrMesh::BasicTetrMesh()
{
	mesh_type.assign("Basic tetrahedron mesh");
};

BasicTetrMesh::~BasicTetrMesh()
{
	for(int i = 0; i < nodes.size(); i++)
		nodes[i].elements->clear();
	nodes.clear();
	new_nodes.clear();
	tetrs.clear();
};

// TODO move actual file and string operations into TaskPreparator or MshFileReader
int BasicTetrMesh::load_mesh(char* file_name)
{
	string str;
	int tmp_int;
	int number_of_nodes;
	int number_of_elements;
	ElasticNode new_node;
	Tetrahedron_1st_order new_tetr;

	ifstream infile;
	infile.open(file_name, ifstream::in);
	if(!infile.is_open()) {
		if(logger != NULL)
			logger->write(string("ERROR: BasicTetrMesh::load_mesh - can not open file"));
		return -1;
	}

	if(logger != NULL)
		logger->write(string("INFO: BasicTetrMesh::load_mesh - Reading file..."));

	infile >> str;
	if(strcmp(str.c_str(),"$MeshFormat") != 0) {
		if(logger != NULL)
			logger->write(string("ERROR: BasicTetrMesh::load_mesh - wrong file format. '$MeshFormat' expected."));
		return -1;
	}

	infile >> tmp_int >> tmp_int >> tmp_int;

	infile >> str;
	if(strcmp(str.c_str(),"$EndMeshFormat") != 0) {
		if(logger != NULL)
			logger->write(string("ERROR: BasicTetrMesh::load_mesh - wrong file format. '$EndMeshFormat' expected."));
		return -1;
	}

	infile >> str;
	if(strcmp(str.c_str(),"$Nodes") != 0) {
		if(logger != NULL)
			logger->write(string("ERROR: BasicTetrMesh::load_mesh - wrong file format. '$Nodes' expected."));
		return -1;
	}

	infile >> number_of_nodes;

	for(int i = 0; i < number_of_nodes; i++)
	{
		// Zero all values
		new_node.local_num = new_node.remote_num = new_node.absolute_num = new_node.zone_num = 0;
		new_node.coords[0] = new_node.coords[1] = new_node.coords[2] = 0;
		new_node.fixed_coords[0] = new_node.fixed_coords[1] = new_node.fixed_coords[2] = 0;
		new_node.la = new_node.mu = new_node.rho = 0;
		new_node.values[0] = new_node.values[1] = new_node.values[2] = 0;
		new_node.values[3] = new_node.values[4] = new_node.values[5] = 0;
		new_node.values[6] = new_node.values[7] = new_node.values[8] = 0;
		new_node.elements = NULL;

		infile >> new_node.local_num;
		if(new_node.local_num > 0)
		{
			new_node.local_num--;
			infile >> new_node.coords[0] >> new_node.coords[1] >> new_node.coords[2];
			new_node.placement_type = LOCAL;
			// TODO set other values
		}
		else if(new_node.local_num < 0)
		{
			new_node.local_num = -new_node.local_num;
			new_node.local_num--;
			infile >> new_node.zone_num >> new_node.remote_num;
			new_node.placement_type = REMOTE;
			new_node.remote_num--;
			// TODO set other values
		}
		else
		{
			if(logger != NULL)
				logger->write(string("ERROR: BasicTetrMesh::load_mesh - wrong file format. Node number can not be 0."));
			return -1;
		}
		nodes.push_back(new_node);
		new_nodes.push_back(new_node);
	}

	infile >> str;
	if(strcmp(str.c_str(),"$EndNodes") != 0) {
		if(logger != NULL)
			logger->write(string("ERROR: BasicTetrMesh::load_mesh - wrong file format. '$EndNodes' expected."));
		return -1;
	}

	infile >> str;
	if(strcmp(str.c_str(),"$Elements") != 0) {
		if(logger != NULL)
			logger->write(string("ERROR: BasicTetrMesh::load_mesh - wrong file format. '$Elements' expected."));
		return -1;
	}

	infile >> number_of_elements;
	for(int i = 0; i < number_of_elements; i++)
	{
		infile >> tmp_int >> tmp_int;
		if(tmp_int != 4) {
			getline(infile, str);
			continue;
		}
		new_tetr.local_num = tetrs.size();
		infile >> tmp_int >> tmp_int >> tmp_int >> tmp_int 
			>> new_tetr.vert[0] >> new_tetr.vert[1] >> new_tetr.vert[2] >> new_tetr.vert[3];

		if( (new_tetr.vert[0] <= 0) || (new_tetr.vert[1] <= 0) || (new_tetr.vert[2] <= 0) || (new_tetr.vert[3] <= 0) ) {
			if(logger != NULL)
				logger->write(string("ERROR: BasicTetrMesh::load_mesh - wrong file format. Vert number must be positive."));
			return -1;
		}

		new_tetr.vert[0]--; new_tetr.vert[1]--; new_tetr.vert[2]--; new_tetr.vert[3]--;

		tetrs.push_back(new_tetr);
	}

	infile >> str;
	if(strcmp(str.c_str(),"$EndElements") != 0) {
		if(logger != NULL)
			logger->write(string("ERROR: BasicTetrMesh::load_mesh - wrong file format. '$EndElements' expected."));
		return -1;
	}

	if(logger != NULL)
		logger->write(string("INFO: BasicTetrMesh::load_mesh - File read."));

	infile.close();

	// Check if internal numbers of nodes are the same as numbers in array
	// We need it in future to perform quick access to nodes in array
	for(int i = 0; i < nodes.size(); i++)
	{
		if(nodes[i].local_num != i) {
			if(logger != NULL)
				logger->write(string("ERROR: BasicTetrMesh::load_mesh - Invalid nodes numbering!"));
			return -1;
		}
	}
	for(int i = 0; i < tetrs.size(); i++)
	{
		if(tetrs[i].local_num != i) {
			if(logger != NULL)
				logger->write(string("ERROR: BasicTetrMesh::load_mesh - Invalid tetrahedron numbering!"));
			return -1;
		}
	}

	// Init vectors for "reverse lookups" of tetrahedrons current node is a member of.
	for(int i = 0; i < nodes.size(); i++) { nodes[i].elements = new vector<int>; }

	// Go through all the tetrahedrons
	for(int i = 0; i < tetrs.size(); i++)
	{
		// For all verticles
		for(int j = 0; j < 4; j++)
		{
			// Push to data of nodes the number of this tetrahedron
			nodes[tetrs[i].vert[j]].elements->push_back(i);
		}
	}

	// TODO Do we need this part?
	// Check all the nodes and find 'unused' - remote ones that have connections only with remote ones
	for(int i = 0; i < nodes.size(); i++)
	{
		// If node is remote
		if(nodes[i].placement_type == REMOTE)
		{
			int count = 0;
			// Check tetrahedrons it is a member of
			for(int j = 0; j < nodes[i].elements->size(); j++)
			{
				// Check verticles
				for(int k = 0; k < 4; k++)
				{
					// If it is local - count++
					if(nodes[tetrs[nodes[i].elements->at(j)].vert[k]].placement_type == LOCAL) { count++; }
				}
			}
			// If remote node is NOT connected with at least one local - it is unused one
			if(count == 0)
			{
				nodes[i].placement_type = UNUSED;
			}
		}
	}

	return 0;
};

// Finds minimum h over mesh
float BasicTetrMesh::get_min_h()
{
	float min_h = -1;
	// Go through tetrahedrons
	for(int i = 0; i < tetrs.size(); i++)
	{
		if ( (nodes[tetrs[i].vert[0]].placement_type == UNUSED) 
			|| (nodes[tetrs[i].vert[1]].placement_type == UNUSED) 
			|| (nodes[tetrs[i].vert[2]].placement_type == UNUSED) 
			|| (nodes[tetrs[i].vert[3]].placement_type == UNUSED) )
			continue;

		float h;
		float area[4];
		// Find volume
		float vol = qm_engine.tetr_volume(
			nodes[tetrs[i].vert[1]].coords[0] - nodes[tetrs[i].vert[0]].coords[0], 
			nodes[tetrs[i].vert[1]].coords[1] - nodes[tetrs[i].vert[0]].coords[1],
			nodes[tetrs[i].vert[1]].coords[2] - nodes[tetrs[i].vert[0]].coords[2],
			nodes[tetrs[i].vert[2]].coords[0] - nodes[tetrs[i].vert[0]].coords[0],
			nodes[tetrs[i].vert[2]].coords[1] - nodes[tetrs[i].vert[0]].coords[1],
			nodes[tetrs[i].vert[2]].coords[2] - nodes[tetrs[i].vert[0]].coords[2],
			nodes[tetrs[i].vert[3]].coords[0] - nodes[tetrs[i].vert[0]].coords[0],
			nodes[tetrs[i].vert[3]].coords[1] - nodes[tetrs[i].vert[0]].coords[1],
			nodes[tetrs[i].vert[3]].coords[2] - nodes[tetrs[i].vert[0]].coords[2]
		);

		// Find area of first face (verticles - 0,1,2)
		area[0] = qm_engine.tri_area(
			nodes[tetrs[i].vert[1]].coords[0] - nodes[tetrs[i].vert[0]].coords[0],
			nodes[tetrs[i].vert[1]].coords[1] - nodes[tetrs[i].vert[0]].coords[1],
			nodes[tetrs[i].vert[1]].coords[2] - nodes[tetrs[i].vert[0]].coords[2],
                	nodes[tetrs[i].vert[2]].coords[0] - nodes[tetrs[i].vert[0]].coords[0],
			nodes[tetrs[i].vert[2]].coords[1] - nodes[tetrs[i].vert[0]].coords[1],
			nodes[tetrs[i].vert[2]].coords[2] - nodes[tetrs[i].vert[0]].coords[2]
		);

		// Find area of second face (verticles - 0,1,3)
		area[1] = qm_engine.tri_area(
			nodes[tetrs[i].vert[1]].coords[0] - nodes[tetrs[i].vert[0]].coords[0],
			nodes[tetrs[i].vert[1]].coords[1] - nodes[tetrs[i].vert[0]].coords[1],
			nodes[tetrs[i].vert[1]].coords[2] - nodes[tetrs[i].vert[0]].coords[2],
			nodes[tetrs[i].vert[3]].coords[0] - nodes[tetrs[i].vert[0]].coords[0],
			nodes[tetrs[i].vert[3]].coords[1] - nodes[tetrs[i].vert[0]].coords[1],
			nodes[tetrs[i].vert[3]].coords[2] - nodes[tetrs[i].vert[0]].coords[2]
		);

		// Find area of third face (verticles - 0,2,3)
		area[2] = qm_engine.tri_area(
			nodes[tetrs[i].vert[2]].coords[0] - nodes[tetrs[i].vert[0]].coords[0],
			nodes[tetrs[i].vert[2]].coords[1] - nodes[tetrs[i].vert[0]].coords[1],
			nodes[tetrs[i].vert[2]].coords[2] - nodes[tetrs[i].vert[0]].coords[2],
	                nodes[tetrs[i].vert[3]].coords[0] - nodes[tetrs[i].vert[0]].coords[0],
			nodes[tetrs[i].vert[3]].coords[1] - nodes[tetrs[i].vert[0]].coords[1],
			nodes[tetrs[i].vert[3]].coords[2] - nodes[tetrs[i].vert[0]].coords[2]
		);

		// Find area of third face (verticles - 1,2,3)
		area[3] = qm_engine.tri_area(
			nodes[tetrs[i].vert[2]].coords[0] - nodes[tetrs[i].vert[1]].coords[0],
			nodes[tetrs[i].vert[2]].coords[1] - nodes[tetrs[i].vert[1]].coords[1],
			nodes[tetrs[i].vert[2]].coords[2] - nodes[tetrs[i].vert[1]].coords[2],
                	nodes[tetrs[i].vert[3]].coords[0] - nodes[tetrs[i].vert[1]].coords[0],
			nodes[tetrs[i].vert[3]].coords[1] - nodes[tetrs[i].vert[1]].coords[1],
			nodes[tetrs[i].vert[3]].coords[2] - nodes[tetrs[i].vert[1]].coords[2]
		);

		// Check if all nodes are already loaded from other CPUs and tetrahadron is correct
		if(vol == 0) {
			if(logger != NULL)
				logger->write(string("ERROR: BasicTetrMesh::min_h - Volume is zero!"));
			return -1;
		}

		for(int j = 0; j < 4; j++)
		{
			if(area[j] == 0) {
				if(logger != NULL)
					logger->write(string("ERROR: BasicTetrMesh::min_h - Face area is zero!"));
				return -1;
			}
		}

		// If min_h is negative (inital step) - let it be smth positive
		if(min_h < 0) { min_h = fabs(3*vol/area[0]); }
		// Go through all faces of given tetrahedron
		for(int j = 0; j < 4; j++)
		{
			// Find height to this face
			h = fabs(3*vol/area[j]);
			// And check if we should update minimum height
			if(h < min_h) { min_h = h; }
		}
	}

	return min_h;
};

bool BasicTetrMesh::point_in_tetr(float x, float y, float z, Tetrahedron_1st_order* tetr)
{
	float d1,d2;
	d1 = qm_engine.determinant(
		nodes[tetr->vert[1]].coords[0] - nodes[tetr->vert[0]].coords[0],
		nodes[tetr->vert[1]].coords[1] - nodes[tetr->vert[0]].coords[1],
		nodes[tetr->vert[1]].coords[2] - nodes[tetr->vert[0]].coords[2],
		nodes[tetr->vert[2]].coords[0] - nodes[tetr->vert[0]].coords[0],
		nodes[tetr->vert[2]].coords[1] - nodes[tetr->vert[0]].coords[1],
		nodes[tetr->vert[2]].coords[2] - nodes[tetr->vert[0]].coords[2],
		nodes[tetr->vert[3]].coords[0] - nodes[tetr->vert[0]].coords[0],
		nodes[tetr->vert[3]].coords[1] - nodes[tetr->vert[0]].coords[1],
		nodes[tetr->vert[3]].coords[2] - nodes[tetr->vert[0]].coords[2]
	);
	d2 = qm_engine.determinant(
		nodes[tetr->vert[1]].coords[0] - x,
		nodes[tetr->vert[1]].coords[1] - y,
		nodes[tetr->vert[1]].coords[2] - z,
		nodes[tetr->vert[2]].coords[0] - x,
		nodes[tetr->vert[2]].coords[1] - y,
		nodes[tetr->vert[2]].coords[2] - z,
		nodes[tetr->vert[3]].coords[0] - x,
		nodes[tetr->vert[3]].coords[1] - y,
		nodes[tetr->vert[3]].coords[2] - z
	);
	if(d1*d2 < 0) { return false; }

	d1 = qm_engine.determinant(
		nodes[tetr->vert[0]].coords[0] - nodes[tetr->vert[1]].coords[0],
		nodes[tetr->vert[0]].coords[1] - nodes[tetr->vert[1]].coords[1],
		nodes[tetr->vert[0]].coords[2] - nodes[tetr->vert[1]].coords[2],
		nodes[tetr->vert[2]].coords[0] - nodes[tetr->vert[1]].coords[0],
		nodes[tetr->vert[2]].coords[1] - nodes[tetr->vert[1]].coords[1],
		nodes[tetr->vert[2]].coords[2] - nodes[tetr->vert[1]].coords[2],
		nodes[tetr->vert[3]].coords[0] - nodes[tetr->vert[1]].coords[0],
		nodes[tetr->vert[3]].coords[1] - nodes[tetr->vert[1]].coords[1],
		nodes[tetr->vert[3]].coords[2] - nodes[tetr->vert[1]].coords[2]
	);
	d2 = qm_engine.determinant(
		nodes[tetr->vert[0]].coords[0] - x,
		nodes[tetr->vert[0]].coords[1] - y,
		nodes[tetr->vert[0]].coords[2] - z,
		nodes[tetr->vert[2]].coords[0] - x,
		nodes[tetr->vert[2]].coords[1] - y,
		nodes[tetr->vert[2]].coords[2] - z,
		nodes[tetr->vert[3]].coords[0] - x,
		nodes[tetr->vert[3]].coords[1] - y,
		nodes[tetr->vert[3]].coords[2] - z
	);
	if(d1*d2 < 0) { return false; }

	d1 = qm_engine.determinant(
		nodes[tetr->vert[0]].coords[0] - nodes[tetr->vert[2]].coords[0],
		nodes[tetr->vert[0]].coords[1] - nodes[tetr->vert[2]].coords[1],
		nodes[tetr->vert[0]].coords[2] - nodes[tetr->vert[2]].coords[2],
		nodes[tetr->vert[1]].coords[0] - nodes[tetr->vert[2]].coords[0],
		nodes[tetr->vert[1]].coords[1] - nodes[tetr->vert[2]].coords[1],
		nodes[tetr->vert[1]].coords[2] - nodes[tetr->vert[2]].coords[2],
		nodes[tetr->vert[3]].coords[0] - nodes[tetr->vert[2]].coords[0],
		nodes[tetr->vert[3]].coords[1] - nodes[tetr->vert[2]].coords[1],
		nodes[tetr->vert[3]].coords[2] - nodes[tetr->vert[2]].coords[2]
	);
	d2 = qm_engine.determinant(
		nodes[tetr->vert[0]].coords[0] - x,
		nodes[tetr->vert[0]].coords[1] - y,
		nodes[tetr->vert[0]].coords[2] - z,
		nodes[tetr->vert[1]].coords[0] - x,
		nodes[tetr->vert[1]].coords[1] - y,
		nodes[tetr->vert[1]].coords[2] - z,
		nodes[tetr->vert[3]].coords[0] - x,
		nodes[tetr->vert[3]].coords[1] - y,
		nodes[tetr->vert[3]].coords[2] - z
	);
	if(d1*d2 < 0) { return false; }

	d1 = qm_engine.determinant(
		nodes[tetr->vert[0]].coords[0] - nodes[tetr->vert[3]].coords[0],
		nodes[tetr->vert[0]].coords[1] - nodes[tetr->vert[3]].coords[1],
		nodes[tetr->vert[0]].coords[2] - nodes[tetr->vert[3]].coords[2],
		nodes[tetr->vert[1]].coords[0] - nodes[tetr->vert[3]].coords[0],
		nodes[tetr->vert[1]].coords[1] - nodes[tetr->vert[3]].coords[1],
		nodes[tetr->vert[1]].coords[2] - nodes[tetr->vert[3]].coords[2],
		nodes[tetr->vert[2]].coords[0] - nodes[tetr->vert[3]].coords[0],
		nodes[tetr->vert[2]].coords[1] - nodes[tetr->vert[3]].coords[1],
		nodes[tetr->vert[2]].coords[2] - nodes[tetr->vert[3]].coords[2]
	);
	d2 = qm_engine.determinant(
		nodes[tetr->vert[0]].coords[0] - x,
		nodes[tetr->vert[0]].coords[1] - y,
		nodes[tetr->vert[0]].coords[2] - z,
		nodes[tetr->vert[1]].coords[0] - x,
		nodes[tetr->vert[1]].coords[1] - y,
		nodes[tetr->vert[1]].coords[2] - z,
		nodes[tetr->vert[2]].coords[0] - x,
		nodes[tetr->vert[2]].coords[1] - y,
		nodes[tetr->vert[2]].coords[2] - z
	);
	if(d1*d2 < 0) { return false; }

	return true;
};

Tetrahedron_1st_order* BasicTetrMesh::find_owner_tetr(float x, float y, float z, ElasticNode* node)
{
	for(int i = 0; i < (node->elements)->size(); i++)
	{
		if( point_in_tetr(x, y, z, &tetrs[(node->elements)->at(i)]) )
		{
			return &tetrs[(node->elements)->at(i)];
		}
	}
	return NULL;
};

int BasicTetrMesh::interpolate(ElasticNode* node, Tetrahedron_1st_order* tetr)
{
	float Vol = qm_engine.tetr_volume(
		(nodes[tetr->vert[1]].coords[0])-(nodes[tetr->vert[0]].coords[0]),
		(nodes[tetr->vert[1]].coords[1])-(nodes[tetr->vert[0]].coords[1]),
		(nodes[tetr->vert[1]].coords[2])-(nodes[tetr->vert[0]].coords[2]),
		(nodes[tetr->vert[2]].coords[0])-(nodes[tetr->vert[0]].coords[0]),
		(nodes[tetr->vert[2]].coords[1])-(nodes[tetr->vert[0]].coords[1]),
		(nodes[tetr->vert[2]].coords[2])-(nodes[tetr->vert[0]].coords[2]),
		(nodes[tetr->vert[3]].coords[0])-(nodes[tetr->vert[0]].coords[0]),
		(nodes[tetr->vert[3]].coords[1])-(nodes[tetr->vert[0]].coords[1]),
		(nodes[tetr->vert[3]].coords[2])-(nodes[tetr->vert[0]].coords[2])
	);

	float factor[4];

	factor[0] = fabs( qm_engine.tetr_volume(
		(nodes[tetr->vert[1]].coords[0])-(node->coords[0]),
		(nodes[tetr->vert[1]].coords[1])-(node->coords[1]),
		(nodes[tetr->vert[1]].coords[2])-(node->coords[2]),
		(nodes[tetr->vert[2]].coords[0])-(node->coords[0]),
		(nodes[tetr->vert[2]].coords[1])-(node->coords[1]),
		(nodes[tetr->vert[2]].coords[2])-(node->coords[2]),
		(nodes[tetr->vert[3]].coords[0])-(node->coords[0]),
		(nodes[tetr->vert[3]].coords[1])-(node->coords[1]),
		(nodes[tetr->vert[3]].coords[2])-(node->coords[2])
	) / Vol);

	factor[1] = fabs( qm_engine.tetr_volume(
		(nodes[tetr->vert[0]].coords[0])-(node->coords[0]),
		(nodes[tetr->vert[0]].coords[1])-(node->coords[1]),
		(nodes[tetr->vert[0]].coords[2])-(node->coords[2]),
		(nodes[tetr->vert[2]].coords[0])-(node->coords[0]),
		(nodes[tetr->vert[2]].coords[1])-(node->coords[1]),
		(nodes[tetr->vert[2]].coords[2])-(node->coords[2]),
		(nodes[tetr->vert[3]].coords[0])-(node->coords[0]),
		(nodes[tetr->vert[3]].coords[1])-(node->coords[1]),
		(nodes[tetr->vert[3]].coords[2])-(node->coords[2])
	) / Vol);

	factor[2] = fabs( qm_engine.tetr_volume(
		(nodes[tetr->vert[1]].coords[0])-(node->coords[0]),
		(nodes[tetr->vert[1]].coords[1])-(node->coords[1]),
		(nodes[tetr->vert[1]].coords[2])-(node->coords[2]),
		(nodes[tetr->vert[0]].coords[0])-(node->coords[0]),
		(nodes[tetr->vert[0]].coords[1])-(node->coords[1]),
		(nodes[tetr->vert[0]].coords[2])-(node->coords[2]),
		(nodes[tetr->vert[3]].coords[0])-(node->coords[0]),
		(nodes[tetr->vert[3]].coords[1])-(node->coords[1]),
		(nodes[tetr->vert[3]].coords[2])-(node->coords[2])
	) / Vol);

	factor[3] = fabs( qm_engine.tetr_volume(
		(nodes[tetr->vert[1]].coords[0])-(node->coords[0]),
		(nodes[tetr->vert[1]].coords[1])-(node->coords[1]),
		(nodes[tetr->vert[1]].coords[2])-(node->coords[2]),
		(nodes[tetr->vert[2]].coords[0])-(node->coords[0]),
		(nodes[tetr->vert[2]].coords[1])-(node->coords[1]),
		(nodes[tetr->vert[2]].coords[2])-(node->coords[2]),
		(nodes[tetr->vert[0]].coords[0])-(node->coords[0]),
		(nodes[tetr->vert[0]].coords[1])-(node->coords[1]),
		(nodes[tetr->vert[0]].coords[2])-(node->coords[2])
	) / Vol);

	if(factor[0] + factor[1] + factor[2] + factor[3] > 1.01) {
		if(logger != NULL)
			logger->write(string("ERROR: BasicTetrMesh::interpolate - Sum of factors is greater than 1.01!"));
		return -1;
	}

	for (int i = 0; i < 3; i++)
	{
		node->fixed_coords[i] = node->coords[i];
	}

	for (int i = 0; i < 9; i++)
	{
		node->values[i] = (nodes[tetr->vert[0]].values[i]*factor[0] 
				+ nodes[tetr->vert[1]].values[i]*factor[1] 
				+ nodes[tetr->vert[2]].values[i]*factor[2] 
				+ nodes[tetr->vert[3]].values[i]*factor[3]);
	}

        node->la = (nodes[tetr->vert[0]].la*factor[0] + nodes[tetr->vert[1]].la*factor[1] 
			+ nodes[tetr->vert[2]].la*factor[2] + nodes[tetr->vert[3]].la*factor[3]);
        node->mu = (nodes[tetr->vert[0]].mu*factor[0] + nodes[tetr->vert[1]].mu*factor[1] 
			+ nodes[tetr->vert[2]].mu*factor[2] + nodes[tetr->vert[3]].mu*factor[3]);
        node->rho = (nodes[tetr->vert[0]].rho*factor[0] + nodes[tetr->vert[1]].rho*factor[1] 
			+ nodes[tetr->vert[2]].rho*factor[2] + nodes[tetr->vert[3]].rho*factor[3]);

	return 0;
};

float BasicTetrMesh::get_max_possible_tau()
{
	float min_h = get_min_h();
	float max_l = 0;
	float l;

	for(int i = 0; i < nodes.size(); i++)
	{
		if(nodes[i].placement_type == LOCAL)
		{
			l = method->get_max_lambda(&nodes[i]);
			if(l > max_l) { max_l = l; }
		}
	}

	return min_h/max_l;
};

void BasicTetrMesh::do_next_part_step(float tau, int stage)
{
	for(int i = 0; i < nodes.size(); i++)
	{
		if(nodes[i].placement_type == LOCAL)
		{
			method->do_next_part_step(&nodes[i],&new_nodes[i],tau,stage);
		}
	}
	for(int i = 0; i < nodes.size(); i++)
	{
		if(nodes[i].placement_type == LOCAL)
		{
			for(int j = 0; j < 9; j++)
				nodes[i].values[j] = new_nodes[i].values[j];
		}
	}
};

void BasicTetrMesh::move_coords(float tau)
{
	for(int i = 0; i < nodes.size(); i++)
	{
		if(nodes[i].placement_type == LOCAL)
		{
			for(int j = 0; j < 3; j++)
			{
				nodes[i].coords[j] += nodes[i].values[j]*tau;
			}
		}
	}
};
