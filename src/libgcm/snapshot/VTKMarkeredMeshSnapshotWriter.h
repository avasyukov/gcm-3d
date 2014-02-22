#ifndef GCM_VTKMARKEREDMESHSNAPSHOTWRITER_H_
#define GCM_VTKMARKEREDMESHSNAPSHOTWRITER_H_

#include <string>

#include "snapshot/SnapshotWriter.h"
#include "mesh/markers/MarkeredMesh.h"
#include "node/CalcNode.h"
#include "Logging.h"
#include "Utils.h"

#include <vtkstd/string>
#include <vtkStructuredGrid.h>
#include <vtkXMLStructuredGridWriter.h>
#include <vtkSmartPointer.h>
#include <vtkCellArray.h>
#include <vtkPoints.h>
#include <vtkDoubleArray.h>
#include <vtkIntArray.h>
#include <vtkPointData.h>

namespace gcm {
	class VTKMarkeredMeshSnapshotWriter : public SnapshotWriter {
	private:
		void dumpVTK(string filename, MarkeredMesh* mesh, int step);
		USE_LOGGER;
	public:
		VTKMarkeredMeshSnapshotWriter();
		VTKMarkeredMeshSnapshotWriter(const char *snapName);
		/*
		 * Returns snapshot writer type
		 */
		string getType();
		void dump(Mesh* mesh, int step);
	};
}


#endif
