TaskPreparator::TaskPreparator()
{
	task_fname = "";
	task_preparator_type.assign("Generic task preparator");
};

TaskPreparator::TaskPreparator(string fname)
{
	task_fname = fname;
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

int TaskPreparator::load_task( string fname, TetrMeshSet* mesh_set )
{	
	Logger* logger = new Logger();
	mesh_set->attach( logger );
	
	CollisionDetector* cd = new CollisionDetector();
	cd->set_treshold( 0.1 );
	mesh_set->attach( cd );
	
	Stresser* stresser = new GCMStresser();
	stresser->loadTask("task.xml");
	mesh_set->attach(stresser);
	
	VoidRheologyCalculator* rc = new VoidRheologyCalculator();
	mesh_set->attach(rc);
	
	TiXmlDocument document( fname.c_str() );
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
							cout << "\tTaskPreparator : Can not open mesh file!\n";
							return 1;
						}
						GCM_Tetr_Plastic_Interpolation_1stOrder_Rotate_Axis* new_nm 
								= new GCM_Tetr_Plastic_Interpolation_1stOrder_Rotate_Axis();
						new_mesh->attach( new_nm );
						mesh_set->attach( new_mesh );
					}
					else {
						cout << "\tTaskPreparator : warning: mesh path in task is void\n";
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
						cout << "\tTaskPreparator : error: in rheology parameters\n";
						return 1;
					}
				
					TiXmlElement* earea = erheo->FirstChildElement( "area" );
					if( earea ) {
						string areatype = earea->Attribute( "type" );
						if( areatype == "mesh" ) {			//TODO add other options here
							int meshNum = atoi( earea->GetText() );
							if( meshNum == 0 && strcmp( earea->GetText(), "0" ) != 0 ) {
								cout << "\tTaskPreparator : error: cannot read rheology area specification\n";
								return 1;
							}
							set_fixed_elastic_rheology( &( ( mesh_set->get_mesh( meshNum ) )->nodes ), la, mu, rho, yield );
						}
					}
					else {
						cout << "\tTaskPreparator : error: area of rheology is not specified\n";
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
			cout << "\tTaskPreparator : failed to load xml task. exiting\n";
			return 1;	
		}
	}
	else {
		cout << "\tTaskPreparator : failed to load xml task. exiting\n";
		return 1;
	}
	return 0;
};

int TaskPreparator::load_snap_info( string fname, int* snap_num, int* step_per_snap ) 
{
	TiXmlDocument document( fname.c_str() );
	bool loadOk = document.LoadFile();
	if( loadOk ) {
		TiXmlElement* eroot = document.FirstChildElement( "task" );
		if( eroot ) {
			int snap_num_l = atoi( eroot->Attribute( "snap_num" ) );
			int step_per_snap_l = atoi( eroot->Attribute( "step_per_snap" ) );
			
			if( snap_num_l <= 0 || step_per_snap_l <= 0 ) {
				cout << "\tTaskPreparator : error: wrong snap info\n";
				return 1;
			}
			else {
				*snap_num = snap_num_l;
				*step_per_snap = step_per_snap_l;
				return 0;
			}
		}
		else {
			cout << "\tTaskPreparator : failed to load xml task. exiting\n";
			return 1;	
		}
	}
	else {
		cout << "\tTaskPreparator : failed to load xml task. exiting\n";
		return 1;
	}
	return 0;
};
