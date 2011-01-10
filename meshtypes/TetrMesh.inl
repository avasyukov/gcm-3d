TetrMesh::TetrMesh()
{
	mesh_type.assign("Basic tetrahedron mesh");
};

TetrMesh::~TetrMesh() { };

void TetrMesh::attach(TetrNumericalMethod* new_numerical_method)
{
	method = new_numerical_method;
	if(method != NULL)
	{
		method->attach(logger);
		if(logger != NULL)
		{
			logger->write(string("Attached numerical method. Type: ") + *(method->get_num_method_type()));
		}
	}
};

void TetrMesh::attach(Logger* new_logger)
{
	Mesh::attach(new_logger);
};

void TetrMesh::attach(RheologyCalculator* new_rheology)
{
	Mesh::attach(new_rheology);
};
