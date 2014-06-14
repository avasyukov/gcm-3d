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

#include "libgcm/snapshot/SnapshotWriter.hpp"
#include "libgcm/mesh/tetr/TetrMeshFirstOrder.hpp"
#include "libgcm/mesh/tetr/TetrMeshSecondOrder.hpp"
#include "libgcm/node/CalcNode.hpp"
#include "libgcm/elem/TetrFirstOrder.hpp"
#include "libgcm/Logging.hpp"

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
