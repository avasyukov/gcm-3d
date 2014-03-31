#ifndef GCM_VTKCUBICSNAPSHOTWRITER_H_
#define GCM_VTKCUBICSNAPSHOTWRITER_H_

#include <string>

#include "libgcm/snapshot/SnapshotWriter.h"
#include "libgcm/mesh/cube/BasicCubicMesh.h"
#include "libgcm/node/CalcNode.h"
#include "libgcm/Logging.h"
#include "libgcm/Utils.h"

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
