#ifndef GCM_VTKSNAPSHOTWRITER_H_
#define GCM_VTKSNAPSHOTWRITER_H_

#include <string>
#include "SnapshotWriter.h"
#include "../Utils.h"
#include "../node/CalcNode.h"
#include "../elem/TetrFirstOrder.h"
#include "../mesh/tetr/TetrMeshFirstOrder.h"
#include "../mesh/tetr/TetrMeshSecondOrder.h"
#include "../Logging.h"

#include <vtkUnstructuredGrid.h>
#include <vtkXMLUnstructuredGridWriter.h>
#include <vtkUnstructuredGridWriter.h>
#include <vtkTetra.h>
#include <vtkDoubleArray.h>
#include <vtkIntArray.h>
#include <vtkPointData.h>
#include <vtkCellData.h>
#include <vtkstd/string>

namespace gcm {
	class VTKSnapshotWriter : public SnapshotWriter {
	private:
		void dumpVTK(string filename, TetrMeshSecondOrder* mesh, int step);
		string fname;
		USE_LOGGER;
	public:
		VTKSnapshotWriter();
		VTKSnapshotWriter(const char *snapName);
		~VTKSnapshotWriter();
		/*
		 * Returns snapshot writer type
		 */
		string getFileName(int cpuNum, int step, string meshId);
		void setFileName(string name);
		string getType();
		void dump(TetrMeshFirstOrder* mesh, int step);
		void dump(TetrMeshSecondOrder* mesh, int step);
		void init();
	};
}


#endif
