TaskPreparator::TaskPreparator()
{
	logger = new Logger();
	task_filename = "";
	task_preparator_type.assign("Generic task preparator");
};

TaskPreparator::TaskPreparator(string filename)
{
	logger = new Logger();
	task_filename = filename;
	task_preparator_type.assign("Generic task preparator");
};

TaskPreparator::~TaskPreparator() { };

void TaskPreparator::attach(Logger* new_logger)
{
	if(logger != NULL)
		delete(logger);
	logger = new_logger;
};

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

int TaskPreparator::load_task( string filename, TetrMeshSet* mesh_set )
{	
	mesh_set->attach( logger );
	
	CollisionDetector* cd = new CollisionDetector();
	cd->set_treshold( 0.1 );
	mesh_set->attach( cd );
	
	Stresser* stresser = new GCMStresser();
	stresser->loadTask("task.xml");
	mesh_set->attach(stresser);
	
	VoidRheologyCalculator* rc = new VoidRheologyCalculator();
	mesh_set->attach(rc);
	
	TiXmlDocument document( filename.c_str() );
	bool loadOk = document.LoadFile();
	if( loadOk ) {
		TiXmlElement* eroot = document.FirstChildElement( "task" );
		if( eroot ) {
			TiXmlElement* emesh = eroot->FirstChildElement( "mesh" );
			while( true )
			{	
				if( emesh ) {
					string meshpath = emesh->Attribute( "file" );
					if( meshpath != "" ) {
						TetrMesh_1stOrder* new_mesh = new TetrMesh_1stOrder();
						if ( new_mesh->load_msh_file( const_cast<char*>( meshpath.c_str() ) ) < 0 ) {
							if(logger != NULL)
								logger->write("ERROR: TaskPreparator - can not open mesh file!");
							return 1;
						}
						GCM_Tetr_Plastic_Interpolation_1stOrder_Rotate_Axis* new_nm 
								= new GCM_Tetr_Plastic_Interpolation_1stOrder_Rotate_Axis();
						new_mesh->attach( new_nm );
						mesh_set->attach( new_mesh );
					}
					else {
						if(logger != NULL)
							logger->write("WARN: TaskPreparator - mesh path in task is void");
					}
				}
				else {
					break;
				}
				emesh = emesh->NextSiblingElement( "mesh" );
			}
			
			TiXmlElement* erheo = eroot->FirstChildElement( "rheology" );
			while( true )
			{	
				if( erheo ) {
					float la = atof( erheo->Attribute( "la" ) );
					float mu = atof( erheo->Attribute( "mu" ) );
					float rho = atof( erheo->Attribute( "rho" ) );
					float yield = atof( erheo->Attribute( "yield" ) ); 
				
					if( la <= 0.0 || mu <= 0.0 || rho <= 0.0 || yield <= 0.0 ) {
						if(logger != NULL)
							logger->write("ERROR: TaskPreparator - bad rheology parameters!");
						return 1;
					}
				
					TiXmlElement* earea = erheo->FirstChildElement( "area" );
					if( earea ) {
						string areatype = earea->Attribute( "type" );
						if( areatype == "mesh" ) {			//TODO add other options here
							int meshNum = atoi( earea->GetText() );
							if( meshNum == 0 && strcmp( earea->GetText(), "0" ) != 0 ) {
							if(logger != NULL)
								logger->write("ERROR: TaskPreparator - cannot read rheology area specification!");
								return 1;
							}
							set_fixed_elastic_rheology( &( ( mesh_set->get_mesh( meshNum ) )->nodes ), la, mu, rho, yield );
						}
					}
					else {
						if(logger != NULL)
							logger->write("ERROR: TaskPreparator - area of rheology is not specified!");
						return 1;
					}
					
				}
				else {
					break;
				}
				erheo = erheo->NextSiblingElement( "rheology" );
			}
		}
		else {
			if(logger != NULL)
				logger->write("ERROR: TaskPreparator - failed to load xml task!");
			return 1;	
		}
	}
	else {
		if(logger != NULL)
			logger->write("ERROR: TaskPreparator - failed to load xml task!");
		return 1;
	}
	return 0;
};

int TaskPreparator::load_snap_info( string filename, int* snap_num, int* step_per_snap, SnapshotWriter* sw ) 
{
	sw->attach( logger );

	TiXmlDocument document( filename.c_str() );
	bool loadOk = document.LoadFile();
	if( loadOk ) {
		TiXmlElement* eroot = document.FirstChildElement( "task" );
		if( eroot ) {
			int snap_num_l = atoi( eroot->Attribute( "snap_num" ) );
			int step_per_snap_l = atoi( eroot->Attribute( "step_per_snap" ) );
			
			if( snap_num_l <= 0 || step_per_snap_l <= 0 ) {
				if(logger != NULL)
					logger->write("ERROR: TaskPreparator - wrong snap info!");
				return 1;
			}
			else {
				*snap_num = snap_num_l;
				*step_per_snap = step_per_snap_l;
				return 0;
			}
		}
		else {
			if(logger != NULL)
				logger->write("ERROR: TaskPreparator - failed to load xml task!");
			return 1;	
		}
	}
	else {
		if(logger != NULL)
			logger->write("ERROR: TaskPreparator - failed to load xml task!");
		return 1;
	}
	return 0;
};
