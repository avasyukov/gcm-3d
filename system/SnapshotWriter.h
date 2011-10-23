#ifndef _GCM_SNAPSHOT_WRITER_H
#define _GCM_SNAPSHOT_WRITER_H  1

#include <string>
#include <vector>
#include <fstream>
#include <sstream>

using std::string;
using std::vector;
using std::ofstream;
using std::stringstream;

#include "../datatypes/ElasticNode.h"
#include "../meshtypes/TetrMesh_1stOrder.h"

#include <vtkUnstructuredGrid.h>
#include <vtkXMLUnstructuredGridWriter.h>
#include <vtkUnstructuredGridWriter.h>
#include <vtkTetra.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <vtkstd/string>

class SnapshotWriter
{
public:
	SnapshotWriter();
	~SnapshotWriter();
	string* get_snapshot_writer_type();
	int dump_vtk(TetrMesh_1stOrder* tetr_mesh, int zone_num, int snap_num);
	void attach(Logger* new_logger);
	int dump_tetr_mesh(TetrMesh_1stOrder* tetr_mesh, int zone_num, int snap_num);
	void set_basement(float x0, float y0, float z0, float h, int N);
//	int dump_cubic_mesh(TetrMesh_1stOrder* tetr_mesh, int zone_num, int snap_num);
		// TODO change parametres names to make them descriptive
	int tmp_dump_line(TetrMesh_1stOrder* tetr_mesh, int snap_num);
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
