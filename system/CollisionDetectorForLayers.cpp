#include "CollisionDetectorForLayers.h"

CollisionDetectorForLayers::CollisionDetectorForLayers()
{
	logger = NULL;
	index_shift = -1;
	shift_direction = -1;
};

CollisionDetectorForLayers::~CollisionDetectorForLayers() { };

void CollisionDetectorForLayers::attach(Logger* new_logger)
{
	logger = new_logger;
};

void CollisionDetectorForLayers::set_treshold(float value)
{
	treshold = value;
};

int CollisionDetectorForLayers::find_collisions(TetrMesh_1stOrder* mesh1, TetrMesh_1stOrder* mesh2, vector<ElasticNode>* virt_nodes, float time_step)
{
	mesh1_nodes.clear();
	mesh2_nodes.clear();

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

	// If it is the first run - process mesh once and find and remember index_shift
	if(index_shift < 0 )
		find_index_shift(mesh1, mesh2);

	// Find nodes in intersection to check them in details later
	find_nodes_in_intersect(mesh1, &intersect, &mesh1_nodes);
	find_nodes_in_intersect(mesh2, &intersect, &mesh2_nodes);

	process_mesh(&mesh1_nodes, mesh1, mesh2, virt_nodes);
	process_mesh(&mesh2_nodes, mesh2, mesh1, virt_nodes);

	return 0;
};

void CollisionDetectorForLayers::find_index_shift(TetrMesh_1stOrder* mesh1, TetrMesh_1stOrder* mesh2)
{
	float h = mesh1->get_min_h() / 2;
	MeshOutline outline1 = mesh1->outline;
	MeshOutline outline2 = mesh2->outline;

	for(int i = 0; i < 3; i++)
		if( fabs(outline1.min_coords[i] - outline2.min_coords[i]) > h )
			shift_direction = i;

	if(shift_direction < 0)
		throw GCMException( GCMException::COLLISION_EXCEPTION, "Can not determine shift direction");

	float shift_value = fabs( outline1.min_coords[shift_direction] - outline2.min_coords[shift_direction] );
	if( fabs( outline1.max_coords[shift_direction] - outline1.min_coords[shift_direction] - shift_value ) > h )
		throw GCMException( GCMException::COLLISION_EXCEPTION, "Shift value does not match layer width");

	int base_index = -1;

	for(int i = 0; i < (mesh1->nodes).size(); i++) {
		if( fabs( (mesh1->nodes).at(i).coords[shift_direction] - outline1.min_coords[shift_direction] ) < h ) {
			base_index = i;
			break;
		}
	}

	if(base_index < 0)
		throw GCMException( GCMException::COLLISION_EXCEPTION, "Can not determine base index");

	for(int i = 0; i < (mesh1->nodes).size(); i++) {
		bool is_base_pair = true;
		for(int j = 0; j < 3; j++) {
			if( ( j != shift_direction ) 
					&& ( fabs( (mesh1->nodes).at(i).coords[j] 
						- (mesh1->nodes).at(base_index).coords[j] ) > h ) )
				is_base_pair = false;
			if( ( j == shift_direction )
					&& ( fabs( fabs( (mesh1->nodes).at(i).coords[j] 
						- (mesh1->nodes).at(base_index).coords[j] ) - shift_value ) > h ) )
				is_base_pair = false;
		}
		if( is_base_pair ) {
			index_shift = i - base_index;
			break;
		}
	}

	if(index_shift < 0)
		throw GCMException( GCMException::COLLISION_EXCEPTION, "Can not determine index shift");

	for(int i = 0; i < (mesh1->nodes).size(); i++) {
		if( fabs( (mesh1->nodes).at(i).coords[shift_direction] - outline1.min_coords[shift_direction] ) < h )
		{
			for(int j = 0; j < 3; j++) {
				if( j != shift_direction ) {
					if( fabs( (mesh1->nodes).at(i).coords[j] 
						- (mesh1->nodes).at(i+index_shift).coords[j] ) > h )
						throw GCMException( GCMException::COLLISION_EXCEPTION, "Bad index shift");
				} else {
					if( fabs( fabs( (mesh1->nodes).at(i).coords[j] 
						- (mesh1->nodes).at(i+index_shift).coords[j] ) - shift_value ) > h )
						throw GCMException( GCMException::COLLISION_EXCEPTION, "Bad index shift");
				}
			}
		}
	}

	cout << "DEBUG: Treshold " << treshold << endl;
	cout << "DEBUG: Layer shift direction " << shift_direction << endl;
	cout << "DEBUG: Layer shift value " << shift_value << endl;
	cout << "DEBUG: Resulting node index delta " << index_shift << endl;
};

void CollisionDetectorForLayers::find_nodes_in_intersect(TetrMesh_1stOrder* mesh, MeshOutline* intersect, vector<int>* nodes_vector)
{
	for(int i = 0; i < (mesh->nodes).size(); i++) {
		(mesh->nodes).at(i).contact_type = FREE;
		if(node_in_intersection( &((mesh->nodes).at(i)), intersect ) ) {
			nodes_vector->push_back(i);
		}
	}
};

bool CollisionDetectorForLayers::node_in_intersection(ElasticNode* node, MeshOutline* intersect)
{
	for(int j = 0; j < 3; j++)
		if( (node->coords[j] < intersect->min_coords[j]) || (node->coords[j] > intersect->max_coords[j]) )
			return false;
	return true;
};

void CollisionDetectorForLayers::process_mesh(vector<int>* nodes_vector, TetrMesh_1stOrder* current_mesh, TetrMesh_1stOrder* other_mesh, vector<ElasticNode>* virt_nodes)
{
	ElasticNode* node;
	float h = current_mesh->get_min_h() / 2;
	MeshOutline outline = current_mesh->outline;

	for(int i = 0; i < nodes_vector->size(); i++)
	{
		node = &(current_mesh->nodes).at( nodes_vector->at(i) );
		if( node->border_type == BORDER )
		{
			current_mesh->clear_contact_data( node );

			bool node_on_edge = false;
			for(int j = 0; j < 3; j++)
				if( ( j != shift_direction ) && ( ( fabs( node->coords[j] - outline.min_coords[j] ) < h ) 
							|| ( fabs( node->coords[j] - outline.max_coords[j] ) < h ) ) )
					node_on_edge = true;

			if( !node_on_edge )
			{
				if( fabs( node->coords[shift_direction] - outline.min_coords[shift_direction] ) < h )
				{
					node->contact_type = IN_CONTACT;
					// We always set 'axis_plus[0]' (!!!) because the first axis goes along _outer_ normal
					node->contact_data->axis_plus[0] = virt_nodes->size();
					virt_nodes->push_back( (other_mesh->nodes).at( nodes_vector->at(i) + index_shift ) );
				}
				else if( fabs( node->coords[shift_direction] - outline.max_coords[shift_direction] ) < h )
				{
					node->contact_type = IN_CONTACT;
					node->contact_data->axis_plus[0] = virt_nodes->size();
					virt_nodes->push_back( (other_mesh->nodes).at( nodes_vector->at(i) - index_shift ) );
				}
			}
		}
	}

};

