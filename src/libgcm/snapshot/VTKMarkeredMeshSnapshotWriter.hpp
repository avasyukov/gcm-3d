#ifndef GCM_VTKMARKEREDMESHSNAPSHOTWRITER_H_
#define GCM_VTKMARKEREDMESHSNAPSHOTWRITER_H_

#include <string>

#include "libgcm/snapshot/SnapshotWriter.hpp"
#include "libgcm/mesh/markers/MarkeredMesh.hpp"
#include "libgcm/node/CalcNode.hpp"
#include "libgcm/Logging.hpp"

#ifdef CONFIG_VTK_5
#include <vtkstd/string>
#else
#include <vtkStdString.h>
#endif
#include <vtkStructuredGrid.h>
#include <vtkXMLStructuredGridWriter.h>
#include <vtkSmartPointer.h>
#include <vtkCellArray.h>
#include <vtkPoints.h>
#include <vtkDoubleArray.h>
#include <vtkIntArray.h>
#include <vtkPointData.h>
#include <vtkCellData.h>

namespace gcm {
    class VTKMarkeredMeshSnapshotWriter : public SnapshotWriter 
    {
    private:
    public:
        void dumpVTK(string filename, const MarkeredMesh& mesh, int step);
        USE_LOGGER;
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
