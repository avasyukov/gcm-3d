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

#include <vtkUnstructuredGrid.h>
#include <vtkXMLUnstructuredGridWriter.h>
#include <vtkUnstructuredGridWriter.h>
#include <vtkTetra.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <vtkstd/string>

#include "../meshtypes/TetrMesh_1stOrder.h"
#include "TetrMeshSet.h"

class VTKSnapshotWriter: LoggerUser
{
public:
	VTKSnapshotWriter();
	VTKSnapshotWriter(string result_dir);
	~VTKSnapshotWriter();
	void set_result_dir(string new_result_dir);
	string* get_snapshot_writer_type();
	int dump_vtk(TetrMesh_1stOrder* tetr_mesh, int snap_num);
	int dump_vtk(int snap_num);
	int dump_tetr_mesh(TetrMesh_1stOrder* tetr_mesh, int zone_num, int snap_num);

protected:
	string snapshot_writer_type;
	string result_dir;
};

#endif
