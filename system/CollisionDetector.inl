CollisionDetector::CollisionDetector()
{
	logger = NULL;
};

CollisionDetector::~CollisionDetector() { };

void CollisionDetector::attach(Logger* new_logger)
{
	logger = new_logger;
};

void CollisionDetector::set_treshold(float value)
{
	treshold = value;
};

int CollisionDetector::find_collisions(TetrMesh_1stOrder* mesh1, TetrMesh_1stOrder* mesh2, vector<ElasticNode>* virt_nodes)
{
	mesh1_nodes.clear();
	mesh2_nodes.clear();
	mesh1_tetrs.clear();
	mesh2_tetrs.clear();

	MeshOutline outline1 = mesh1->outline;
	MeshOutline outline2 = mesh2->outline;

	stringstream ss;
        ss << "Mesh outline 1:" << endl
                << "MinX: " << outline1.min_coords[0] << endl
                << "MaxX: " << outline1.max_coords[0] << endl
                << "MinY: " << outline1.min_coords[1] << endl
                << "MaxY: " << outline1.max_coords[1] << endl
                << "MinZ: " << outline1.min_coords[2] << endl
                << "MaxZ: " << outline1.max_coords[2] << endl;

       ss << "Mesh outline 2:" << endl
                << "MinX: " << outline2.min_coords[0] << endl
                << "MaxX: " << outline2.max_coords[0] << endl
                << "MinY: " << outline2.min_coords[1] << endl
                << "MaxY: " << outline2.max_coords[1] << endl
                << "MinZ: " << outline2.min_coords[2] << endl
                << "MaxZ: " << outline2.max_coords[2] << endl;

	cout << ss.str();

	MeshOutline intersect;

	bool colliding = true;
	for(int j = 0; j < 3; j++) {
		intersect.min_coords[j] = fmaxf(outline1.min_coords[j] - treshold, outline2.min_coords[j] - treshold);
		intersect.max_coords[j] = fminf(outline1.max_coords[j] + treshold, outline2.max_coords[j] + treshold);
		if(intersect.min_coords[j] > intersect.max_coords[j])
			colliding = false;
	}

	if(!colliding) {
		cout << "Not colliding\n";
		return 0;
	}

	cout << "Colliding!\n";
        cout << "Intersection:" << endl
	                << "MinX: " << intersect.min_coords[0] << endl
	                << "MaxX: " << intersect.max_coords[0] << endl
	                << "MinY: " << intersect.min_coords[1] << endl
	                << "MaxY: " << intersect.max_coords[1] << endl
	                << "MinZ: " << intersect.min_coords[2] << endl
	                << "MaxZ: " << intersect.max_coords[2] << endl;

	// TODO - unreadable code - rewrite it based on get_* functions
	for(int i = 0; i < (mesh1->nodes).size(); i++) {
		(mesh1->nodes).at(i).contact_type = FREE;
		if(node_in_intersection( &((mesh1->nodes).at(i)), &intersect ) ) {
			mesh1_nodes.push_back(i);
			for(int j = 0; j < (((mesh1->nodes).at(i)).elements)->size(); j++)
				mesh1_tetrs.push_back( (((mesh1->nodes).at(i)).elements)->at(j) );
		}
	}

	for(int i = 0; i < (mesh2->nodes).size(); i++) {
		(mesh2->nodes).at(i).contact_type = FREE;
		if(node_in_intersection( &((mesh2->nodes).at(i)), &intersect ) ) {
			mesh2_nodes.push_back(i);
			for(int j = 0; j < (((mesh2->nodes).at(i)).elements)->size(); j++)
				mesh2_tetrs.push_back( (((mesh2->nodes).at(i)).elements)->at(j) );
		}
	}

	ElasticNode node;
	float dx[3];

	for(int i = 0; i < mesh1_nodes.size(); i++) {
		node = (mesh1->nodes).at( mesh1_nodes[i] );

		mesh1->find_border_node_normal(mesh1_nodes[i], &dx[0], &dx[1], &dx[2]);
		node.coords[0] += dx[0] * treshold * 2; // FIXME - remove hardcoded value '2'
		node.coords[1] += dx[1] * treshold * 2;
		node.coords[2] += dx[2] * treshold * 2;

		for(int j = 0; j < mesh2_tetrs.size(); j++) {
			if( mesh2->point_in_tetr(node.coords[0], node.coords[1], node.coords[2], &((mesh2->tetrs).at( mesh2_tetrs[j] )) ) ) {
				(mesh1->nodes).at( mesh1_nodes[i] ).contact_type = IN_CONTACT;
				break;
			}
		}
	}

        for(int i = 0; i < mesh2_nodes.size(); i++) {
                node = (mesh2->nodes).at( mesh2_nodes[i] );
                
                mesh2->find_border_node_normal(mesh2_nodes[i], &dx[0], &dx[1], &dx[2]);
                node.coords[0] += dx[0] * treshold * 2; // FIXME - remove hardcoded value '2'
                node.coords[1] += dx[1] * treshold * 2;
                node.coords[2] += dx[2] * treshold * 2;

                for(int j = 0; j < mesh1_tetrs.size(); j++) {
                        if( mesh1->point_in_tetr(node.coords[0], node.coords[1], node.coords[2], &((mesh1->tetrs).at( mesh1_tetrs[j] )) ) ) {
                                (mesh2->nodes).at( mesh2_nodes[i] ).contact_type = IN_CONTACT;
                                break;
                        }
                }
        }

	return 0;
};

bool CollisionDetector::node_in_intersection(ElasticNode* node, MeshOutline* intersect)
{
	for(int j = 0; j < 3; j++)
		if( (node->coords[j] < intersect->min_coords[j]) || (node->coords[j] > intersect->max_coords[j]) )
			return false;
	return true;
};
