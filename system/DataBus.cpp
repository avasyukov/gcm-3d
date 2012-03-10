#include "DataBus.h"

DataBus::DataBus()
{
	data_bus_type.assign("Not so stupid echo data bus");
	// try to initialize MPI
	// TODO: add command line arguments processing if we really need it
	MPI::Init();
	// register error handler that throws exceptions
	MPI::COMM_WORLD.Set_errhandler(MPI::ERRORS_THROW_EXCEPTIONS);
	// MPI initialized, get processor number ant total number of processors
	proc_num = MPI::COMM_WORLD.Get_rank();
	proc_total_num = MPI::COMM_WORLD.Get_size();
	logger->set_proc_num(proc_num);
	logger->write("MPI initialized");
	// register new MPI types
	MPINodeRequest node_req;
	MPINodeResponse node_resp;

	MPI::Aint mpi_addr_struct;
	MPI::Aint mpi_addr_struct_values;
	MPI::Aint mpi_addr_struct_coords;
	MPI::Aint mpi_addr_struct_local_num;
	MPI::Aint mpi_addr_struct_remote_num;
	MPI::Aint mpi_addr_struct_local_zone_num;
	MPI::Aint mpi_addr_struct_remote_zone_num;

	mpi_addr_struct = MPI::Get_address(&node_req);
	mpi_addr_struct_local_num = MPI::Get_address(&node_req.local_num);
	mpi_addr_struct_remote_num = MPI::Get_address(&node_req.remote_num);
	mpi_addr_struct_local_zone_num = MPI::Get_address(&node_req.local_zone_num);
	mpi_addr_struct_remote_zone_num = MPI::Get_address(&node_req.remote_zone_num);

	MPI::Datatype types1[] = {MPI::INT, MPI::INT, MPI::INT, MPI::INT};
	int lengths1[] = {1, 1, 1, 1};
	MPI::Aint displacements1[] = {
		mpi_addr_struct_local_num-mpi_addr_struct,
		mpi_addr_struct_remote_num-mpi_addr_struct,
		mpi_addr_struct_local_zone_num-mpi_addr_struct,
		mpi_addr_struct_remote_zone_num-mpi_addr_struct
	};

	MPI_NODE_REQ = MPI::Datatype::Create_struct(4, lengths1, displacements1, types1);
	
	mpi_addr_struct = MPI::Get_address(&node_resp);
	mpi_addr_struct_values = MPI::Get_address(&node_resp.values);
	mpi_addr_struct_coords = MPI::Get_address(&node_resp.coords);
	mpi_addr_struct_local_num = MPI::Get_address(&node_resp.local_num);
	mpi_addr_struct_local_zone_num = MPI::Get_address(&node_resp.local_zone_num);

	MPI::Datatype types2[] = {MPI::FLOAT, MPI::FLOAT, MPI::INT, MPI::INT};
	int lengths2[] = {9, 3, 1, 1};
	MPI::Aint displacements2[] = {
		mpi_addr_struct_values-mpi_addr_struct,
		mpi_addr_struct_coords-mpi_addr_struct,
		mpi_addr_struct_local_num-mpi_addr_struct,
		mpi_addr_struct_local_zone_num-mpi_addr_struct,
	};

	MPI_NODE_RESP = MPI::Datatype::Create_struct(4, lengths2, displacements2, types2);
};

DataBus::~DataBus()
{
	// try to finilize MPI
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

float DataBus::get_max_possible_tau(float local_time_step)
{
	float max_tau;
	MPI::Status status;
	if (!proc_num)
	{
		// main process

		// get data from all nodes
		logger->write("Retrieving taus from other processes");
		for (int i = 1; i < proc_total_num; i++) {
			MPI::COMM_WORLD.Recv(&max_tau, 1, MPI::FLOAT, i, TAG_SYNC_TIME_STEP, status);
			if (local_time_step < max_tau)
				local_time_step = max_tau;
		}
		max_tau = local_time_step;
		logger->write("Tau found, sending it back to all processes");
		// max tau found, send it to all procs
		for (int i = 1; i < proc_total_num; i++) {
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

	// FIXME
	// string stream again
	stringstream ss;
	ss << "Time step synchronized, value is: " << max_tau; 
	logger->write(ss.str());

	return max_tau;
}

int DataBus::process_request(int source, int tag)
{
	MPINodeRequest req;
	MPINodeResponse resp;
	MPI::Status status;
	// retrieve message from buffer
	if (tag == TAG_SYNC_NODE_DONE)
	{
		MPI::COMM_WORLD.Recv(NULL, 0, MPI::BYTE, source, tag, status);
		// return flag that one more process've synced
		return 1;
	}
	else
	{
		// process message
		switch (tag) 
		{
			ElasticNode *node;
			case TAG_SYNC_NODE_REQ:
				MPI::COMM_WORLD.Recv(&req, 1, MPI_NODE_REQ, source, tag, status);
				node = mesh_set->get_mesh_by_zone_num(req.remote_zone_num)->get_node(req.remote_num);
				// FIXME
				if (node->local_num != req.remote_num)
					throw GCMException(GCMException::SYNC_EXCEPTION, "get_node() returned node with invalid num.");
				// FIXME
				// here we have to find an appropriate node to copy data from
				memcpy(resp.values, node->values, sizeof(float)*9);
				memcpy(resp.coords, node->coords, sizeof(float)*3);
				resp.local_num = req.local_num;
				resp.local_zone_num = req.local_zone_num;
				// send an answer 
				MPI::COMM_WORLD.Send(&resp, 1, MPI_NODE_RESP, source, TAG_SYNC_NODE_RESP);
				break;
			case TAG_SYNC_NODE_RESP:
				MPI::COMM_WORLD.Recv(&resp, 1, MPI_NODE_RESP, source, tag, status);
				node = mesh_set->get_mesh_by_zone_num(resp.local_zone_num)->get_node(resp.local_num);
				// FIXME
				if (node->local_num != resp.local_num)
					throw GCMException(GCMException::SYNC_EXCEPTION, "get_node() returned node with invalid num.");
				memcpy(node->values, resp.values, 9*sizeof(float));
				memcpy(node->coords, resp.coords, 3*sizeof(float));
				break;
		}
	}

	return 0;
}

int DataBus::sync_nodes()
{

	logger->write("Starting nodes sync");
	MPINodeRequest req;
	int nodes_to_be_done = 0;
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
				// fill sync struct
				// FIXME
				// declare somewhere a constant for values number
				req.local_num = mesh->nodes[j].local_num;
				req.remote_num = mesh->nodes[j].remote_num;
				req.local_zone_num = mesh->nodes[j].local_zone_num;
				req.remote_zone_num = mesh->nodes[j].remote_zone_num;
				// send a message
				MPI::COMM_WORLD.Send(&req, 1, MPI_NODE_REQ, get_proc_for_zone(mesh->nodes[j].remote_zone_num), TAG_SYNC_NODE_REQ);
				// process incoming requests
				while (MPI::COMM_WORLD.Iprobe(MPI::ANY_SOURCE, MPI::ANY_TAG, status))
					nodes_to_be_done -= process_request(status.Get_source(), status.Get_tag());
			}
	}

	// nodes synced, notify all processed
	logger->write("Remote nodes synchronized, waiting fo other nodes to end sync");
	nodes_to_be_done--;
	for (i = 0; i < proc_total_num; i++)
		if (i != proc_num)
			MPI::COMM_WORLD.Send(0, 0, MPI::BYTE, i, TAG_SYNC_NODE_DONE);

	// wait other nodes to end sync
	while (nodes_to_be_done > 0 )
	{
		MPI::COMM_WORLD.Probe(MPI::ANY_SOURCE, MPI::ANY_TAG, status);
		nodes_to_be_done -= process_request(status.Get_source(), status.Get_tag());
	}

	logger->write("Sync done");

	// FIXME
	// do we really need to return a value?
	return 0;
};

void DataBus::load_zones_info(string file_name)
{
	// TODO
}

int DataBus::get_proc_num()
{
	return proc_num;
}

int DataBus::get_total_proc_num()
{
	return proc_total_num;
}

int DataBus::get_proc_for_zone(int zone_num)
{
	return zones_info[zone_num];
}
