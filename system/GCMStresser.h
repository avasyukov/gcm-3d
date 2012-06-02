#ifndef H_ELASTICNODE
#define H_ELASTICNODE 1

#ifndef TIXML_USE_STL
#define TIXML_USE_STL
#endif
#include <tinyxml.h>
#include <sstream>
#include <vector>
#include <string>
#include <math.h>

using std::stringstream;
using std::cerr;

#include "CylTest.h"
#include "Stresser.h"
#include "../datatypes/ElasticNode.h"
#include "../system/LoggerUser.h"

#define STRESS_FORM_TRIANGLE "triangle"
#define STRESS_FORM_STEP "step"

#define STRESS_AREA_BOX "box"
#define STRESS_AREA_SPHERE "sphere"
#define STRESS_AREA_CYLINDER "cylinder"

#define STRESS_PERFORM_UNIFORM "uniform"
#define STRESS_PERFORM_RADIAL "radial"
#define STRESS_PERFORM_CYLINDRICAL "cylindrical"

class GCMStressArea: protected LoggerUser
{
public:
	virtual bool isInArea( ElasticNode* cur_node ) = 0;
	virtual void print() = 0;
};

class GCMStressAreaSphere : public GCMStressArea 
{
private:
	float radius;
	float center[3];
public:
	GCMStressAreaSphere( float _radius, float x, float y, float z );
	virtual bool isInArea( ElasticNode* cur_node );
	virtual void print();
};

class GCMStressAreaBox : public GCMStressArea 
{
private:
	float minX, maxX;
	float minY, maxY;
	float minZ, maxZ;
public:
	GCMStressAreaBox( float _minX, float _maxX, float _minY, float _maxY, float _minZ, float _maxZ );
	virtual bool isInArea( ElasticNode* cur_node );
	virtual void print();
};

class GCMStressAreaCylinder : public GCMStressArea
{
public:
	float radius;
	float begin[3];
	float end[3];

	GCMStressAreaCylinder( float _radius, float _bX, float _bY, float _bZ, float _eX, float _eY, float _eZ );
	virtual bool isInArea( ElasticNode* cur_node );
	virtual void print();
};

class GCMStressForm: protected LoggerUser
{
public:
	virtual float calcMagnitude( float currentTime, int varNum ) = 0;
	virtual void print() = 0;
};

class GCMStressFormStep : public GCMStressForm
{
private:
	float beginTime;
	float duration;
	float* amplitude;
public:
	GCMStressFormStep( float _beginTime, float _duration, float* _amplitude );
	virtual float calcMagnitude( float currentTime, int varNum = 0 );
	virtual void print();
};

class GCMStressFormTriangle : public GCMStressForm
{
private:
	float beginTime;
	float duration;
	float* amplitude;
public:
	GCMStressFormTriangle( float _beginTime, float _duration, float* _amplitude );
	virtual float calcMagnitude( float currentTime, int varNum = 0 );
	virtual void print();
};

class GCMStressPerformer: protected LoggerUser
{
public:
//	GCMStressPerformer() {}
	virtual bool performStress( GCMStressArea* area, GCMStressForm* form, ElasticNode* cur_node, ElasticNode* new_node, float current_time ) = 0;
	virtual void print() = 0;
};

class GCMStressPerformerRadial : public GCMStressPerformer
{
public:
	GCMStressPerformerRadial() {}
	virtual bool performStress( GCMStressArea* area, GCMStressForm* form, ElasticNode* cur_node, ElasticNode* new_node, float current_time );
	virtual void print();
};

class GCMStressPerformerUniform : public GCMStressPerformer
{
public:
	//GCMStressPerformerUniform();
	virtual bool performStress( GCMStressArea* area, GCMStressForm* form, ElasticNode* cur_node, ElasticNode* new_node, float current_time );
	virtual void print();	
};

class GCMStressPerformerCylindrical : public GCMStressPerformer
{
public:
	GCMStressPerformerCylindrical() {}
	virtual bool performStress( GCMStressArea* area, GCMStressForm* form, ElasticNode* cur_node, ElasticNode* new_node, float current_time );
	virtual void print();
};

class GCMStress: protected LoggerUser
{
public:
	GCMStressArea* area;
	GCMStressForm* form;
	GCMStressPerformer* performer;

	bool checkArea(ElasticNode* cur_node);
	bool performStress(ElasticNode* cur_node, ElasticNode* new_node, float current_time);
	void print();
};

class GCMStresser : public Stresser
{
private:
	vector<GCMStress*> stresses;
	void badFormat();
	void tokenize( const string& str, vector<string>* result );
	bool getAmpl( const string& str, float* vals );
	GCMStressArea* createArea( const string& type, const string& str );

public:
	GCMStresser();
	~GCMStresser();
	void print();
	bool loadTask(string fname);
	void set_current_stress(ElasticNode* cur_node, ElasticNode* new_node, float current_time);
};

#endif
