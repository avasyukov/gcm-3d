#ifndef GCM_VTK2SNAPSHOTWRITER_H_
#define GCM_VTK2SNAPSHOTWRITER_H_

#include <string>

#include <vtkstd/string>
#include <vtkUnstructuredGrid.h>
#include <vtkXMLUnstructuredGridWriter.h>
#include <vtkUnstructuredGridWriter.h>
#include <vtkTetra.h>
#include <vtkDoubleArray.h>
#include <vtkIntArray.h>
#include <vtkPointData.h>
#include <vtkCellData.h>

#include "snapshot/SnapshotWriter.h"
#include "mesh/tetr/TetrMeshFirstOrder.h"
#include "mesh/tetr/TetrMeshSecondOrder.h"
#include "node/CalcNode.h"
#include "elem/TetrFirstOrder.h"
#include "Logging.h"
#include "Utils.h"

namespace gcm {
    class VTK2SnapshotWriter : public SnapshotWriter {
    private:
        void dumpVTK(string filename, TetrMeshSecondOrder* mesh, int step);
        bool shouldSnapshot(CalcNode& node, TetrMeshSecondOrder* mesh);

        USE_LOGGER;
    public:
        VTK2SnapshotWriter();
        VTK2SnapshotWriter(const char *snapName);
        virtual ~VTK2SnapshotWriter();
        /*
         * Returns snapshot writer type
         */
        string getType();
        void dump(Mesh* mesh, int step);
    };
}


#endif
