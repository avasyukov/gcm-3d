#include "DataBus.h"

int DataBus::TAG_CLASS_SYNC_NODE[] = {
	DataBus::TAG_SYNC_NODE_REQ,
	DataBus::TAG_SYNC_NODE_RESP,
	DataBus::TAG_SYNC_NODE_DONE, 
	-1
};

int DataBus::TAG_CLASS_SYNC_OUTLINE[] = {
	DataBus::TAG_SYNC_OUTLINE, 
	DataBus::TAG_SYNC_OUTLINE_DONE, 
	-1
};

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
	logger->write("MPI initialized");
	// register new MPI types
	MPINodeRequest node_req;
	MPINodeResponse node_resp;
	MPIMeshOutline outl;
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

	mpi_addr_struct = MPI::Get_address(&node_req);
	mpi_addr_struct_local_num = MPI::Get_address(&node_req.local_num);
	mpi_addr_struct_remote_num = MPI::Get_address(&node_req.remote_num);
	mpi_addr_struct_local_zone_num = MPI::Get_address(&node_req.local_zone_num);
	mpi_addr_struct_remote_zone_num = MPI::Get_address(&node_req.remote_zone_num);

	MPI::Datatype req_types[] = {
		MPI::INT,
		MPI::INT,
		MPI::INT,
		MPI::INT
	};
	int req_lengths[] = {
		1,
		1,
		1,
		1
	};
	MPI::Aint req_displacements[] = {
		mpi_addr_struct_local_num-mpi_addr_struct,
		mpi_addr_struct_remote_num-mpi_addr_struct,
		mpi_addr_struct_local_zone_num-mpi_addr_struct,
		mpi_addr_struct_remote_zone_num-mpi_addr_struct
	};

	MPI_NODE_REQ = MPI::Datatype::Create_struct(
		4,
		req_lengths, 
		req_displacements, 
		req_types
	);
	
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
		mpi_addr_struct_num-mpi_addr_struct,
		mpi_addr_struct_zone_num-mpi_addr_struct,
		mpi_addr_struct_la-mpi_addr_struct,
		mpi_addr_struct_mu-mpi_addr_struct,
		mpi_addr_struct_rho-mpi_addr_struct
	};

	MPI_NODE_RESP = MPI::Datatype::Create_struct(
		7,
		resp_lengths,
		resp_displacements,
		resp_types
	);
	
	mpi_addr_struct = MPI::Get_address(&outl);
	mpi_addr_struct_proc_num = MPI::Get_address(&outl.proc_num);
	mpi_addr_struct_zone_num = MPI::Get_address(&outl.zone_num);
	mpi_addr_struct_min_coords = MPI::Get_address(&outl.min_coords);
	mpi_addr_struct_max_coords = MPI::Get_address(&outl.max_coords);

	MPI::Datatype outl_types[] = {
		MPI::INT,
		MPI::INT,
		MPI::FLOAT,
		MPI::FLOAT
	};
	int outl_lengths[] = {
		1,
		1,
		3,
		3
	};
	MPI::Aint outl_displacements[] = {
		mpi_addr_struct_proc_num-mpi_addr_struct,
		mpi_addr_struct_zone_num-mpi_addr_struct,
		mpi_addr_struct_min_coords-mpi_addr_struct,
		mpi_addr_struct_max_coords-mpi_addr_struct
	};

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

	MPI_NODE_REQ.Commit();
	MPI_NODE_RESP.Commit();
	MPI_OUTLINE.Commit();
	MPI_FACES_REQ.Commit();
	MPI_TETRS_REQ.Commit();
	MPI_FACES_F_RESP.Commit();
	MPI_FACES_N_RESP.Commit();
	MPI_TETRS_T_RESP.Commit();
};

DataBus::~DataBus()
{
	// try to finilize MPI
	MPI_NODE_REQ.Free();
	MPI_NODE_RESP.Free();
	MPI::Finalize();
	logger->write("MPI finalized");
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
	MPI::Status status;
	if (!proc_num)
	{
		// main process

		// get data from all nodes
		logger->write("Retrieving taus from other processes");
		for (int i = 1; i < procs_total_num; i++) {
			MPI::COMM_WORLD.Recv(&max_tau, 1, MPI::FLOAT, i, TAG_SYNC_TIME_STEP, status);
			if (local_time_step < max_tau)
				local_time_step = max_tau;
		}
		max_tau = local_time_step;
		logger->write("Tau found, sending it back to all processes");
		// max tau found, send it to all procs
		for (int i = 1; i < procs_total_num; i++) {
			MPI::COMM_WORLD.Send(&max_tau, 1, MPI::FLOAT, i, TAG_SYNC_TIME_STEP);
		}
	}
	else
	{
		// slave process

		// send tau to main process
		logger->write("Sending max tau to main process");
		MPI::COMM_WORLD.Send(&local_time_step, 1, MPI::FLOAT, 0, TAG_SYNC_TIME_STEP);
		// get synchronized value
		logger->write("Waiting for tau from main process");
		MPI::COMM_WORLD.Recv(&max_tau, 1, MPI::FLOAT, 0, TAG_SYNC_TIME_STEP, status);
	}

	*logger << "Time step synchronized, value is: " < max_tau; 

	return max_tau;
}

void DataBus::process_nodes_sync_message(int source, int tag, int &nodes_to_sync, int &procs_to_sync)
{
	MPINodeRequest req;
	MPINodeResponse resp;
	MPI::Status status;
	ElasticNode *node;
		
	// process message
	switch (tag) 
	{
		case TAG_SYNC_NODE_DONE:
			MPI::COMM_WORLD.Recv(NULL, 0, MPI::BYTE, source, tag, status);
			procs_to_sync--;
			break;
		case TAG_SYNC_NODE_REQ:
			MPI::COMM_WORLD.Recv(&req, 1, MPI_NODE_REQ, source, tag, status);
			node = mesh_set->get_mesh_by_zone_num(req.remote_zone_num)->get_node(req.remote_num);
			// FIXME
			// DEBUG
			if (node->local_num != req.remote_num)
				throw GCMException(GCMException::SYNC_EXCEPTION, "get_node() returned node with invalid num.");
			// FIXME
			// here we have to find an appropriate node to copy data from
			memcpy(resp.values, node->values, sizeof(float)*9);
			memcpy(resp.coords, node->coords, sizeof(float)*3);
			resp.local_num = req.local_num;
			resp.local_zone_num = req.local_zone_num;
			resp.la = node->la;
			resp.mu = node->mu;
			resp.rho = node->rho;
			// send an answer 
			MPI::COMM_WORLD.Send(&resp, 1, MPI_NODE_RESP, source, TAG_SYNC_NODE_RESP);
			break;
		case TAG_SYNC_NODE_RESP:
			MPI::COMM_WORLD.Recv(&resp, 1, MPI_NODE_RESP, source, tag, status);
			node = mesh_set->get_mesh_by_zone_num(resp.local_zone_num)->get_node(resp.local_num);
			// FIXME
			if (node->local_num != resp.local_num)
				throw GCMException(GCMException::SYNC_EXCEPTION, "get_node() returned node with invalid num.");
			nodes_to_sync--;
			memcpy(node->values, resp.values, 9*sizeof(float));
			memcpy(node->coords, resp.coords, 3*sizeof(float));
			node->la = resp.la;
			node->mu = resp.mu;
			node->rho = resp.rho;
			break;
		default:
			throw GCMException(GCMException::SYNC_EXCEPTION, "Got invalid tag while processing sync node incoming messages.");
	}
}

int DataBus::sync_nodes()
{

	logger->write("Starting nodes sync");
	MPINodeRequest req;
	int procs_to_sync = procs_total_num;
	int nodes_to_sync = 0;
	MPI::Status status;
	int i;

	// FIXME
	// meshes are protected, while nodes inside mesh are public
	for (i = 0; i < mesh_set->get_number_of_meshes(); i++)
	{
		TetrMesh_1stOrder* mesh = mesh_set->get_mesh(i);
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
				else
				{
					// fill sync struct
					// FIXME
					// declare somewhere a constant for values number
					req.local_num = mesh->nodes[j].local_num;
					req.remote_num = mesh->nodes[j].remote_num;
					req.local_zone_num = mesh->nodes[j].local_zone_num;
					req.remote_zone_num = mesh->nodes[j].remote_zone_num;
					// send a message
					nodes_to_sync++;
					MPI::COMM_WORLD.Send(&req, 1, MPI_NODE_REQ, dest, TAG_SYNC_NODE_REQ);
				}
				// process incoming requests
				while (check_messages_async(MPI::ANY_SOURCE, TAG_CLASS_SYNC_NODE, status))
					process_nodes_sync_message(status.Get_source(), status.Get_tag(), nodes_to_sync, procs_to_sync);
			}
	}

	// nodes synced, notify all processed
	logger->write("All sync requests sent");

	// wait other nodes to end sync
	while (procs_to_sync > 0 )
	{
		if (nodes_to_sync == 0)
		{
			nodes_to_sync = -1;
			for (i = 0; i < procs_total_num; i++)
				if (i != proc_num)
					MPI::COMM_WORLD.Send(NULL, 0, MPI::BYTE, i, TAG_SYNC_NODE_DONE);
			if (--procs_to_sync == 0)
				break;
		}
		// FIXME
		// DEBUG
		else if (nodes_to_sync < -1)
			throw GCMException(GCMException::SYNC_EXCEPTION, "nodes_to_sync < -1, something wrong with sync!!");
		check_messages_sync(MPI::ANY_SOURCE, TAG_CLASS_SYNC_NODE, status);
		process_nodes_sync_message(status.Get_source(), status.Get_tag(), nodes_to_sync, procs_to_sync);
	}

	logger->write("Sync done");

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


int DataBus::process_outlines_sync_message(int source, int tag, vector<MeshOutline> &remote, vector<MeshOutlineInfo> &info)
{
	MPIMeshOutline mpi_outl;
	MeshOutline outl;
	MeshOutlineInfo outl_info;

	MPI::Status status;
	// process message
	switch (tag) 
	{
		case TAG_SYNC_OUTLINE_DONE:
			MPI::COMM_WORLD.Recv(NULL, 0, MPI::BYTE, source, tag, status);
			return 1;
		case TAG_SYNC_OUTLINE:
			// get message
			MPI::COMM_WORLD.Recv(&mpi_outl, 1, MPI_OUTLINE, source, tag, status);
			// fill struct
			outl_info.proc_num = mpi_outl.proc_num;
			outl_info.zone_num = mpi_outl.zone_num;
			memcpy(outl.min_coords, mpi_outl.min_coords, 3*sizeof(float));
			memcpy(outl.max_coords, mpi_outl.max_coords, 3*sizeof(float));
			// DEBUG
			*logger < "Got outline";
			*logger << "zone: " << outl_info.zone_num << " proc: " < outl_info.proc_num;
			for (int t = 0; t < 3; t++)
				*logger << outl.min_coords[t]  << "  " < outl.max_coords[t];
			// add to list
			remote.push_back(outl);
			info.push_back(outl_info);
			break;
		default:
			throw GCMException(GCMException::SYNC_EXCEPTION, "Got invalid tag while processing sync outline incoming messages.");
	}
	return 0;
}

void DataBus::sync_outlines(vector<MeshOutline> &local, vector<MeshOutline> &remote, vector<MeshOutlineInfo> &info)
{
	int procs_to_sync = procs_total_num;
	MPIMeshOutline mpi_mesh_outline;
	MeshOutlineInfo mesh_outline_info;
	MeshOutline mesh_outline;

	MPI::Status status;

	logger->write("Starting outlines sync");

	for (int i = 0; i < local.size(); i++)
		for (int j = 0; j < procs_total_num; j++)
			if (j != proc_num)
			{
				// build struct
				memcpy(mpi_mesh_outline.min_coords, local[i].min_coords, 3*sizeof(float));
				memcpy(mpi_mesh_outline.max_coords, local[i].max_coords, 3*sizeof(float));
				mpi_mesh_outline.proc_num = proc_num;
				// FIXME
				// avoid access to mesh_set, zones should be passed as
				// arguments?
				mpi_mesh_outline.zone_num = mesh_set->get_mesh(i)->zone_num;
				// send outline
				// DEBUG
				*logger < "Sending outline";
				*logger << "zone: " << mesh_set->get_mesh(i)->zone_num << " proc: " < proc_num;
				for (int t = 0; t < 3; t++)
					*logger << mpi_mesh_outline.min_coords[t]  << "  " < mpi_mesh_outline.max_coords[t];
				*logger < "";
				MPI::COMM_WORLD.Send(&mpi_mesh_outline, 1, MPI_OUTLINE, j, TAG_SYNC_OUTLINE);
				// process incoming messages
				while (check_messages_async(MPI::ANY_SOURCE, TAG_CLASS_SYNC_OUTLINE, status))
					procs_to_sync -= process_outlines_sync_message(status.Get_source(), status.Get_tag(), remote, info);
			}
	procs_to_sync--;
	// outlines sent, notify
	for (int i = 0; i < procs_total_num; i++)
		if (i != proc_num)
			MPI::COMM_WORLD.Send(NULL, 0, MPI::BYTE,  i, TAG_SYNC_OUTLINE_DONE);

	logger->write("All local outlines sent, waiting for other procs");

	// processing incoming messages
	while (procs_to_sync > 0)
	{
		check_messages_sync(MPI::ANY_SOURCE, TAG_CLASS_SYNC_OUTLINE, status);
		procs_to_sync -= process_outlines_sync_message(status.Get_source(), status.Get_tag(),  remote, info);
	}

	logger->write("Outlines sync done");
}

void DataBus::sync()
{
	MPI::Status status;

	logger->write("Syncing state");
	if (proc_num != 0)
	{
		// send ready messages
		MPI::COMM_WORLD.Send(NULL, 0, MPI::BYTE, 0, TAG_SYNC_READY);
		// get a response
		MPI::COMM_WORLD.Recv(NULL, 0, MPI::BYTE, 0, TAG_SYNC_READY, status);
	}
	else
	{
		int i;
		// wait for all procs
		for (i = 1; i < procs_total_num; i++)
			MPI::COMM_WORLD.Recv(NULL, 0, MPI::BYTE, i, TAG_SYNC_READY, status);
		// notify, that states are ok
		for (i = 1; i < procs_total_num; i++)
			MPI::COMM_WORLD.Send(NULL, 0, MPI::BYTE, i, TAG_SYNC_READY);
	}

	logger->write("State synced");
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
					MPI::COMM_WORLD.Send(&tetrs_t_resp, 1, MPI_TETRS_T_RESP, source, TAG_SYNC_TETRS_T_RESP);
				}
			for (int i = 0; i < local_nodes.size(); i++)
			{
				memcpy(faces_n_resp.coords, local_nodes[i].coords, 3*sizeof(float));
				memcpy(faces_n_resp.values, local_nodes[i].values, 9*sizeof(float));
				faces_n_resp.zone_num = req.zone_num;
				faces_n_resp.num = local_nodes[i].local_num;
				MPI::COMM_WORLD.Send(&faces_n_resp, 1, MPI_FACES_N_RESP, source, TAG_SYNC_TETRS_N_RESP);
			}
			MPI::COMM_WORLD.Send(NULL, 0, MPI::BYTE, source, TAG_SYNC_TETRS_R_END);
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
		MPI::COMM_WORLD.Send(&req, 1, MPI_TETRS_REQ, get_proc_for_zone(req.zone_num), TAG_SYNC_TETRS_REQ);
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
					MPI::COMM_WORLD.Send(NULL, 0, MPI::BYTE, i, TAG_SYNC_TETRS_DONE);
			if (--procs_to_sync == 0)
				break;
		}
		check_messages_sync(MPI::ANY_SOURCE, TAG_CLASS_SYNC_TETRS, status);
		process_tetrs_sync_message(status.Get_source(), status.Get_tag(), tetrs, nodes, resps_to_get, procs_to_sync);
	}
}
