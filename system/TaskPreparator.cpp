#include "TaskPreparator.h"

TaskPreparator::TaskPreparator()
{
	task_preparator_type.assign("Generic task preparator");
};

TaskPreparator::~TaskPreparator() { };

string* TaskPreparator::get_task_preparator_type()
{
	return &task_preparator_type;
};

void TaskPreparator::set_fixed_elastic_rheology(vector<ElasticNode>* nodes, float la, float mu, float rho, float yield_limit)
{
	if(nodes == NULL)
		return;
	for(int i = 0; i < nodes->size(); i++)
	{
		(nodes->at(i)).la = la;
		(nodes->at(i)).mu = mu;
		(nodes->at(i)).rho = rho;
		(nodes->at(i)).yield_limit = yield_limit;
	}
};

void TaskPreparator::set_fixed_elastic_rheology(vector<ElasticNode>* nodes, MeshOutline* box, float la, float mu, float rho, float yield_limit)
{
	if(nodes == NULL)
		return;
	for(int i = 0; i < nodes->size(); i++)
	{
		if( ( (nodes->at(i)).coords[0] >= box->min_coords[0] ) && ( (nodes->at(i)).coords[0] <= box->max_coords[0] )
			&& ( (nodes->at(i)).coords[1] >= box->min_coords[1] ) && ( (nodes->at(i)).coords[1] <= box->max_coords[1] )
			&& ( (nodes->at(i)).coords[2] >= box->min_coords[2] ) && ( (nodes->at(i)).coords[2] <= box->max_coords[2] ) )
		{
			(nodes->at(i)).la = la;
			(nodes->at(i)).mu = mu;
			(nodes->at(i)).rho = rho;
			(nodes->at(i)).yield_limit = yield_limit;
		}
	}
};

void TaskPreparator::check_rheology_loaded(vector<ElasticNode>* nodes)
{
	if(nodes == NULL)
		return;
	for(int i = 0; i < nodes->size(); i++)
		if( ( (nodes->at(i)).la == 0 ) || ( (nodes->at(i)).mu == 0 ) 
				|| ( (nodes->at(i)).rho == 0 ) || ( (nodes->at(i)).yield_limit == 0 ) )
			throw GCMException(GCMException::CONFIG_EXCEPTION, "Rheology is not set for some nodes");
};

int TaskPreparator::load_zones_info(string zones_file, vector<int>* zones_info)
{
	// Try to open zones map file
	TiXmlDocument zones_document( zones_file.c_str() );
	bool loadOk = zones_document.LoadFile();
	if( !loadOk )
		throw GCMException(GCMException::CONFIG_EXCEPTION, "Can not open zone map file");

	// Read XML
	TiXmlElement* eroot = zones_document.FirstChildElement( "zones_map" );
	if( !eroot )
		throw GCMException(GCMException::CONFIG_EXCEPTION, "Malformed zone map file");

	TiXmlElement* ezone = eroot->FirstChildElement( "zone" );
	if( !ezone )
		throw GCMException(GCMException::CONFIG_EXCEPTION, "Malformed zone map file");

	// For each zone element
	while ( ezone )
	{
		// Read zone number and number of CPU it should belong to
		int zone_num = atoi( ezone->Attribute( "num" ) );
		int cpu_num = atoi( ezone->Attribute( "cpu" ) );
		// We require for the moment that zones go sequentially in the file
		// TODO - add malformed numbering handling and remove this requirement
		if( zone_num != zones_info->size() )
			throw GCMException(GCMException::CONFIG_EXCEPTION, "Malformed zone map file");
		zones_info->push_back( cpu_num );
		ezone = ezone->NextSiblingElement( "zone" );
	}

	*logger << zones_info->size() < " mesh zones in map file"; 

	return 0;
};

int TaskPreparator::load_task( string task_file, string zones_file, string data_dir, 
				int* snap_num, int* steps_per_snap)
{
	*logger < "Loading task from XML";

	// Current process number
	DataBus *data_bus = DataBus::getInstance();
	TetrMeshSet *mesh_set = TetrMeshSet::getInstance();
	int proc_num = data_bus->get_proc_num();

	// Create stresser
	Stresser* stresser = new GCMStresser();
	// Stresser loads data from the file on its own
	// TODO - load should be done by task preparator as well
	stresser->loadTask(task_file);
	// Attach stresser to mesh set
	mesh_set->attach(stresser);

	// Create vector to store zones info
	vector<int> zones_info;
	zones_info.clear();

	// Load zones map from XML file
	load_zones_info(zones_file, &zones_info);

	// Check if we use all CPUs - otherwise consider zones map malformed
	for(int i = 0; i < data_bus->get_procs_total_num(); i++) {
		bool cpu_used = false;
		for(int j = 0; j < zones_info.size(); j++)
			if(zones_info[j] == i)
				cpu_used = true;
		if( !cpu_used )
			throw GCMException(GCMException::CONFIG_EXCEPTION, "Some CPUs have no zones to process");
	}

	// Check if all zones are mapped to correct CPUs - otherwise consider zones map malformed
	for(int i = 0; i < zones_info.size(); i++)
		if(zones_info[i] >= data_bus->get_procs_total_num())
			throw GCMException(GCMException::CONFIG_EXCEPTION, "Some zones are not mapped to correct CPUs");

	// Path zones map to data bus
	data_bus->load_zones_info(&zones_info);
	mesh_set->init_mesh_container(zones_info);

	// Create default VoidRheologyCalculator and attach it to mesh set
	// TODO - make RheologyCalculator type configurable in task xml (see CollisionDetector below)
	VoidRheologyCalculator* rc = new VoidRheologyCalculator();
	mesh_set->attach(rc);
 
	// Collision detector to be used (will be determined and created later)
	CollisionDetector* col_det;
	// Default col det type
	string col_det_type = "BruteforceCollisionDetector";
	string static_col_det = "false";

	// Try to open task file
	TiXmlDocument task_document( task_file.c_str() );
	bool loadOk = task_document.LoadFile();
	if( !loadOk )
		throw GCMException(GCMException::CONFIG_EXCEPTION, "Can not open task file");

	// Read XML
	TiXmlElement* etask = task_document.FirstChildElement( "task" );
	if( !etask )
		throw GCMException(GCMException::CONFIG_EXCEPTION, "Malformed task file");

	// Read snapshot related information
	int snap_num_l = atoi( etask->Attribute( "snap_num" ) );
	int step_per_snap_l = atoi( etask->Attribute( "step_per_snap" ) );

	if( snap_num_l <= 0 || step_per_snap_l <= 0 )
		throw GCMException(GCMException::CONFIG_EXCEPTION, "Bad shapshot info");

	*snap_num = snap_num_l;
	*steps_per_snap = step_per_snap_l;

	// Read system related configuration
	TiXmlElement* esystem = etask->FirstChildElement( "system" );
	if( esystem )
	{
		// Try ty read collision detector info
		TiXmlElement* ecd = esystem->FirstChildElement( "collision_detector" );
		if( ecd )
		{
			string ecd_type = ecd->Attribute( "type" );
			if( ecd_type != "" )
				col_det_type = ecd_type;
			string static_cd = ecd->Attribute( "static" );
			if( static_cd != "" )
				static_col_det = static_cd;
		}
	}

	// Create collision detector
	if( col_det_type == "VoidCollisionDetector" )
		col_det = new VoidCollisionDetector();
	else if( col_det_type == "CollisionDetectorForLayers" )
		col_det = new CollisionDetectorForLayers();
	else
		col_det = new BruteforceCollisionDetector();

	if( static_col_det == "true" )
		col_det->set_static(true);

	// Configure collision detector and attach to mesh set
	col_det->set_treshold( 0.1 );
	mesh_set->attach( col_det );


	// Read meshes
	TiXmlElement* emesh = etask->FirstChildElement( "mesh" );
	if( !emesh )
		throw GCMException(GCMException::CONFIG_EXCEPTION, "Malformed task file");

	vector<TetrMesh_1stOrder*> mesh_zones;

	// Read mesh
	while( emesh )
	{
		// Find mesh zones
		TiXmlElement* ezones = emesh->FirstChildElement( "zones" );
		if( !ezones )
			throw GCMException(GCMException::CONFIG_EXCEPTION, "Malformed task file");

		TiXmlElement* ezone = ezones->FirstChildElement( "zone" );
		if( !ezone )
			throw GCMException(GCMException::CONFIG_EXCEPTION, "Malformed task file");

		// There are no zones in current mesh yet
		mesh_zones.clear();

		// Read all zones
		while( ezone )
		{
			int zone_num = atoi( ezone->Attribute( "num" ) );

			TetrMesh_1stOrder *new_mesh = mesh_set->get_mesh_by_zone_num(zone_num);
			// Load only zones that are scheduled for this CPU
			// create an empty container for other zones
			if( new_mesh->local )
			{
				*logger << "Loading zone: " < zone_num;

				string meshpath = ezone->Attribute( "file" );
				if( meshpath == "" )
					throw GCMException(GCMException::CONFIG_EXCEPTION, "Malformed task file");

				if(meshpath[0] != '/')
					meshpath = data_dir + meshpath;

				if ( new_mesh->load_msh_file( const_cast<char*>( meshpath.c_str() ) ) < 0 )
					throw GCMException(GCMException::CONFIG_EXCEPTION, "Can not open mesh file");

				GCM_Tetr_Plastic_Interpolation_1stOrder_Rotate_Axis* new_nm 
								= new GCM_Tetr_Plastic_Interpolation_1stOrder_Rotate_Axis();
				new_mesh->attach( new_nm );
			}
			mesh_zones.push_back( new_mesh );

			ezone = ezone->NextSiblingElement( "zone" );
		}

		// Read and execute mesh transformations
		TiXmlElement* etrans = emesh->FirstChildElement("transform");
		while( etrans ) {
			string trans_type = etrans->Attribute("type");
			if(trans_type == "translate") {
				float dx = atof( etrans->Attribute("x") );
				float dy = atof( etrans->Attribute("y") );
				float dz = atof( etrans->Attribute("z") );
				for(int i = 0; i < mesh_zones.size(); i++ )
					if( mesh_zones[i]->local )
						mesh_zones[i]->translate(dx, dy, dz);
			}
			etrans = etrans->NextSiblingElement("transform");
		}

		// Attach all zones of current mesh to mesh set
		for(int i = 0; i < mesh_zones.size(); i++ )
			mesh_set->attach( mesh_zones[i] );

		emesh = emesh->NextSiblingElement( "mesh" );
	}
			
	// Read rheology
	TiXmlElement* erheo = etask->FirstChildElement( "rheology" );
	while( erheo )
	{
		float la = atof( erheo->Attribute( "la" ) );
		float mu = atof( erheo->Attribute( "mu" ) );
		float rho = atof( erheo->Attribute( "rho" ) );
		float yield = atof( erheo->Attribute( "yield" ) );

		if( la <= 0.0 || mu <= 0.0 || rho <= 0.0 || yield <= 0.0 ) 
			throw GCMException(GCMException::CONFIG_EXCEPTION, "Bad rheology parameters!");

		TiXmlElement* earea = erheo->FirstChildElement( "area" );
		if( !earea )
			throw GCMException(GCMException::CONFIG_EXCEPTION, "Malformed task file");

		string areatype = earea->Attribute( "type" );
		if( areatype == "mesh" )
		{
			int zone_num = atoi( earea->GetText() );
			if( zone_num == 0 && strcmp( earea->GetText(), "0" ) != 0 )
				throw GCMException(GCMException::CONFIG_EXCEPTION, "Cannot read rheology area specification!");

			// Work only with zones that are scheduled for this CPU
			if( zones_info[zone_num] == proc_num )
			{
				*logger << "Setting rheology for zone: " < zone_num;

				set_fixed_elastic_rheology( 
					&( ( mesh_set->get_mesh_by_zone_num( zone_num ) )->nodes ), 
					la, mu, rho, yield );
			}
		} else if( areatype == "box" ) {
			MeshOutline outline;
			outline.min_coords[0] = atof( earea->Attribute( "minX" ) );
			outline.max_coords[0] = atof( earea->Attribute( "maxX" ) );
			outline.min_coords[1] = atof( earea->Attribute( "minY" ) );
			outline.max_coords[1] = atof( earea->Attribute( "maxY" ) );
			outline.min_coords[2] = atof( earea->Attribute( "minZ" ) );
			outline.max_coords[2] = atof( earea->Attribute( "maxZ" ) );

			for(int i = 0; i < mesh_set->get_number_of_local_meshes(); i++)
					set_fixed_elastic_rheology( &( ( mesh_set->get_local_mesh(i) )->nodes ), &outline, la, mu, rho, yield );
		}
		erheo = erheo->NextSiblingElement( "rheology" );
	}

	for(int i = 0; i < mesh_set->get_number_of_local_meshes(); i++)
			check_rheology_loaded( &( ( mesh_set->get_local_mesh(i) )->nodes ) );

	*logger < "Task loaded";

	return 0;
};
