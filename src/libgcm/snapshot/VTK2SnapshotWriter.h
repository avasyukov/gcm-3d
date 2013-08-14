#ifndef GCM_VTK2SNAPSHOTWRITER_H_
#define GCM_VTK2SNAPSHOTWRITER_H_

#include <string>
#include "SnapshotWriter.h"
#include "../Utils.h"
#include "../node/CalcNode.h"
#include "../elem/TetrFirstOrder.h"
#include "../mesh/TetrMeshFirstOrder.h"
#include "../mesh/TetrMeshSecondOrder.h"
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
	class VTK2SnapshotWriter : public SnapshotWriter {
	private:
		void dumpVTK(string filename, TetrMeshSecondOrder* mesh, int step);
		bool shouldSnapshot(CalcNode* node, TetrMeshSecondOrder* mesh);
		string fname;
		USE_LOGGER;
	public:
		VTK2SnapshotWriter();
		VTK2SnapshotWriter(const char *snapName);
		~VTK2SnapshotWriter();
		/*
		 * Returns snapshot writer type
		 */
		string getFileName(int cpuNum, int step);
		void setFileName(string name);
		string getType();
		void dump(TetrMeshFirstOrder* mesh, int step);
		void dump(TetrMeshSecondOrder* mesh, int step);
		void init();
	};
}


#endif
