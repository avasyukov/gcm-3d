#ifndef GCM_VTKCUBICSNAPSHOTWRITER_H_
#define GCM_VTKCUBICSNAPSHOTWRITER_H_

#include <string>

#include "snapshot/SnapshotWriter.h"
#include "mesh/cube/BasicCubicMesh.h"
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
	class VTKCubicSnapshotWriter : public SnapshotWriter {
	private:
		void dumpVTK(string filename, BasicCubicMesh* mesh, int step);
		USE_LOGGER;
	public:
		VTKCubicSnapshotWriter();
		VTKCubicSnapshotWriter(const char *snapName);
		/*
		 * Returns snapshot writer type
		 */
		string getType();
		void dump(Mesh* mesh, int step);
	};
}


#endif
