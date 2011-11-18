#include "math.h"
#include "CylTest.cpp"

GCMStressAreaSphere::GCMStressAreaSphere( float _radius, float x, float y, float z )
{
	radius = _radius;
	center[0] = x;
	center[1] = y;
	center[2] = z;
}

bool GCMStressAreaSphere::isInArea( ElasticNode* cur_node )
{
	if( ( cur_node->coords[0] - center[0] ) * ( cur_node->coords[0] - center[0] ) +
		( cur_node->coords[1] - center[1] ) * ( cur_node->coords[1] - center[1] ) + 
		( cur_node->coords[2] - center[2] ) * ( cur_node->coords[2] - center[2] ) < 
		radius * radius ) 
	{
		return true;
	} 
	return false;
}

void GCMStressAreaSphere::print()
{
	cout << " sphere\n";
	for ( int i = 0; i < 3; ++i ) {
		cout << " " << center[i];
	}
	cout << endl;
}

GCMStressAreaBox::GCMStressAreaBox( float _minX, float _maxX, float _minY, float _maxY, float _minZ, float _maxZ )
{
	minX = _minX;
	maxX = _maxX;
	minY = _minY;
	maxY = _maxY;
	minZ = _minZ;
	maxZ = _maxZ;
}

bool GCMStressAreaBox::isInArea( ElasticNode* cur_node )
{
	if( cur_node->coords[0] < maxX &&
		cur_node->coords[0] > minX &&
		cur_node->coords[1] < maxY &&
		cur_node->coords[1] > minY &&
		cur_node->coords[2] < maxZ &&
		cur_node->coords[2] > minZ )
	{
		return true;
	}
	return false;
}

void GCMStressAreaBox::print()
{
	cout << " box\n";
	cout << " " << minX << " " << maxX << endl;
	cout << " " << minY << " " << maxY << endl;
	cout << " " << minZ << " " << maxZ << endl;
}


GCMStressAreaCylinder::GCMStressAreaCylinder( float _radius, float _bX, float _bY, float _bZ, float _eX, float _eY, float _eZ )
{
	radius = _radius;
	begin[0] = _bX;
	begin[1] = _bY;
	begin[2] = _bZ;

	end[0] = _eX;
	end[1] = _eY;
	end[2] = _eZ;
}

bool GCMStressAreaCylinder::isInArea( ElasticNode* cur_node )
{
	Vec3 testpt;
	testpt.x = cur_node->coords[0];
	testpt.y = cur_node->coords[1];
	testpt.z = cur_node->coords[2];
	
	Vec3 pt1;
	pt1.x = begin[0];
	pt1.y = begin[1];
	pt1.z = begin[2];
	
	Vec3 pt2;
	pt2.x = end[0];
	pt2.y = end[1];
	pt2.z = end[2];
	
	float radius_sq = radius * radius;
	float length_sq = ( pt1.x - pt2.x ) * ( pt1.x - pt2.x ) + ( pt1.y - pt2.y ) * ( pt1.y - pt2.y ) + ( pt1.z - pt2.z ) * ( pt1.z - pt2.z );
	
	if( CylTest_CapsFirst( pt1, pt2, length_sq, radius_sq, testpt ) >= 0.0 ) {
		return true;
	}
	
	return false;
}

void GCMStressAreaCylinder::print()
{
	cout << " cylinder\n";
	cout << " " << radius << endl;
	for ( int i = 0; i < 3; ++i ) {
		cout << " " << begin[i];
	}
	for ( int i = 0; i < 3; ++i ) {
		cout << " " << end[i];
	}
	cout << endl;
}

//============================GCMSTRESSFORM====================================

GCMStressFormStep::GCMStressFormStep( float _beginTime, float _duration, float* _amplitude)
{
	beginTime = _beginTime;
	duration = _duration;
	amplitude = new float[9];
	
	memcpy( amplitude, _amplitude, sizeof( float ) * 9 );			//caution here
}

float GCMStressFormStep::calcMagnitude(float currentTime, int varNum )
{
	if( currentTime < beginTime || currentTime > beginTime + duration) {
		return -1.0;
	}
	else {
		return amplitude[varNum];	//caution here
	}
	return -1.0;
}

void GCMStressFormStep::print()
{
	cout << " step\n";
	cout << " " << beginTime << " " << duration << endl;
	for( int i = 0; i < 9; ++i ) {
		cout << " " << amplitude[i];
	}
	cout << endl;
}

GCMStressFormTriangle::GCMStressFormTriangle( float _beginTime, float _duration, float* _amplitude)
{
	beginTime = _beginTime;
	duration = _duration;
	amplitude = new float[9];		
	
	memcpy( amplitude, _amplitude, sizeof( float ) * 9 );		//caution here
}

float GCMStressFormTriangle::calcMagnitude(float currentTime, int varNum )
{
	if( currentTime < beginTime || currentTime > beginTime + duration) {
		return -1.0;
	}
	else {
		if( currentTime < beginTime + duration / 2 ) {
			return amplitude[varNum] * ( currentTime - beginTime ) / duration / 2;	//caution here
		}
		else {
			return amplitude[varNum] * ( 1 - ( currentTime - ( beginTime + duration / 2 ) ) / duration / 2 );	//caution here
		}
	}
	return -1.0;
}

void GCMStressFormTriangle::print()
{
	cout << " triangle\n";
	cout << " " << beginTime << " " << duration << endl;
	for( int i = 0; i < 9; ++i ) {
		cout << " " << amplitude[i];
	}
	cout << endl;
}

//============================GCMSTRESSPERFORMER==================================

bool GCMStressPerformerUniform::performStress( GCMStressArea* area, GCMStressForm* form, ElasticNode* cur_node, ElasticNode* new_node, float current_time)
{
	for( int i = 0; i < 9; ++i) {
		float new_val = form->calcMagnitude( current_time, i );
		if( new_val < 0 ) {
			return true;
		}
		cur_node->values[i] = new_val;
	}
	return true;
}

void GCMStressPerformerUniform::print()
{
	cout << " uniform\n";
}

bool GCMStressPerformerCylindrical::performStress( GCMStressArea* area, GCMStressForm* form, ElasticNode* cur_node, ElasticNode* new_node, float current_time )
{
	float new_val = form->calcMagnitude( current_time, 0 );
	if( new_val < 0 ) {
		return true;
	}
	
	GCMStressAreaCylinder* areaCyl = static_cast<GCMStressAreaCylinder*>( area );
	Vec3 VNode( cur_node->values[0], cur_node->values[1], cur_node->values[2] );
	Vec3 V1( areaCyl->begin[0], areaCyl->begin[1], areaCyl->begin[2] );
	Vec3 V2( areaCyl->end[0], areaCyl->end[1], areaCyl->end[2] );
	
	Vec3 newVel = Norm_Project_Point_Line( VNode, V1, V2 );
	
	cur_node->values[0] = newVel.x * new_val;
	cur_node->values[1] = newVel.y * new_val;
	cur_node->values[2] = newVel.z * new_val;
	
	return true;
}

void GCMStressPerformerCylindrical::print()
{
	cout << " cylindrical\n";
}

bool GCMStressPerformerRadial::performStress( GCMStressArea* area, GCMStressForm* form, ElasticNode* cur_node, ElasticNode* new_node, float current_time)
{
	/*for( int i = 0; i < 9; ++i) {
		float new_val = form->calcMagnitude( current_time, i );
		if( new_val < 0 ) {
			return true;
		}
		cur_node->values[i] = new_val;
	}*/
	return true;
}

void GCMStressPerformerRadial::print()
{
	cout << " radial\n";
}

//============================GCMSTRESS====================================

bool GCMStress::performStress(ElasticNode* cur_node, ElasticNode* new_node, float current_time)
{
	performer->performStress( area, form, cur_node, new_node, current_time);
}

void GCMStress::print()
{
	cout << " stress\n";
	
	if( area == 0 ) {
		cout << "area is null!!!\n";
	}
	
	
	form->print();
	area->print();
	performer->print();
}

//============================GCMSTRESSER==================================

void GCMStresser::set_current_stress(ElasticNode* cur_node, ElasticNode* new_node, float current_time)
{
	for( int i = 0; i < stresses.size(); ++i ) {
		( stresses[i] )->performStress( cur_node, new_node, current_time );
	}
}

void GCMStresser::badFormat()
{
	cerr << "Bad file formatting. Terminating\n";
}

void GCMStresser::tokenize( const string& str, vector<string>* result )
{
	stringstream ss( stringstream::in | stringstream::out );
	ss << str;
	
	string strPart;
	ss >> strPart;
	result->push_back( strPart );
	while( ss.eof() != true ) {
		ss >> strPart;
		result->push_back( strPart );
		cout << " - " << strPart << endl;
	}
}

bool GCMStresser::getAmpl( const string& str, float* vals )
{
	vector<string> parts;
	tokenize( str, &parts );
	
	if( parts.size() != 10 ) {
		return false;
	}
	if( parts[0] != "amplitude" ) {
		return false;
	}

	for( int i = 0; i < 9; ++i ) {
		vals[i] = atof( parts[i + 1].c_str() );
	}
	return true;
}

GCMStressArea* GCMStresser::createArea( const string& type, const string& str ) {
	vector<string> parts;
	tokenize( str, &parts );
	
	GCMStressArea* area;
	
	if( type == STRESS_AREA_BOX ) { 
		if( parts.size() != 9 ) {
			return 0;
		}
		float minX, maxX;
		float minY, maxY;
		float minZ, maxZ;
		
		minX = atof( parts[1].c_str() );
		maxX = atof( parts[2].c_str() );
		minY = atof( parts[4].c_str() );
		maxY = atof( parts[5].c_str() );
		minZ = atof( parts[7].c_str() );
		maxZ = atof( parts[8].c_str() );
		
		area = new GCMStressAreaBox( minX, maxX, minY, maxY, minZ, maxZ );
	}
	else if( type == STRESS_AREA_SPHERE ) {
		if( parts.size() != 6 ) {
			return 0;
		}
		float radius;
		float centerX, centerY, centerZ;
		
		radius = atof( parts[1].c_str() );
		centerX = atof( parts[3].c_str() );
		centerY = atof( parts[4].c_str() );
		centerZ = atof( parts[5].c_str() );
		
		area = new GCMStressAreaSphere( radius, centerX, centerY, centerZ );
	}
	else if( type == STRESS_AREA_CYLINDER ) {
		if( parts.size() != 10 ) {
			return 0;
		}
		float radius;
		float bX, bY, bZ;
		float eX, eY, eZ;
		
		radius = atof( parts[1].c_str() );
		bX = atof( parts[3].c_str() );
		bY = atof( parts[4].c_str() );
		bZ = atof( parts[5].c_str() );

		eX = atof( parts[7].c_str() );
		eY = atof( parts[8].c_str() );
		eZ = atof( parts[9].c_str() );
				
		area = new GCMStressAreaCylinder( radius, bX, bY, bZ, eX, eY, eZ );
	}
	else {
		return 0;
	}
	
	return area;	
}

bool GCMStresser::loadTask(string fname)
{
	TiXmlDocument document( fname.c_str() );
	bool loadOk = document.LoadFile();
	if( loadOk ) {
		TiXmlElement* eroot = document.FirstChildElement( "task" );
		if( eroot ) {
			TiXmlElement* estress = eroot->FirstChildElement( "stress" );; 
			while( true )
			{
				/*GCMStress* stress;
				GCMStressForm* stressForm;
				GCMStressArea* stressArea;
				GCMStressPerformer* stressPerformer;*/
				
				
				if( estress ) {
					stresses.push_back( new GCMStress() );
					//stress = new GCMStress();
					
					TiXmlElement* eform = estress->FirstChildElement( "form" );
					if( eform ) {
						string formtype = eform->Attribute( "type" );
						int begintime = atoi( eform->Attribute( "begintime" ) );
						int duration = atoi( eform->Attribute( "duration" ) );
					
						string amplitudeStr = eform->GetText();
						float vals[9];
						if( getAmpl( amplitudeStr, vals ) != true ) {
							badFormat();
							return false;
						}
						
						if( formtype == STRESS_FORM_STEP ) {
							stresses[ stresses.size() - 1 ]->form = new GCMStressFormStep( begintime, duration, vals );
						}
						else if( formtype == STRESS_FORM_TRIANGLE ) {
							stresses[ stresses.size() - 1 ]->form = new GCMStressFormTriangle( begintime, duration, vals );
						}
					}
					else {
						badFormat();
						return false;
					}
					
					
					TiXmlElement* earea = estress->FirstChildElement( "area" );
					if( earea ) {
						string areatype = earea->Attribute( "type" );
						string areaString = earea->GetText();			
						
						stresses[ stresses.size() - 1 ]->area = createArea( areatype, areaString );
						if( stresses[ stresses.size() - 1 ]->area == 0 ) {
							badFormat();
							return false;
						}
					}
					else {
						badFormat();
						return false;
					}
					
					TiXmlElement* eperform = estress->FirstChildElement( "perform" );
					if( eperform ) {
						string performtype = eperform->Attribute( "type" );
							
						if( performtype == STRESS_PERFORM_UNIFORM ) {
							stresses[ stresses.size() - 1 ]->performer = new GCMStressPerformerUniform();
						}	
						else if( performtype == STRESS_PERFORM_RADIAL ) {
							stresses[ stresses.size() - 1 ]->performer = new GCMStressPerformerRadial();
						}
						else if( performtype == STRESS_PERFORM_CYLINDRICAL ) {
							stresses[ stresses.size() - 1 ]->performer = new GCMStressPerformerCylindrical();
						}
						else {
							badFormat();
							return false;
						}
						cout << performtype << endl;
					}
					else {
						badFormat();
						return false;
					}
				}
				else {
					break;
				}
				
				estress = estress->NextSiblingElement( "stress" );
				cout << "-----------interation----------------\n";
			}
		}
	}
	else {
		cout << "failed to load xml task. exiting\n";
	}
	
	return true;
}

void GCMStresser::print()
{
	cout << " size " << stresses.size() << endl;
	for( int i = 0; i < stresses.size(); ++i ) {
		cout << i << endl;
		stresses[i]->print();
	}	
}
