#ifndef _GCM_SNAPSHOT_WRITER_H
#define _GCM_SNAPSHOT_WRITER_H  1

#include <string>
#include <vector>

using std::string;
using std::vector;

#include "../datatypes/ElasticNode.h"
#include "../meshtypes/TetrMesh_1stOrder.h"

class SnapshotWriter
{
public:
	SnapshotWriter();
	~SnapshotWriter();
	string* get_snapshot_writer_type();
	void attach(Logger* new_logger);
	int dump_tetr_mesh(TetrMesh_1stOrder* tetr_mesh, string file_name);
	void set_basement(float x0, float y0, float z0, float h, int N);
	int dump_cubic_mesh(TetrMesh_1stOrder* tetr_mesh, string file_name);
		// TODO change parametres names to make them descriptive
protected:
	void zero_node_values(ElasticNode* node); // TODO move it to some helper?
	float min_of_four(float a, float b, float c, float d); // TODO move it to some helper?
	float max_of_four(float a, float b, float c, float d); // TODO move it to some helper?

	float base_coord[3];
	float step_h;
	int number_of_segments;

	string snapshot_writer_type;
	Logger* logger;
};

#include "SnapshotWriter.inl"

#endif
