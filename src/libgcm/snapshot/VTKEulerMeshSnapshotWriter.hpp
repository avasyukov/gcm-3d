#ifndef GCM_VTKEULERMESHSNAPSHOTWRITER_H_
#define GCM_VTKEULERMESHSNAPSHOTWRITER_H_

#include <string>

#include "libgcm/util/Singleton.hpp"
#include "libgcm/snapshot/AbstractVTKSnapshotWriter.hpp"
#include "libgcm/Logging.hpp"

namespace gcm {

    class EulerMesh;

    class VTKEulerMeshSnapshotWriter : public StructuredVTKSnapshotWriter<EulerMesh>, public Singleton<VTKEulerMeshSnapshotWriter>
    {
     protected:
        USE_LOGGER;
        void dumpMeshSpecificData(EulerMesh* mesh, vtkSmartPointer<vtkStructuredGrid>& grid, vtkSmartPointer<vtkPoints>& points) const override;
     public:
        VTKEulerMeshSnapshotWriter();
    };
}


#endif
