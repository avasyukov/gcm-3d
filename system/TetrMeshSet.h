#ifndef _GCM_MESH_SET
#define _GCM_MESH_SET 1

#include <vector>
#include <string>
#include "../system/CollisionDetector.h"

using std::string;
using std::vector;

class TetrMeshSet
{
public:
	TetrMeshSet();
	~TetrMeshSet();
	void attach(Logger* new_logger);
	void attach(DataBus* new_data_bus);
	void attach(Stresser* new_stresser);
	void attach(RheologyCalculator* new_rheology);
	void attach(TetrNumericalMethod* new_numerical_method);
	void attach(TetrMesh_1stOrder* new_mesh);
	void attach(CollisionDetector* new_collision_detector);
	void log_meshes_types();
	void log_meshes_stats();
	float get_current_time();
	int do_next_step();
	int get_number_of_meshes();
	TetrMesh_1stOrder* get_mesh(int num);
protected:
	Logger* logger;
	RheologyCalculator* rheology;
	DataBus* data_bus;
	Stresser* stresser;
	TetrNumericalMethod* numerical_method;
	CollisionDetector* collision_detector;
        int mesh_set_num; // TODO - we do not use it now, reserved for parallel version
        vector<TetrMesh_1stOrder*> meshes;
};

#include "TetrMeshSet.inl"

#endif
