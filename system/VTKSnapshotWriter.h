#ifndef _GCM_VTK_SNAPSHOT_WRITER_H
#define _GCM_VTK_SNAPSHOT_WRITER_H  1

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <getopt.h>
#include <stdlib.h>
#include "Utils.h"

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
#include "SnapshotWriter.h"

class VTKSnapshotWriter: public SnapshotWriter
{
public:
	VTKSnapshotWriter(char *param);
	~VTKSnapshotWriter();
	string* get_snapshot_writer_type();
	int dump_vtk(TetrMesh_1stOrder* tetr_mesh, int snap_num);
	int dump_vtk(int snap_num);
	int dump_tetr_mesh(TetrMesh_1stOrder* tetr_mesh, int zone_num, int snap_num);
	void dump(int snap_num);
	void parseArgs(int argc, char **argv);
	void init();
protected:
	string snapshot_writer_type;
	string fname;
	string resultdir;
};

#endif
