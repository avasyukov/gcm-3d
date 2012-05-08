#include "DataBus.h"

int DataBus::TAG_CLASS_SYNC_FACES[] = {
	DataBus::TAG_SYNC_FACES_REQ,
	DataBus::TAG_SYNC_FACES_F_RESP,
	DataBus::TAG_SYNC_FACES_N_RESP,
	DataBus::TAG_SYNC_FACES_R_END,
	DataBus::TAG_SYNC_FACES_DONE,
	-1
};

int DataBus::TAG_CLASS_SYNC_TETRS[] = {
	DataBus::TAG_SYNC_TETRS_REQ,
	DataBus::TAG_SYNC_TETRS_N_RESP,
	DataBus::TAG_SYNC_TETRS_T_RESP,
	DataBus::TAG_SYNC_TETRS_DONE,
	DataBus::TAG_SYNC_TETRS_R_END,
	-1
};

DataBus::DataBus()
{
	logger = new Logger();
	DataBus(logger);
};

DataBus::DataBus(Logger* new_logger)
{
	data_bus_type.assign("Basic MPI DataBus");
	attach(new_logger);
	// try to initialize MPI
	// TODO: add command line arguments processing if we really need it
	MPI::Init();
	// register error handler that throws exceptions
	// FIXME
	// fedora's MPI seems to be compiled without  --enable-cxx-exceptions,
	// so we cannot use this error handler
	// MPI::COMM_WORLD.Set_errhandler(MPI::ERRORS_THROW_EXCEPTIONS);
	// MPI initialized, get processor number ant total number of processors
	proc_num = MPI::COMM_WORLD.Get_rank();
	procs_total_num = MPI::COMM_WORLD.Get_size();
	logger->set_proc_num(proc_num);
	*logger < "MPI initialized";
};

DataBus::~DataBus()
{
	// try to finilize MPI
	MPI::Finalize();
	*logger < "MPI finalized";
};

string* DataBus::get_data_bus_type()
{
	return &data_bus_type;
};

void DataBus::attach(Logger* new_logger)
{
	logger = new_logger;
};

void DataBus::attach(TetrMeshSet* new_mesh_set)
{
	mesh_set = new_mesh_set;
};

void DataBus::attach(CollisionDetector* cd)
{
	collision_detector = cd;
};

float DataBus::get_max_possible_tau(float local_time_step)
{
	float max_tau;     
	MPI::COMM_WORLD.Allreduce(&local_time_step, &max_tau, 1, MPI::FLOAT, MPI::MIN);
	*logger << "Time step synchronized, value is: " < max_tau; 

	return max_tau;
}

int DataBus::sync_nodes()
{
	MPI::COMM_WORLD.Barrier();
	*logger < "Starting nodes sync";
	for (int i = 0; i < mesh_set->get_number_of_local_meshes(); i++)
	{
		TetrMesh_1stOrder* mesh = mesh_set->get_local_mesh(i);
		for (int j = 0; j < mesh->nodes.size(); j++)
			if (mesh->nodes[j].placement_type == REMOTE)
			{
				int dest = get_proc_for_zone(mesh->nodes[j].remote_zone_num);
				// avoid sending self-requests
				if (dest == proc_num)
				{
					ElasticNode *node= mesh_set->get_mesh_by_zone_num(mesh->nodes[j].remote_zone_num)->get_node(mesh->nodes[j].remote_num);
					memcpy(mesh->nodes[j].values, node->values, 9*sizeof(float));
					memcpy(mesh->nodes[j].coords, node->coords, 3*sizeof(float));
				}
			}
	}
	
	for (int i = 0; i < zones_info.size(); i++)
		if (get_proc_for_zone(i) != proc_num)
			for (int j = 0; j < zones_info.size(); j++)
				if (local_numbers[i][j].size())
				{
					*logger << "Sending nodes from zone " << j << " to zone " < i;
					MPI::COMM_WORLD.Isend(
						&mesh_set->get_mesh_by_zone_num(j)->nodes[0],
						1,
						MPI_NODE_TYPES[i][j],
						get_proc_for_zone(i),
						TAG_SYNC_NODE+i^j
					);
				}
	
	for (int i = 0; i < zones_info.size(); i++)
		if (get_proc_for_zone(i) == proc_num)
			for (int j = 0; j < zones_info.size(); j++)
				if (local_numbers[i][j].size())
				{
					*logger << "Receiving nodes from zone " << j << " to zone " < i;
					MPI::COMM_WORLD.Recv(
						&mesh_set->get_mesh_by_zone_num(i)->nodes[0],
						1,
						MPI_NODE_TYPES[i][j],
						get_proc_for_zone(j),
						TAG_SYNC_NODE+i^j
					);
				}

	*logger < "Nodes sync done";

	// FIXME
	// do we really need to return a value?
	return 0;
};

void DataBus::load_zones_info(vector<int>* map)
{
	zones_info.clear();
	for(int i = 0; i < map->size(); i++)
		zones_info.push_back( map->at(i) );
}

int DataBus::get_proc_num()
{
	return proc_num;
}

int DataBus::get_procs_total_num()
{
	return procs_total_num;
}

int DataBus::get_proc_for_zone(int zone_num)
{
	return zones_info[zone_num];
}

void DataBus::sync_outlines()
{
	// calculate displacements
	int *displ = (int*)malloc(sizeof(int)*procs_total_num);
	displ[0] = 0;
	for (int i = 1; i < procs_total_num; i++)
		displ[i] = displ[i-1]+ mesh_set->meshes_at_proc[i-1];
	*logger < "Syncing outlines";
	MPI::COMM_WORLD.Allgatherv(mesh_set->get_local_mesh(0), mesh_set->meshes_at_proc[proc_num], MPI_OUTLINE, mesh_set->get_mesh(0), mesh_set->meshes_at_proc, displ, MPI_OUTLINE);
	*logger < "Outlines synced";
	free(displ);
}

bool DataBus::process_faces_sync_message(int source, int tag, vector<ElasticNode> &remote_nodes, vector<Triangle> &remote_faces, int &procs_to_sync)
{
	MPI::Status status;
	MPIFacesFResponse faces_f_resp;
	MPIFacesNResponse faces_n_resp;
	MPIFacesRequest req;
	ElasticNode node;
	Triangle face;
	vector<int> local_nodes;
	vector<Triangle> local_faces;
	MeshOutline outl;

	// process message
	switch (tag)
	{
		case TAG_SYNC_FACES_DONE:
			// just extract message
			MPI::COMM_WORLD.Recv(NULL, 0, MPI::BYTE, source, tag);
			procs_to_sync--;
			break;
		case TAG_SYNC_FACES_R_END:
			// extract message and decrease a counter
			MPI::COMM_WORLD.Recv(NULL, 0, MPI::BYTE, source, tag);
			return true;
		case TAG_SYNC_FACES_F_RESP:
			// add face to array
			MPI::COMM_WORLD.Recv(&faces_f_resp, 1, MPI_FACES_F_RESP, source, tag);
			memcpy(face.vert, faces_f_resp.verts, 3*sizeof(int));
			face.local_num = faces_f_resp.num;
			remote_faces.push_back(face);
			break;
		case TAG_SYNC_FACES_N_RESP:
			// add node to array
			MPI::COMM_WORLD.Recv(&faces_n_resp, 1, MPI_FACES_N_RESP, source, tag);
			memcpy(node.coords, faces_n_resp.coords, 3*sizeof(float));
			memcpy(node.values, faces_n_resp.values, 9*sizeof(float));
			node.local_num = faces_n_resp.num;
			node.remote_zone_num = faces_n_resp.zone_num;
			node.la = faces_n_resp.la;
			node.mu = faces_n_resp.mu;
			node.rho = faces_n_resp.rho;
			remote_nodes.push_back(node);
			break;
		case TAG_SYNC_FACES_REQ:
			// get request
			MPI::COMM_WORLD.Recv(&req, 1, MPI_FACES_REQ, source, tag);
			memcpy(outl.min_coords, req.min_coords, 3*sizeof(float));
			memcpy(outl.max_coords, req.max_coords, 3*sizeof(float));
			// DEBUG
			*logger < "Got FACES_REQ";
			for (int t = 0; t < 3; t++)
				*logger << outl.min_coords[t] << "   " < outl.max_coords[t];
			// get faces in intersection
			collision_detector->find_faces_in_intersection(mesh_set->get_mesh_by_zone_num(req.zone_num)->border, mesh_set->get_mesh_by_zone_num(req.zone_num)->nodes, outl, local_faces);
			// send found faces back with corresponding nodes
			for (int i = 0; i < local_faces.size(); i++)
			{
				// FIXME
				// optimize nodes lookup
				for (int k = 0; k < 3; k++)
				{
					bool flag = true;
					for (int j = 0; j < local_nodes.size(); j++)
						if (local_nodes[j] == local_faces[i].vert[k])
						{
							flag = false;
							break;
						}
					// send node
					if (flag)
					{
						local_nodes.push_back(local_faces[i].vert[k]);
						memcpy(faces_n_resp.coords, mesh_set->get_mesh_by_zone_num(req.zone_num)->nodes[local_faces[i].vert[k]].coords, 3*sizeof(float));
						memcpy(faces_n_resp.values, mesh_set->get_mesh_by_zone_num(req.zone_num)->nodes[local_faces[i].vert[k]].values, 9*sizeof(float));
						faces_n_resp.num = mesh_set->get_mesh_by_zone_num(req.zone_num)->nodes[local_faces[i].vert[k]].local_num;
						faces_n_resp.zone_num = req.zone_num;
						faces_n_resp.la = mesh_set->get_mesh_by_zone_num(req.zone_num)->nodes[local_faces[i].vert[k]].la;
						faces_n_resp.mu = mesh_set->get_mesh_by_zone_num(req.zone_num)->nodes[local_faces[i].vert[k]].mu;
						faces_n_resp.rho = mesh_set->get_mesh_by_zone_num(req.zone_num)->nodes[local_faces[i].vert[k]].rho;
						MPI::COMM_WORLD.Send(&faces_n_resp, 1, MPI_FACES_N_RESP, source, TAG_SYNC_FACES_N_RESP);
					}
				}
				memcpy(faces_f_resp.verts, local_faces[i].vert, 3*sizeof(int));
				faces_f_resp.num = local_faces[i].local_num;
				faces_f_resp.zone_num = req.zone_num;
				MPI::COMM_WORLD.Send(&faces_f_resp, 1, MPI_FACES_F_RESP, source, TAG_SYNC_FACES_F_RESP);
			}
			local_faces.clear();
			local_nodes.clear();
			// message processed, notify
			MPI::COMM_WORLD.Send(NULL, 0, MPI::BYTE, source, TAG_SYNC_FACES_R_END);
			break;
		default:
			throw GCMException(GCMException::SYNC_EXCEPTION, "Got invalid tag while processing sync outline incoming messages.");
	}
	return false;
}

void DataBus::get_remote_faces_in_intersection(int proc_num, int zone_num, MeshOutline &intersection, vector<ElasticNode> &remote_nodes, vector<Triangle> &remote_faces, int &procs_to_sync)
{
	MPIFacesRequest req;
	MPI::Status status;

	// send sync request
	*logger << "Requesting for remote nodes in intersection with zone " << zone_num << " from proc " < proc_num;
	memcpy(req.min_coords, intersection.min_coords, 3*sizeof(float));
	memcpy(req.max_coords, intersection.max_coords, 3*sizeof(float));
	req.zone_num = zone_num;
	// DEBUG
	*logger < "Sending FACES_REQ";
	for (int t = 0; t < 3; t++)
		*logger << intersection.min_coords[t] << "   " < intersection.max_coords[t];
	MPI::COMM_WORLD.Send(&req, 1, MPI_FACES_REQ, proc_num, TAG_SYNC_FACES_REQ);
	*logger < "Request sent";
	// wait for response
	while (true)
	{
		check_messages_sync(MPI::ANY_SOURCE, TAG_CLASS_SYNC_FACES, status);
		if (process_faces_sync_message(status.Get_source(), status.Get_tag(), remote_nodes, remote_faces, procs_to_sync))
			return;
	}
}

bool DataBus::check_messages_async(int source, int *tags, MPI::Status &status)
{
	int *ptr;

	ptr = tags;
	while (*ptr > 0)
	{
		if (MPI::COMM_WORLD.Iprobe(source, *ptr, status))
			return true;
		ptr++;
	}
	
	return false;
	
}

void DataBus::check_messages_sync(int source, int *tags, MPI::Status &status)
{
	int *ptr;

	while (true)
	{
		ptr = tags;
		while (*ptr > 0)
		{
			if (MPI::COMM_WORLD.Iprobe(source, *ptr, status))
				return;
			ptr++;
		}
	}
}

void DataBus::remote_faces_sync_done()
{
	for (int i = 0; i < procs_total_num; i++)
		if (i != proc_num)
			MPI::COMM_WORLD.Send(NULL, 0, MPI::BYTE, i, TAG_SYNC_FACES_DONE);
}

void DataBus::terminate()
{
	MPI::COMM_WORLD.Abort(MPI_CODE_TERMINATED);
}

void DataBus::process_tetrs_sync_message(int source, int tag, vector<Tetrahedron_1st_order> &tetrs, vector<ElasticNode> &nodes, int &resps_to_get, int &procs_to_sync)
{
	Tetrahedron_1st_order tetr;
	ElasticNode node;
	MPITetrsTResponse tetrs_t_resp;
	MPIFacesNResponse faces_n_resp;
	MPITetrsRequest req;
	vector<ElasticNode> local_nodes;
	TetrMesh_1stOrder *mesh;
	Triangle *face;
	Tetrahedron_1st_order *tmp_tetr;
	bool flag;

	switch (tag)
	{
		case TAG_SYNC_TETRS_R_END:
			MPI::COMM_WORLD.Recv(NULL, 0, MPI::BYTE, source, tag);
			resps_to_get--;
			return;
		case TAG_SYNC_TETRS_DONE:
			MPI::COMM_WORLD.Recv(NULL, 0, MPI::BYTE, source, tag);
			procs_to_sync--;
			return;
		case TAG_SYNC_TETRS_T_RESP:
			MPI::COMM_WORLD.Recv(&tetrs_t_resp, 1, MPI_TETRS_T_RESP, source, tag);
			memcpy(tetr.vert, tetrs_t_resp.verts, 4*sizeof(int));
			// FIXME 
			// here's not so obvious variables assignment
			tetr.local_num = tetrs_t_resp.face_num;
			tetr.absolute_num = tetrs_t_resp.zone_num;
			tetrs.push_back(tetr);
			break;
		case TAG_SYNC_TETRS_N_RESP:
			MPI::COMM_WORLD.Recv(&faces_n_resp, 1, MPI_FACES_N_RESP, source, tag);
			memcpy(node.coords, faces_n_resp.coords, 3*sizeof(float));
			memcpy(node.values, faces_n_resp.values, 9*sizeof(float));
			node.local_num = faces_n_resp.num;
			node.local_zone_num = faces_n_resp.zone_num;
			node.la = faces_n_resp.la;
			node.mu = faces_n_resp.mu;
			node.rho = faces_n_resp.rho;
			nodes.push_back(node);
			break;
		case TAG_SYNC_TETRS_REQ:
			MPI::COMM_WORLD.Recv(&req, 1, MPI_TETRS_REQ, source, tag);
			mesh = mesh_set->get_mesh_by_zone_num(req.zone_num);
			face = &mesh->border[req.face_num];
			for (int i = 0; i < 3; i++)
				for (int j = 0; j < mesh->nodes[face->vert[i]].elements->size(); j++)
				{
					tmp_tetr = &(mesh->tetrs[ mesh->nodes[ face->vert[i] ].elements->at(j) ]);
					memcpy(tetrs_t_resp.verts, tmp_tetr->vert, 4*sizeof(int));
					for (int k = 0; k < 4; k++)
					{
						flag = true;
						for (int m = 0; m < local_nodes.size(); m++)
							if (local_nodes[m].local_num == tmp_tetr->vert[k])
							{
								flag = false;
								break;
							}
						if (flag)
							local_nodes.push_back(mesh->nodes[ tmp_tetr->vert[k] ]);
					}
					tetrs_t_resp.face_num = tmp_tetr->local_num;
					tetrs_t_resp.zone_num = req.zone_num;
					MPI::COMM_WORLD.Isend(&tetrs_t_resp, 1, MPI_TETRS_T_RESP, source, TAG_SYNC_TETRS_T_RESP);
				}
			for (int i = 0; i < local_nodes.size(); i++)
			{
				memcpy(faces_n_resp.coords, local_nodes[i].coords, 3*sizeof(float));
				memcpy(faces_n_resp.values, local_nodes[i].values, 9*sizeof(float));
				faces_n_resp.zone_num = req.zone_num;
				faces_n_resp.num = local_nodes[i].local_num;
				MPI::COMM_WORLD.Isend(&faces_n_resp, 1, MPI_FACES_N_RESP, source, TAG_SYNC_TETRS_N_RESP);
			}
			MPI::COMM_WORLD.Isend(NULL, 0, MPI::BYTE, source, TAG_SYNC_TETRS_R_END);
			break;
		default:
			throw GCMException(GCMException::SYNC_EXCEPTION, "Got invalid tag while processing sync outline incoming messages.");
	}
}


void DataBus::get_remote_tetrahedrons(vector<ElasticNode> &virtual_nodes, vector<Tetrahedron_1st_order> &tetrs, vector<ElasticNode> &nodes)
{
	int procs_to_sync = procs_total_num;
	int resps_to_get = 0;
	MPITetrsRequest req;
	MPI::Status status;

	// sending requests
	for (int i = 0; i < virtual_nodes.size(); i++)
	{
		// remote_num stores number of remote face (!)
		req.zone_num = virtual_nodes[i].remote_zone_num;
		req.face_num = virtual_nodes[i].remote_num;
		resps_to_get++;
		MPI::COMM_WORLD.Isend(&req, 1, MPI_TETRS_REQ, get_proc_for_zone(req.zone_num), TAG_SYNC_TETRS_REQ);
		while (check_messages_async(MPI::ANY_SOURCE, TAG_CLASS_SYNC_TETRS, status))
			process_tetrs_sync_message(status.Get_source(), status.Get_tag(), tetrs, nodes, resps_to_get, procs_to_sync);
	}

	while (procs_to_sync > 0)
	{
		if (resps_to_get == 0)
		{
			resps_to_get = -1;
			for (int i = 0; i < procs_total_num; i++)
				if (i != proc_num)
					MPI::COMM_WORLD.Isend(NULL, 0, MPI::BYTE, i, TAG_SYNC_TETRS_DONE);
			if (--procs_to_sync == 0)
				break;
		}
		check_messages_sync(MPI::ANY_SOURCE, TAG_CLASS_SYNC_TETRS, status);
		process_tetrs_sync_message(status.Get_source(), status.Get_tag(), tetrs, nodes, resps_to_get, procs_to_sync);
	}
}

void DataBus::create_custom_types() {
	
	*logger < "Creating custom datatypes";
	// register new MPI types
	MPINode node_resp;
	TetrMesh_1stOrder meshes[2];
	MPIFacesFResponse faces_f_resp;
	MPIFacesNResponse faces_n_resp;
	MPIFacesRequest faces_req;
	MPITetrsTResponse tetrs_t_resp;
	MPITetrsRequest tetrs_req;
	
	MPI::Aint mpi_addr_struct;
	MPI::Aint mpi_addr_struct_values;
	MPI::Aint mpi_addr_struct_verts;
	MPI::Aint mpi_addr_struct_min_coords;
	MPI::Aint mpi_addr_struct_max_coords;
	MPI::Aint mpi_addr_struct_coords;
	MPI::Aint mpi_addr_struct_local_num;
	MPI::Aint mpi_addr_struct_num;
	MPI::Aint mpi_addr_struct_face_num;
	MPI::Aint mpi_addr_struct_remote_num;
	MPI::Aint mpi_addr_struct_local_zone_num;
	MPI::Aint mpi_addr_struct_remote_zone_num;
	MPI::Aint mpi_addr_struct_proc_num;
	MPI::Aint mpi_addr_struct_zone_num;
	MPI::Aint mpi_addr_struct_la;
	MPI::Aint mpi_addr_struct_mu;
	MPI::Aint mpi_addr_struct_rho;

	mpi_addr_struct = MPI::Get_address(&node_resp);
	mpi_addr_struct_values = MPI::Get_address(&node_resp.values);
	mpi_addr_struct_coords = MPI::Get_address(&node_resp.coords);
	mpi_addr_struct_local_num = MPI::Get_address(&node_resp.local_num);
	mpi_addr_struct_local_zone_num = MPI::Get_address(&node_resp.local_zone_num);
	mpi_addr_struct_la = MPI::Get_address(&node_resp.la);
	mpi_addr_struct_mu = MPI::Get_address(&node_resp.mu);
	mpi_addr_struct_rho = MPI::Get_address(&node_resp.rho);

	MPI::Datatype resp_types[] = {
		MPI::FLOAT,
		MPI::FLOAT,
		MPI::INT,
		MPI::INT,
		MPI::FLOAT,
		MPI::FLOAT,
		MPI::FLOAT
	};
	int resp_lengths[] = {
		9,
		3,
		1,
		1,
		1,
		1,
		1
	};
	MPI::Aint resp_displacements[] = {
		mpi_addr_struct_values-mpi_addr_struct,
		mpi_addr_struct_coords-mpi_addr_struct,
		mpi_addr_struct_local_num-mpi_addr_struct,
		mpi_addr_struct_local_zone_num-mpi_addr_struct,
		mpi_addr_struct_la-mpi_addr_struct,
		mpi_addr_struct_mu-mpi_addr_struct,
		mpi_addr_struct_rho-mpi_addr_struct
	};

	MPI_NODE = MPI::Datatype::Create_struct(
		7,
		resp_lengths,
		resp_displacements,
		resp_types
	);
	
	MPI::Datatype outl_types[] = {
		MPI::LB,
		MPI::FLOAT,
		MPI::FLOAT,
		MPI::UB
	};
	int outl_lengths[] = {
		1,
		3,
		3,
		1				
	};
	
	MPI::Aint outl_displacements[] = {
		MPI::Get_address(&meshes[0]),
		MPI::Get_address(&meshes[0].outline.min_coords[0]),
		MPI::Get_address(&meshes[0].outline.max_coords[0]),
		MPI::Get_address(&meshes[1])
	};
	
	for (int i = 3; i >=0; i--)
		outl_displacements[i] -= outl_displacements[0];
	
	MPI_OUTLINE = MPI::Datatype::Create_struct(
		4,
		outl_lengths,
		outl_displacements,
		outl_types
	);
	
	mpi_addr_struct = MPI::Get_address(&faces_req);
	mpi_addr_struct_zone_num = MPI::Get_address(&faces_req.zone_num);
	mpi_addr_struct_min_coords = MPI::Get_address(&faces_req.min_coords);
	mpi_addr_struct_max_coords = MPI::Get_address(&faces_req.max_coords);

	MPI::Datatype faces_req_types[] = {
		MPI::INT,
		MPI::FLOAT,
		MPI::FLOAT
	};
	int faces_req_lengths[] = {
		1,
		3,
		3
	};
	MPI::Aint faces_req_displacements[] = {
		mpi_addr_struct_zone_num-mpi_addr_struct,
		mpi_addr_struct_min_coords-mpi_addr_struct,
		mpi_addr_struct_max_coords-mpi_addr_struct
	};

	MPI_FACES_REQ = MPI::Datatype::Create_struct(
		3,
		faces_req_lengths,
		faces_req_displacements,
		faces_req_types
	);
	
	mpi_addr_struct = MPI::Get_address(&faces_f_resp);
	mpi_addr_struct_verts = MPI::Get_address(&faces_f_resp.verts);
	mpi_addr_struct_num = MPI::Get_address(&faces_f_resp.num);
	mpi_addr_struct_zone_num = MPI::Get_address(&faces_f_resp.zone_num);

	MPI::Datatype faces_f_resp_types[] = {
		MPI::FLOAT,
		MPI::INT,
		MPI::INT
	};
	int faces_f_resp_lengths[] = {
		3,
		1,
		1
	};
	MPI::Aint faces_f_resp_displacements[] = {
		mpi_addr_struct_verts-mpi_addr_struct,
		mpi_addr_struct_num-mpi_addr_struct,
		mpi_addr_struct_zone_num-mpi_addr_struct
	};

	MPI_FACES_F_RESP = MPI::Datatype::Create_struct(
		3,
		faces_f_resp_lengths,
		faces_f_resp_displacements,
		faces_f_resp_types
	);
	
	mpi_addr_struct = MPI::Get_address(&faces_n_resp);
	mpi_addr_struct_num = MPI::Get_address(&faces_n_resp.num);
	mpi_addr_struct_zone_num = MPI::Get_address(&faces_n_resp.zone_num);
	mpi_addr_struct_coords = MPI::Get_address(&faces_n_resp.coords);
	mpi_addr_struct_values = MPI::Get_address(&faces_n_resp.values);
	mpi_addr_struct_la = MPI::Get_address(&faces_n_resp.la);
	mpi_addr_struct_mu = MPI::Get_address(&faces_n_resp.mu);
	mpi_addr_struct_rho = MPI::Get_address(&faces_n_resp.rho);

	MPI::Datatype faces_n_resp_types[] = {
		MPI::INT,
		MPI::INT,
		MPI::FLOAT,
		MPI::FLOAT,
		MPI::FLOAT,
		MPI::FLOAT,
		MPI::FLOAT
	};
	int faces_n_resp_lengths[] = {
		1,
		1,
		3,
		9,
		1,
		1,
		1
	};
	MPI::Aint faces_n_resp_displacements[] = {
		mpi_addr_struct_num-mpi_addr_struct,
		mpi_addr_struct_zone_num-mpi_addr_struct,
		mpi_addr_struct_coords-mpi_addr_struct,
		mpi_addr_struct_values-mpi_addr_struct,
		mpi_addr_struct_la-mpi_addr_struct,
		mpi_addr_struct_mu-mpi_addr_struct,
		mpi_addr_struct_rho-mpi_addr_struct
	};

	MPI_FACES_N_RESP = MPI::Datatype::Create_struct(
		7,
		faces_n_resp_lengths,
		faces_n_resp_displacements,
		faces_n_resp_types
	);
	
	mpi_addr_struct = MPI::Get_address(&tetrs_req);
	mpi_addr_struct_face_num = MPI::Get_address(&tetrs_req.face_num);
	mpi_addr_struct_zone_num = MPI::Get_address(&tetrs_req.zone_num);

	MPI::Datatype tetrs_req_types[] = {
		MPI::INT,
		MPI::INT,
	};
	
	int tetrs_req_lengths[] = {
		1,
		1
	};
	MPI::Aint tetrs_req_displacements[] = {
		mpi_addr_struct_face_num-mpi_addr_struct,
		mpi_addr_struct_zone_num-mpi_addr_struct
	};

	MPI_TETRS_REQ = MPI::Datatype::Create_struct(
		2,
		tetrs_req_lengths,
		tetrs_req_displacements,
		tetrs_req_types
	);
	
	mpi_addr_struct = MPI::Get_address(&tetrs_t_resp);
	mpi_addr_struct_verts = MPI::Get_address(&tetrs_t_resp.verts);
	mpi_addr_struct_zone_num = MPI::Get_address(&tetrs_t_resp.zone_num);
	mpi_addr_struct_num = MPI::Get_address(&tetrs_t_resp.face_num);

	MPI::Datatype tetrs_t_resp_types[] = {
		MPI::FLOAT,
		MPI::INT,
		MPI::INT
	};
	
	int tetrs_t_resp_lengths[] = {
		4,
		1,
		1,
	};
	MPI::Aint tetrs_t_resp_displacements[] = {
		mpi_addr_struct_verts-mpi_addr_struct,
		mpi_addr_struct_zone_num-mpi_addr_struct,
		mpi_addr_struct_num-mpi_addr_struct
	};

	MPI_TETRS_T_RESP = MPI::Datatype::Create_struct(
		3,
		tetrs_t_resp_lengths,
		tetrs_t_resp_displacements,
		tetrs_t_resp_types
	);

	// FIXME
	// it's overhead
	local_numbers = new vector<int>*[zones_info.size()];
	vector<int> **remote_numbers = new vector<int>*[zones_info.size()];	
	MPI_NODE_TYPES = new MPI::Datatype*[zones_info.size()];	
	
	for (int i = 0; i < zones_info.size(); i++)
	{
		local_numbers[i] = new vector<int>[zones_info.size()];
		remote_numbers[i] = new vector<int>[zones_info.size()];
		MPI_NODE_TYPES[i] = new MPI::Datatype[zones_info.size()];
	}		
	
	// find all remote nodes
	for (int i = 0; i < mesh_set->get_number_of_local_meshes(); i++)
	{
		TetrMesh_1stOrder *mesh = mesh_set->get_local_mesh(i);
		for (int j = 0; j < mesh->nodes.size(); j++)
			if (mesh->nodes[j].placement_type == REMOTE &&
					get_proc_for_zone(mesh->nodes[j].remote_zone_num) != proc_num)
			{
				int local_zone_num = mesh->nodes[j].local_zone_num;
				int remote_zone_num = mesh->nodes[j].remote_zone_num;
				local_numbers[local_zone_num][remote_zone_num].push_back(mesh->nodes[j].local_num);
				remote_numbers[local_zone_num][remote_zone_num].push_back(mesh->nodes[j].remote_num);
			}
	}
	
	// sync types
	ElasticNode elnodes[2];
	MPI::Datatype elnode_types[] = {
		MPI::LB,
		MPI::FLOAT,
		MPI::FLOAT,
		MPI::FLOAT,
		MPI::FLOAT,
		MPI::FLOAT,
		MPI::UB
	};
	
	int elnode_lens[] = {
		1,
		9,
		3,
		1,
		1,
		1,
		1
	};
	
	MPI::Aint elnode_displs[] = {
		MPI::Get_address(&elnodes[0]),
		MPI::Get_address(&elnodes[0].values[0]),
		MPI::Get_address(&elnodes[0].coords[0]),
		MPI::Get_address(&elnodes[0].rho),
		MPI::Get_address(&elnodes[0].la),
		MPI::Get_address(&elnodes[0].mu),
		MPI::Get_address(&elnodes[1])
	};
	for (int i = 6; i >= 0; i--)
		elnode_displs[i] -= elnode_displs[0];
	
	MPI_ELNODE = MPI::Datatype::Create_struct(
		7,
		elnode_lens,
		elnode_displs,
		elnode_types
	);
	MPI_ELNODE.Commit();
	
	int max_len = 0;
	for (int i = 0; i < zones_info.size(); i++)
		for (int j = 0; j < zones_info.size(); j++)
			if (local_numbers[i][j].size() > max_len)
				max_len = local_numbers[i][j].size();

	vector<int> lengths;
	for (int i = 0; i < max_len; i++)
		lengths.push_back(1);
	
	int info[3];
	
	for (int i = 0; i < zones_info.size(); i++)
		for (int j = 0; j < zones_info.size(); j++)
			if (local_numbers[i][j].size() > 0)
			{
				info[0] = remote_numbers[i][j].size();
				info[1] = i;
				info[2] = j;
				MPI_NODE_TYPES[i][j] =  MPI_ELNODE.Create_indexed(
					local_numbers[i][j].size(),
					&lengths[0],
					&local_numbers[i][j][0]
				);
				MPI_NODE_TYPES[i][j].Commit();
				MPI::COMM_WORLD.Isend(
					&remote_numbers[i][j][0],
					remote_numbers[i][j].size(),
					MPI::INT,
					get_proc_for_zone(j),
					TAG_SYNC_NODE_TYPES
				);
				MPI::COMM_WORLD.Isend(
					info,
					3,
					MPI::INT,
					get_proc_for_zone(j),
					TAG_SYNC_NODE_TYPES_I
				);
			}

	MPI::COMM_WORLD.Barrier();
	
	MPI::Status status;	
	
	while (MPI::COMM_WORLD.Iprobe(MPI::ANY_SOURCE, TAG_SYNC_NODE_TYPES_I, status))
	{
		MPI::COMM_WORLD.Recv(
			info,
			3,
			MPI::INT,
			status.Get_source(),
			TAG_SYNC_NODE_TYPES_I
		);
		local_numbers[info[1]][info[2]].resize(info[0]);
		MPI::COMM_WORLD.Recv(
			&local_numbers[info[1]][info[2]][0],
			info[0],
			MPI::INT,
			status.Get_source(),
			TAG_SYNC_NODE_TYPES
		);
		if (lengths.size() < info[0])
			for (int i = lengths.size(); i < info[0]; i++)
				lengths.push_back(1);
		MPI_NODE_TYPES[info[1]][info[2]] =  MPI_ELNODE.Create_indexed(
			info[0],
			&lengths[0],
			&local_numbers[info[1]][info[2]][0]
		);
		MPI_NODE_TYPES[info[1]][info[2]].Commit();
	}

	MPI::COMM_WORLD.Barrier();
	
	for (int i = 0 ; i < zones_info.size(); i++)
		delete[] remote_numbers[i];
	delete[] remote_numbers;
	
	for (int i = 0; i < zones_info.size(); i++)
		for (int j = 0; j < zones_info.size(); j++)
			if (local_numbers[i][j].size())
				if (get_proc_for_zone(i) == proc_num)
					*logger << "Nodes in zone " << i << " from zone " << j << " " <  local_numbers[i][j].size();
				else
					*logger << "Nodes in zone " << j << " for zone " << i << " " <  local_numbers[i][j].size();

	MPI_NODE.Commit();
	MPI_OUTLINE.Commit();
	MPI_FACES_REQ.Commit();
	MPI_TETRS_REQ.Commit();
	MPI_FACES_F_RESP.Commit();
	MPI_FACES_N_RESP.Commit();
	MPI_TETRS_T_RESP.Commit();	
	
	*logger < "Custom datatypes created";
}