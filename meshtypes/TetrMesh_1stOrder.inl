TetrMesh_1stOrder::TetrMesh_1stOrder()
{
	mesh_type.assign("Tetrahedron mesh 1st order");
	clear_mesh_stats();
};

TetrMesh_1stOrder::~TetrMesh_1stOrder()
{
	for(int i = 0; i < nodes.size(); i++)
		nodes[i].elements->clear();
	nodes.clear();
	new_nodes.clear();
	tetrs.clear();
};

void TetrMesh_1stOrder::clear_mesh_stats()
{
	mesh_stats.find_owner_tetr_quick_searches = 0;
	mesh_stats.find_owner_tetr_long_searches = 0;
};

//TODO - ugly hack - think about if we need it and how to implement it
void TetrMesh_1stOrder::add_node(ElasticNode* node)
{
	ElasticNode new_node = *node;
	nodes.push_back(new_node);
	new_nodes.push_back(new_node);
};

//TODO - ugly hack - think about if we need it and how to implement it
void TetrMesh_1stOrder::add_tetr(Tetrahedron_1st_order* tetr)
{
	Tetrahedron_1st_order new_tetr = *tetr;
	tetrs.push_back(new_tetr);
};

// TODO - add (1) auto scale and (2) scale back
int TetrMesh_1stOrder::pre_process_mesh(float scale_factor)
{
	// If node has no connections - mark it as unused
	for(int i = 0; i < nodes.size(); i++)
		if( (nodes[i].elements)->size() == 0 )
			nodes[i].placement_type = UNUSED;

	// Scale mesh
	for(int i = 0; i < nodes.size(); i++)
		for(int j = 0; j < 3; j++)
			nodes[i].coords[j] *= scale_factor;

	return 0;
};

// TODO move actual file and string operations into TaskPreparator or MshFileReader
int TetrMesh_1stOrder::load_node_ele_files(char* node_file_name, char* ele_file_name)
{
	int tmp_int;

	int number_of_nodes;
	int number_of_elements;
	ElasticNode new_node;
	Tetrahedron_1st_order new_tetr;

	ifstream node_infile;
	ifstream ele_infile;

	node_infile.open(node_file_name, ifstream::in);
	if(!node_infile.is_open()) {
		if(logger != NULL)
			logger->write(string("ERROR: TetrMesh_1stOrder::load_node_ele_files - can not open node file"));
		return -1;
	}

	ele_infile.open(ele_file_name, ifstream::in);
	if(!ele_infile.is_open()) {
		if(logger != NULL)
			logger->write(string("ERROR: TetrMesh_1stOrder::load_node_ele_files - can not open ele file"));
		return -1;
	}

	if(logger != NULL)
		logger->write(string("INFO: TetrMesh_1stOrder::load_node_ele_files - Reading file..."));

	node_infile >> number_of_nodes >> tmp_int >> tmp_int >> tmp_int;

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

		node_infile >> new_node.local_num;
		if(new_node.local_num > 0)
		{
			new_node.local_num--;
			node_infile >> new_node.coords[0] >> new_node.coords[1] >> new_node.coords[2];
			new_node.placement_type = LOCAL;
			// TODO set other values
		}
		else
		{
			if(logger != NULL)
				logger->write(string("ERROR: TetrMesh_1stOrder::load_node_ele_files - wrong file format. Node number can not be <= 0."));
			return -1;
		}
		nodes.push_back(new_node);
		new_nodes.push_back(new_node);
	}

	ele_infile >> number_of_elements >> tmp_int >> tmp_int;

	for(int i = 0; i < number_of_elements; i++)
	{
		ele_infile >> new_tetr.local_num >> new_tetr.vert[0] >> new_tetr.vert[1] >> new_tetr.vert[2] >> new_tetr.vert[3];

		if( (new_tetr.vert[0] <= 0) || (new_tetr.vert[1] <= 0) || (new_tetr.vert[2] <= 0) || (new_tetr.vert[3] <= 0) ) {
			if(logger != NULL)
				logger->write(string("ERROR: TetrMesh_1stOrder::load_node_ele_files - wrong file format. Vert number must be positive."));
			return -1;
		}

		new_tetr.vert[0]--; new_tetr.vert[1]--; new_tetr.vert[2]--; new_tetr.vert[3]--;
		new_tetr.local_num--;

		tetrs.push_back(new_tetr);
	}

	if(logger != NULL)
		logger->write(string("INFO: TetrMesh_1stOrder::load_node_ele_files - File read."));

	node_infile.close();
	ele_infile.close();

	// Check if internal numbers of nodes are the same as numbers in array
	// We need it in future to perform quick access to nodes in array
	for(int i = 0; i < nodes.size(); i++)
	{
		if(nodes[i].local_num != i) {
			if(logger != NULL)
				logger->write(string("ERROR: TetrMesh_1stOrder::load_node_ele_files - Invalid nodes numbering!"));
			return -1;
		}
	}
	for(int i = 0; i < tetrs.size(); i++)
	{
		if(tetrs[i].local_num != i) {
			if(logger != NULL)
				logger->write(string("ERROR: TetrMesh_1stOrder::load_node_ele_files - Invalid tetrahedron numbering!"));
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

	// TODO - move this part to mesh pre-processing. It is not specific for any file type.
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

// TODO move actual file and string operations into TaskPreparator or MshFileReader
int TetrMesh_1stOrder::load_gmv_file(char* file_name)
{
	string str;
	int tmp_int;
	int count;

	int number_of_nodes;
	int number_of_elements;
	ElasticNode new_node;
	Tetrahedron_1st_order new_tetr;

	ifstream infile;

	infile.open(file_name, ifstream::in);
	if(!infile.is_open()) {
		if(logger != NULL)
			logger->write(string("ERROR: TetrMesh_1stOrder::load_gmv_file - can not open node file"));
		return -1;
	}

	if(logger != NULL)
		logger->write(string("INFO: TetrMesh_1stOrder::load_gmv_file - Reading file..."));

	getline(infile, str);
	if(strcmp(str.c_str(),"gmvinput ascii") != 0) {
		if(logger != NULL)
			logger->write(string("ERROR: TetrMesh_1stOrder::load_gmv_file - wrong file format. 'gmvinput ascii' expected."));
		return -1;
	}

	getline(infile, str);

	infile >> str >> number_of_nodes;

	cout << "NODES: " << number_of_nodes << endl;

	count = 1;
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

		new_node.local_num = count - 1;
		infile >> new_node.coords[0] >> new_node.coords[1] >> new_node.coords[2];

		nodes.push_back(new_node);
		new_nodes.push_back(new_node);

		count++;
	}

	getline(infile, str);

	infile >> str >> number_of_elements;

	cout << "ELEMS: " << number_of_elements << endl;

	count = 1;
	for(int i = 0; i < number_of_elements; i++)
	{
		new_tetr.local_num = count;

		infile >> str >> tmp_int >> new_tetr.vert[0] >> new_tetr.vert[1] >> new_tetr.vert[2] >> new_tetr.vert[3];

		if( (new_tetr.vert[0] <= 0) || (new_tetr.vert[1] <= 0) || (new_tetr.vert[2] <= 0) || (new_tetr.vert[3] <= 0) ) {
			if(logger != NULL)
				logger->write(string("ERROR: TetrMesh_1stOrder::load_gmv_file - wrong file format. Vert number must be positive."));
			return -1;
		}

		new_tetr.vert[0]--; new_tetr.vert[1]--; new_tetr.vert[2]--; new_tetr.vert[3]--;
		new_tetr.local_num--;

		tetrs.push_back(new_tetr);

		count++;
	}

	cout << "1ST TET: " << tetrs[0].vert[0] << " " << tetrs[0].vert[1] << " " << tetrs[0].vert[2] << " " << tetrs[0].vert[3] << endl;

	if(logger != NULL)
		logger->write(string("INFO: TetrMesh_1stOrder::load_gmv_file - File read."));

	infile.close();

	// Check if internal numbers of nodes are the same as numbers in array
	// We need it in future to perform quick access to nodes in array
	for(int i = 0; i < nodes.size(); i++)
	{
		if(nodes[i].local_num != i) {
			if(logger != NULL)
				logger->write(string("ERROR: TetrMesh_1stOrder::load_gmv_file - Invalid nodes numbering!"));
			return -1;
		}
	}
	for(int i = 0; i < tetrs.size(); i++)
	{
		if(tetrs[i].local_num != i) {
			if(logger != NULL)
				logger->write(string("ERROR: TetrMesh_1stOrder::load_gmv_file - Invalid tetrahedron numbering!"));
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

// TODO move actual file and string operations into TaskPreparator or MshFileReader
int TetrMesh_1stOrder::load_msh_file(char* file_name)
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
			logger->write(string("ERROR: TetrMesh_1stOrder::load_msh_file - can not open file"));
		return -1;
	}

	if(logger != NULL)
		logger->write(string("INFO: TetrMesh_1stOrder::load_msh_file - Reading file..."));

	infile >> str;
	if(strcmp(str.c_str(),"$MeshFormat") != 0) {
		if(logger != NULL)
			logger->write(string("ERROR: TetrMesh_1stOrder::load_msh_file - wrong file format. '$MeshFormat' expected."));
		return -1;
	}

	infile >> tmp_int >> tmp_int >> tmp_int;

	infile >> str;
	if(strcmp(str.c_str(),"$EndMeshFormat") != 0) {
		if(logger != NULL)
			logger->write(string("ERROR: TetrMesh_1stOrder::load_msh_file - wrong file format. '$EndMeshFormat' expected."));
		return -1;
	}

	infile >> str;
	if(strcmp(str.c_str(),"$Nodes") != 0) {
		if(logger != NULL)
			logger->write(string("ERROR: TetrMesh_1stOrder::load_msh_file - wrong file format. '$Nodes' expected."));
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
				logger->write(string("ERROR: TetrMesh_1stOrder::load_msh_file - wrong file format. Node number can not be 0."));
			return -1;
		}
		nodes.push_back(new_node);
		new_nodes.push_back(new_node);
	}

	infile >> str;
	if(strcmp(str.c_str(),"$EndNodes") != 0) {
		if(logger != NULL)
			logger->write(string("ERROR: TetrMesh_1stOrder::load_msh_file - wrong file format. '$EndNodes' expected."));
		return -1;
	}

	infile >> str;
	if(strcmp(str.c_str(),"$Elements") != 0) {
		if(logger != NULL)
			logger->write(string("ERROR: TetrMesh_1stOrder::load_msh_file - wrong file format. '$Elements' expected."));
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
				logger->write(string("ERROR: TetrMesh_1stOrder::load_msh_file - wrong file format. Vert number must be positive."));
			return -1;
		}

		new_tetr.vert[0]--; new_tetr.vert[1]--; new_tetr.vert[2]--; new_tetr.vert[3]--;

		tetrs.push_back(new_tetr);
	}

	infile >> str;
	if(strcmp(str.c_str(),"$EndElements") != 0) {
		if(logger != NULL)
			logger->write(string("ERROR: TetrMesh_1stOrder::load_msh_file - wrong file format. '$EndElements' expected."));
		return -1;
	}

	if(logger != NULL)
		logger->write(string("INFO: TetrMesh_1stOrder::load_msh_file - File read."));

	infile.close();

	// Check if internal numbers of nodes are the same as numbers in array
	// We need it in future to perform quick access to nodes in array
	for(int i = 0; i < nodes.size(); i++)
	{
		if(nodes[i].local_num != i) {
			if(logger != NULL)
				logger->write(string("ERROR: TetrMesh_1stOrder::load_msh_file - Invalid nodes numbering!"));
			return -1;
		}
	}
	for(int i = 0; i < tetrs.size(); i++)
	{
		if(tetrs[i].local_num != i) {
			if(logger != NULL)
				logger->write(string("ERROR: TetrMesh_1stOrder::load_msh_file - Invalid tetrahedron numbering!"));
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

	// TODO - move this part to mesh pre-processing. It is not specific for any file type.
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

float TetrMesh_1stOrder::tetr_h(int i)
{
		float min_h;
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
				logger->write(string("ERROR: TetrMesh_1stOrder::tetr_h - Volume is zero!"));
			return -1;
		}

		for(int j = 0; j < 4; j++)
		{
			if(area[j] == 0) {
				if(logger != NULL)
					logger->write(string("ERROR: TetrMesh_1stOrder::tetr_h - Face area is zero!"));
				return -1;
			}
		}

		min_h = fabs(3*vol/area[0]);

		// Go through all faces of given tetrahedron
		for(int j = 1; j < 4; j++)
		{
			// Find height to this face
			h = fabs(3*vol/area[j]);
			// And check if we should update minimum height
			if(h < min_h) { min_h = h; }
		}

		return min_h;
};

// Finds minimum h over mesh
float TetrMesh_1stOrder::get_min_h()
{
	float min_h = -1;
	float h;
	// Go through tetrahedrons
	for(int i = 0; i < tetrs.size(); i++)
	{
		if ( (nodes[tetrs[i].vert[0]].placement_type == UNUSED) 
			|| (nodes[tetrs[i].vert[1]].placement_type == UNUSED) 
			|| (nodes[tetrs[i].vert[2]].placement_type == UNUSED) 
			|| (nodes[tetrs[i].vert[3]].placement_type == UNUSED) )
			continue;

		// Get current h
		h = tetr_h(i);
		if(min_h < 0)
			min_h = h;
		// If it is negative - return error
		if(h < 0)
			return -1;

		// Otherwise - just find minimum
		if(h < min_h) { min_h = h; }
	}

	return min_h;
};

// Finds maximum h over mesh
float TetrMesh_1stOrder::get_max_h()
{
	float h;
	float max_h = -1;
	// Go through tetrahedrons
	for(int i = 0; i < tetrs.size(); i++)
	{
		if ( (nodes[tetrs[i].vert[0]].placement_type == UNUSED) 
			|| (nodes[tetrs[i].vert[1]].placement_type == UNUSED) 
			|| (nodes[tetrs[i].vert[2]].placement_type == UNUSED) 
			|| (nodes[tetrs[i].vert[3]].placement_type == UNUSED) )
			continue;

		// Get current h
		h = tetr_h(i);
		// If it is negative - return error
		if(h < 0)
			return -1;

		// Otherwise - just find minimum
		if(h > max_h) { max_h = h; }
	}

	return max_h;
};

int TetrMesh_1stOrder::log_quality_stats()
{
	stringstream ss;
	ss.setf(ios::fixed,ios::floatfield);
	ss.precision(10);
	ss.str("");
	float h;

	float max_h;
	float avg_h = 0;

	float hyst[10];
	hyst[0] = hyst[1] = hyst[2] = hyst[3] = hyst[4] = hyst[5] = hyst[6] = hyst[7] = hyst[8] = hyst[9] = 0;

	max_h = get_max_h();

	int num;

	for(int i = 0; i < tetrs.size(); i++)
	{
		if ( (nodes[tetrs[i].vert[0]].placement_type == UNUSED) 
			|| (nodes[tetrs[i].vert[1]].placement_type == UNUSED) 
			|| (nodes[tetrs[i].vert[2]].placement_type == UNUSED) 
			|| (nodes[tetrs[i].vert[3]].placement_type == UNUSED) )
			continue;

		// Get current h
		h = tetr_h(i);
		// If it is negative - return error
		if(h < 0)
			return -1;

		avg_h += h/tetrs.size();

		h = h / max_h;
		num = (int)(h/0.1);
		hyst[num]++;
	}

	ss << "Max H = " << get_max_h() << endl;
	ss << "Min H = " << get_min_h() << endl;
	ss << "Avg H = " << avg_h << endl;
	ss << "Histogramm:" << endl;
	for(int i = 0; i < 10; i++)
		ss << hyst[i] << endl;

	if(logger != NULL)
		logger->write(ss.str());

	return 0;
};

/*bool TetrMesh_1stOrder::point_in_tetr(float x, float y, float z, Tetrahedron_1st_order* tetr)
{
	return point_in_tetr(x, y, z, (Tetrahedron*) tetr);
};

bool TetrMesh_1stOrder::point_in_tetr(float ext_x, float ext_y, float ext_z, Tetrahedron* tetr)
{
	#ifdef DEBUG_MESH_GEOMETRY
	if(logger != NULL) {
		stringstream ss;
		ss.setf(ios::fixed,ios::floatfield);
		ss.precision(10);
		ss << "DEBUG: TetrMesh_1stOrder::point_in_tetr" << endl;
		ss << "Point: x: " << ext_x << " y: " << ext_y << " z: " << ext_z << endl;
		ss << "Tetr: " << tetr->local_num << endl;
		for(int j = 0; j < 4; j++) {
			ElasticNode* tmp_node = get_node( tetr->vert[j] );
			ss << "\t\tVert: " << j << " num: " << tmp_node->local_num << "\t"
				<< " x: " << tmp_node->coords[0]
				<< " y: " << tmp_node->coords[1]
				<< " z: " << tmp_node->coords[2] << endl;
		}
		logger->write(ss.str());
	}
	#endif

	// Center coordinates at point (ext_x, ext_y, ext_z)
	shift_coordinates(tetr, -ext_x, -ext_y, -ext_z);
	float x = 0; float y = 0; float z = 0;

	#ifdef DEBUG_MESH_GEOMETRY
	if(logger != NULL) {
		stringstream ss;
		ss.setf(ios::fixed,ios::floatfield);
		ss.precision(10);
		ss << "Tetr after shift: " << tetr->local_num << endl;
		for(int j = 0; j < 4; j++) {
			ElasticNode* tmp_node = get_node( tetr->vert[j] );
			ss << "\t\tVert: " << j << " num: " << tmp_node->local_num << "\t"
				<< " x: " << tmp_node->coords[0]
				<< " y: " << tmp_node->coords[1]
				<< " z: " << tmp_node->coords[2] << endl;
		}
		logger->write(ss.str());
	}
	#endif

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
	#ifdef DEBUG_MESH_GEOMETRY
	if(logger != NULL) {
		stringstream ss;
		ss.setf(ios::fixed,ios::floatfield);
		ss.precision(10);
		ss << "\t\tStage1: d1: " << d1 << " d2: " << d2 << endl;
		logger->write(ss.str());
	}
	#endif
	if(d1*d2 < 0) { shift_coordinates(tetr, ext_x, ext_y, ext_z); return false; }

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
	#ifdef DEBUG_MESH_GEOMETRY
	if(logger != NULL) {
		stringstream ss;
		ss.setf(ios::fixed,ios::floatfield);
		ss.precision(10);
		ss << "\t\tStage2: d1: " << d1 << " d2: " << d2 << endl;
		logger->write(ss.str());
	}
	#endif
	if(d1*d2 < 0) { shift_coordinates(tetr, ext_x, ext_y, ext_z); return false; }

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
	#ifdef DEBUG_MESH_GEOMETRY
	if(logger != NULL) {
		stringstream ss;
		ss.setf(ios::fixed,ios::floatfield);
		ss.precision(10);
		ss << "\t\tStage3: d1: " << d1 << " d2: " << d2 << endl;
		logger->write(ss.str());
	}
	#endif
	if(d1*d2 < 0) { shift_coordinates(tetr, ext_x, ext_y, ext_z); return false; }

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
	#ifdef DEBUG_MESH_GEOMETRY
	if(logger != NULL) {
		stringstream ss;
		ss.setf(ios::fixed,ios::floatfield);
		ss.precision(10);
		ss << "\t\tStage4: d1: " << d1 << " d2: " << d2 << endl;
		logger->write(ss.str());
	}
	#endif
	if(d1*d2 < 0) { shift_coordinates(tetr, ext_x, ext_y, ext_z); return false; }

	shift_coordinates(tetr, ext_x, ext_y, ext_z);
	return true;
};*/

bool TetrMesh_1stOrder::point_in_tetr(int base_node_index, float dx, float dy, float dz, Tetrahedron_1st_order* tetr)
{
	return point_in_tetr(base_node_index, dx, dy, dz, (Tetrahedron*) tetr);
};

bool TetrMesh_1stOrder::point_in_tetr(int base_node_index, float dx, float dy, float dz, Tetrahedron* tetr)
{
	float d1,d2;

	#ifdef DEBUG_MESH_GEOMETRY
	if(logger != NULL) {
		stringstream ss;
		ss.setf(ios::fixed,ios::floatfield);
		ss.precision(10);
		ss << "DEBUG: TetrMesh_1stOrder::point_in_tetr" << endl;
		ss << "Point: num: " << base_node_index << " dx: " << dx << " dy: " << dy << " dz: " << dz << endl;
		ss << "Tetr: " << tetr->local_num << endl;
		for(int j = 0; j < 4; j++) {
			ElasticNode* tmp_node = get_node( tetr->vert[j] );
			ss << "\t\tVert: " << j << " num: " << tmp_node->local_num << "\t"
				<< " x: " << tmp_node->coords[0]
				<< " y: " << tmp_node->coords[1]
				<< " z: " << tmp_node->coords[2] << endl;
		}
		logger->write(ss.str());
	}
	#endif

	d1 = calc_determ_pure_tetr(tetr->vert[1], tetr->vert[2], tetr->vert[3], tetr->vert[0]);
	d2 = calc_determ_with_shift(tetr->vert[1], tetr->vert[2], tetr->vert[3], base_node_index, dx, dy, dz);
	#ifdef DEBUG_MESH_GEOMETRY
	if(logger != NULL) {
		stringstream ss;
		ss.setf(ios::fixed,ios::floatfield);
		ss.precision(10);
		ss << "\t\tStage1: d1: " << d1 << " d2: " << d2 << endl;
		logger->write(ss.str());
	}
	#endif
	if(d1*d2 < 0) { return false; }

	d1 = calc_determ_pure_tetr(tetr->vert[0], tetr->vert[2], tetr->vert[3], tetr->vert[1]);
	d2 = calc_determ_with_shift(tetr->vert[0], tetr->vert[2], tetr->vert[3], base_node_index, dx, dy, dz);
	#ifdef DEBUG_MESH_GEOMETRY
	if(logger != NULL) {
		stringstream ss;
		ss.setf(ios::fixed,ios::floatfield);
		ss.precision(10);
		ss << "\t\tStage2: d1: " << d1 << " d2: " << d2 << endl;
		logger->write(ss.str());
	}
	#endif
	if(d1*d2 < 0) { return false; }

	d1 = calc_determ_pure_tetr(tetr->vert[0], tetr->vert[1], tetr->vert[3], tetr->vert[2]);
	d2 = calc_determ_with_shift(tetr->vert[0], tetr->vert[1], tetr->vert[3], base_node_index, dx, dy, dz);
	#ifdef DEBUG_MESH_GEOMETRY
	if(logger != NULL) {
		stringstream ss;
		ss.setf(ios::fixed,ios::floatfield);
		ss.precision(10);
		ss << "\t\tStage3: d1: " << d1 << " d2: " << d2 << endl;
		logger->write(ss.str());
	}
	#endif
	if(d1*d2 < 0) { return false; }

	d1 = calc_determ_pure_tetr(tetr->vert[0], tetr->vert[1], tetr->vert[2], tetr->vert[3]);
	d2 = calc_determ_with_shift(tetr->vert[0], tetr->vert[1], tetr->vert[2], base_node_index, dx, dy, dz);
	#ifdef DEBUG_MESH_GEOMETRY
	if(logger != NULL) {
		stringstream ss;
		ss.setf(ios::fixed,ios::floatfield);
		ss.precision(10);
		ss << "\t\tStage4: d1: " << d1 << " d2: " << d2 << endl;
		logger->write(ss.str());
	}
	#endif
	if(d1*d2 < 0) { return false; }

	return true;
};

void TetrMesh_1stOrder::shift_coordinates(Tetrahedron* tetr, float ext_x, float ext_y, float ext_z)
{
	for(int i = 0; i < 4; i++) {
		nodes[ tetr->vert[i] ].coords[0] += ext_x;
		nodes[ tetr->vert[i] ].coords[1] += ext_y;
		nodes[ tetr->vert[i] ].coords[2] += ext_z;
	}
};

Tetrahedron_1st_order* TetrMesh_1stOrder::find_owner_tetr(int base_node, float dx, float dy, float dz, ElasticNode* node)
{
	float x = nodes[base_node].coords[0] + dx;
	float y = nodes[base_node].coords[1] + dy;
	float z = nodes[base_node].coords[2] + dz;

// We used this implementation when tau was min_h/max_L
// In that case if not found in adjacent tetrs - not found at all and out of body
/*	// Checking adjacent tetrahedrons
	for(int i = 0; i < (node->elements)->size(); i++)
	{
		if( point_in_tetr(x, y, z, &tetrs[(node->elements)->at(i)]) )
		{
			return &tetrs[(node->elements)->at(i)];
		}
	}

	return NULL;
*/
//	mesh_stats.find_owner_tetr_long_searches++;
//	mesh_stats.find_owner_tetr_quick_searches++;


	// A square of distance between point in question and local node
	// Will be used to check if it is worth to continue search or point in question is out of body
	float R2 = (node->coords[0] - x) * (node->coords[0] - x) + (node->coords[1] - y) * (node->coords[1] - y)
			+ (node->coords[2] - z) * (node->coords[2] - z);

	#ifdef DEBUG_MESH_GEOMETRY
	if(logger != NULL) {
		stringstream ss;
		ss.setf(ios::fixed,ios::floatfield);
		ss.precision(10);
		ss << "DEBUG: TetrMesh_1stOrder::find_owner_tetr" << endl;
		ss << "\t\tR2: " << R2 << endl;
		logger->write(ss.str());
	}
	#endif

	//TODO May be std::set will be better? It guarantees unique elements.
	vector<int> checked; // Already checked tetrahedrons
	vector<int> checking; // Tetrs we are checking just now
	vector<int> tmp_to_check; // tmp list, used to construct new 'checking' list

	checked.clear();
	checking.clear();
	tmp_to_check.clear();

	// If current tetrs are inside sphere of radius R or not. If not - we should stop search and return NULL
	bool inside_R = true;

	float local_x, local_y, local_z;

	for(int i = 0; i < (node->elements)->size(); i++)
		checking.push_back((node->elements)->at(i));

	while(inside_R)
	{
		inside_R = false;

		// Check current tetrs
		for(int i = 0; i < checking.size(); i++)
		{
			// If found - return result
			if( point_in_tetr(base_node, dx, dy, dz, &tetrs[checking[i]]) ) {
				#ifdef DEBUG_MESH_GEOMETRY
				if(logger != NULL) {
					stringstream ss;
					ss << "\tFound in tetr: " << checking[i] << endl;
					logger->write(ss.str());
				}
				#endif

				return &tetrs[checking[i]];
			}

			// Check if this tetr is still inside sphere of radius R
			// If we have already found at least one tetr in the sphere - skip check
			if(!inside_R)
			{
				// For all verticles of current tetr
				for(int j = 0; j < 4; j++)
				{
					local_x = nodes[ tetrs[checking[i]].vert[j] ].coords[0];
					local_y = nodes[ tetrs[checking[i]].vert[j] ].coords[1];
					local_z = nodes[ tetrs[checking[i]].vert[j] ].coords[2];
					// If its distance smaller than R
					if( ( (node->coords[0] - local_x) * (node->coords[0] - local_x) 
						+ (node->coords[1] - local_y) * (node->coords[1] - local_y) 
						+ (node->coords[2] - local_z) * (node->coords[2] - local_z) ) < R2 ) {
							inside_R = true;
					}
					// TODO FIXME In theory we whould check if sphere and tetr intersect.
					// Current check - if at least one vert is in sphere.
					// It can be wrong on turbo bad tetrs. It fails if
					// sphere goes 'through' tetr to next layer but does not include its verticles.
				}
			}
		}

		// If current layer is not in sphere - there is no need to create next layer - just return NULL
		if(!inside_R)
			return NULL;

		// If not found in current tetrs - create new lists

		// First - add checked tetrs to the 'checked' list
		for(int i = 0; i < checking.size(); i++)
			checked.push_back(checking[i]);

		// Create very long list of all tetrs adjacent to any tetr in 'checking' list
		// (with duplicates and tetrs already checked)
		tmp_to_check.clear();
		for(int i = 0; i < checking.size(); i++)
			for(int j = 0; j < 4; j++)
				for(int k = 0; k < nodes[ tetrs[checking[i]].vert[j] ].elements->size(); k++)
					tmp_to_check.push_back(nodes[ tetrs[checking[i]].vert[j] ].elements->at(k));

		// Remove duplicates
		sort( tmp_to_check.begin(), tmp_to_check.end() );
		tmp_to_check.erase( unique( tmp_to_check.begin(), tmp_to_check.end() ), tmp_to_check.end() );

		// Copy to 'checking' and removing elements that are already checked
		checking.clear();
		bool is_checked;
		for(int i = 0; i < tmp_to_check.size(); i++)
		{
			is_checked = false;
			for(int j = 0; j < checked.size(); j++)
				if(tmp_to_check[i] == checked[j])
					is_checked = true;
			if(!is_checked)
				checking.push_back(tmp_to_check[i]);
		}
	}

	return NULL;

};

int TetrMesh_1stOrder::interpolate(ElasticNode* node, Tetrahedron* tetr)
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

	// If we see potential instability
	if(factor[0] + factor[1] + factor[2] + factor[3] > 1.0)
	{
		// If it is small - treat instability as minor and just 'smooth' it
		// TODO - think about it more carefully
		//if( point_in_tetr(node->local_num, node->coords[0], node->coords[1], node->coords[2], tetr) )
		if(factor[0] + factor[1] + factor[2] + factor[3] < 1.01)
		{
			float sum = factor[0] + factor[1] + factor[2] + factor[3];
			for(int i = 0; i < 4; i++)
				factor[i] = factor[i] * 0.995 / sum;
		}
		// If point is not in tetr - throw error
		else
		{
			if(logger != NULL) {
				logger->write(string("ERROR: TetrMesh_1stOrder::interpolate - Sum of factors is greater than 1.0!"));
	
				stringstream ss;
				ss.setf(ios::fixed,ios::floatfield);
				ss.precision(10);
				ss << "\tfactor[0]=" << factor[0] << " factor[1]=" << factor[1] << " factor[2]=" << factor[2] 
					<< " factor[3]=" << factor[3];
				logger->write(ss.str());
	
				ss.str("");
				ss << "\tnode.x[0]=" << node->coords[0] << " node.x[1]=" << node->coords[1] 
					<< " node.x[2]=" << node->coords[2];
	                        logger->write(ss.str());
	
				ss.str("");
	                        ss << "\tv0.x[0]=" << nodes[tetr->vert[0]].coords[0] << " v0.x[1]=" << nodes[tetr->vert[0]].coords[1]
        	                        << " v0.x[2]=" << nodes[tetr->vert[0]].coords[2];
                	        logger->write(ss.str());
	
        	                ss.str("");
	                        ss << "\tv1.x[0]=" << nodes[tetr->vert[1]].coords[0] << " v1.x[1]=" << nodes[tetr->vert[1]].coords[1]
        	                        << " v1.x[2]=" << nodes[tetr->vert[1]].coords[2];
                	        logger->write(ss.str());
	
				ss.str("");
                	        ss << "\tv2.x[0]=" << nodes[tetr->vert[2]].coords[0] << " v2.x[1]=" << nodes[tetr->vert[2]].coords[1]
                        	        << " v2.x[2]=" << nodes[tetr->vert[2]].coords[2];
	                        logger->write(ss.str());

				ss.str("");
                	        ss << "\tv3.x[0]=" << nodes[tetr->vert[3]].coords[0] << " v3.x[1]=" << nodes[tetr->vert[3]].coords[1]
                        	        << " v3.x[2]=" << nodes[tetr->vert[3]].coords[2];
	                        logger->write(ss.str());
			}
			return -1;
		}
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

float TetrMesh_1stOrder::get_max_possible_tau()
{
//cout << "GMPT1: " << endl;
	float min_h = get_min_h();
	float max_l = 0;
	float l;

//cout << "GMPT2: " << endl;
	for(int i = 0; i < nodes.size(); i++)
	{
		if(nodes[i].placement_type == LOCAL)
		{
//cout << "GMPT3.1: " << endl;
			l = method->get_max_lambda(&nodes[i], this);
//cout << "GMPT3.2: " << endl;
			if(l < 0) {
				if(logger != NULL)
					logger->write(string("ERROR: TetrMesh_1stOrder::get_max_possible_tau - got error from method on method->get_max_lambda"));
				return -1;
			}
			if(l > max_l) { max_l = l; }
		}
	}

//cout << "GMPT4: " << endl;
	return min_h/max_l;
};

int TetrMesh_1stOrder::do_next_part_step(float tau, int stage)
{
	for(int i = 0; i < nodes.size(); i++)
	{
		if(nodes[i].placement_type == LOCAL)
		{
			if (method->do_next_part_step(&nodes[i], &new_nodes[i], tau, stage, this) < 0) {
				if(logger != NULL)
					logger->write(string("ERROR: TetrMesh_1stOrder::do_next_part_step - got error from method on method->do_next_part_step"));
				return -1;
			}
			// TODO Add details
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
	return 0;
};

void TetrMesh_1stOrder::move_coords(float tau)
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

int TetrMesh_1stOrder::proceed_rheology()
{
	if(rheology == NULL) {
		if(logger != NULL)
			logger->write(string("ERROR: TetrMesh_1stOrder::step - can not do step without RheologyCalculator attached"));
		return -1;
	}
	// TODO if rheology is void we can skip this step
	for(int i = 0; i < nodes.size(); i++)
	{
		if(nodes[i].placement_type == LOCAL)
		{
			rheology->do_calc(&nodes[i], &nodes[i]);
		}
	}
	return 0;
};

int TetrMesh_1stOrder::set_stress(float tau)
{
	if(stresser == NULL) {
		if(logger != NULL)
			logger->write(string("ERROR: TetrMesh_1stOrder::set_stress - can not do step without Stresser attached"));
		return -1;
	}
	// TODO if stress has ended at all we can skip this step
	for(int i = 0; i < nodes.size(); i++)
	{
		if(nodes[i].placement_type == LOCAL)
		{
			stresser->set_current_stress(&nodes[i], &nodes[i], tau);
		}
	}
	return 0;
};

int TetrMesh_1stOrder::do_next_step()
{
	int number_of_stages;

	if(method == NULL){
		if(logger != NULL)
			logger->write(string("ERROR: TetrMesh_1stOrder::do_next_step - can not do step without NumericalMethod attached"));
		return -1;
	}
	if(rheology == NULL) {
		if(logger != NULL)
			logger->write(string("ERROR: TetrMesh_1stOrder::do_next_step - can not do step without RheologyCalculator attached"));
		return -1;
	}

	float time_step = get_max_possible_tau();
//cout << "D1: " << time_step << endl;
	// FIXME
	// time_step = time_step * 2;
	if(time_step < 0) {
		if(logger != NULL)
			logger->write(string("ERROR: TetrMesh_1stOrder::do_next_step - error on determining time step!"));
		return -1;
	}

	if(data_bus != NULL)
		time_step = data_bus->get_max_possible_tau(time_step);

	if( (number_of_stages = method->get_number_of_stages()) <= 0 )
	{
		if(logger != NULL)
			logger->write(string("ERROR: TetrMesh_1stOrder::do_next_step - can not do step, number of stages incorrect!"));
		return -1;
	}

	if(set_stress(current_time) < 0) {
		if(logger != NULL)
			logger->write(string("ERROR: TetrMesh_1stOrder::do_next_step - set_stress failed!"));
		return -1;
	}

//cout << "D2: " << endl;
	for(int i = 0; i < number_of_stages; i++)
	{
		if(data_bus != NULL)
			data_bus->sync_nodes(); // TODO Add error handling

		// TODO Add interaction with scheduler

		if(do_next_part_step(time_step, i) < 0) {
			if(logger != NULL)
				logger->write(string("ERROR: TetrMesh_1stOrder::do_next_step - do_next_part_step failed!"));
			return -1;
		}
//cout << "D3: part " << i << endl;
	}

//	move_coords(time_step); // FIXME temporary disable move

	if(proceed_rheology() < 0) {
		if(logger != NULL)
			logger->write(string("ERROR: TetrMesh_1stOrder::do_next_step - proceed_rheology failed!"));
		return -1;
	}

	current_time += time_step;

	return 0;
};

Tetrahedron* TetrMesh_1stOrder::get_tetrahedron(int index)
{
	return &tetrs[index];
};

ElasticNode* TetrMesh_1stOrder::get_node(int index)
{
	return &nodes[index];
};

float TetrMesh_1stOrder::calc_determ_pure_tetr(int node1, int node2, int node3, int ref_node)
{
	return qm_engine.determinant(
				nodes[node1].coords[0] - nodes[ref_node].coords[0],
				nodes[node1].coords[1] - nodes[ref_node].coords[1],
				nodes[node1].coords[2] - nodes[ref_node].coords[2],
				nodes[node2].coords[0] - nodes[ref_node].coords[0],
				nodes[node2].coords[1] - nodes[ref_node].coords[1],
				nodes[node2].coords[2] - nodes[ref_node].coords[2],
				nodes[node3].coords[0] - nodes[ref_node].coords[0],
				nodes[node3].coords[1] - nodes[ref_node].coords[1],
				nodes[node3].coords[2] - nodes[ref_node].coords[2] );
};

float TetrMesh_1stOrder::calc_determ_with_shift(int node1, int node2, int node3, int base_node, float dx, float dy, float dz)
{
	float x = nodes[base_node].coords[0] + dx;
	float y = nodes[base_node].coords[1] + dy;
	float z = nodes[base_node].coords[2] + dz;

	if(node1 == base_node) {
		return qm_engine.determinant(
				- dx,				- dy,				- dz,
				nodes[node2].coords[0] - x,	nodes[node2].coords[1] - y,	nodes[node2].coords[2] - z,
				nodes[node3].coords[0] - x,	nodes[node3].coords[1] - y,	nodes[node3].coords[2] - z );

	} else if (node2 == base_node) {
		return qm_engine.determinant(
				nodes[node1].coords[0] - x,	nodes[node1].coords[1] - y,	nodes[node1].coords[2] - z,
				- dx,				- dy,				- dz,
				nodes[node3].coords[0] - x,	nodes[node3].coords[1] - y,	nodes[node3].coords[2] - z );

	} else if (node3 == base_node) {
		return qm_engine.determinant(
				nodes[node1].coords[0] - x,	nodes[node1].coords[1] - y,	nodes[node1].coords[2] - z,
				nodes[node2].coords[0] - x,	nodes[node2].coords[1] - y,	nodes[node2].coords[2] - z,
				- dx,				- dy,				- dz			   );

	} else {
		return qm_engine.determinant(
				nodes[node1].coords[0] - x,	nodes[node1].coords[1] - y,	nodes[node1].coords[2] - z,
				nodes[node2].coords[0] - x,	nodes[node2].coords[1] - y,	nodes[node2].coords[2] - z,
				nodes[node3].coords[0] - x,	nodes[node3].coords[1] - y,	nodes[node3].coords[2] - z );
	}

};

