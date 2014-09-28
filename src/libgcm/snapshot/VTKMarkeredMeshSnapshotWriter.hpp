#ifndef GCM_VTKMARKEREDMESHSNAPSHOTWRITER_H_
#define GCM_VTKMARKEREDMESHSNAPSHOTWRITER_H_

#include <string>

#include "libgcm/util/Singleton.hpp"
#include "libgcm/snapshot/AbstractVTKSnapshotWriter.hpp"
#include "libgcm/Logging.hpp"

namespace gcm {

    class MarkeredMesh;

    class VTKMarkeredMeshSnapshotWriter : public StructuredVTKSnapshotWriter<MarkeredMesh, true>, public Singleton<VTKMarkeredMeshSnapshotWriter>
    {
     protected:
        USE_LOGGER;
        void dumpMeshSpecificData(MarkeredMesh* mesh, vtkSmartPointer<vtkStructuredGrid>& grid, vtkSmartPointer<vtkPoints>& points) const override;
     public:
        VTKMarkeredMeshSnapshotWriter();
    };
}


#endif
