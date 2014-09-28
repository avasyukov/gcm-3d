#ifndef GCM_VTKCUBICSNAPSHOTWRITER_H_
#define GCM_VTKCUBICSNAPSHOTWRITER_H_

#include <string>

#include "libgcm/util/Singleton.hpp"
#include "libgcm/snapshot/AbstractVTKSnapshotWriter.hpp"
#include "libgcm/node/CalcNode.hpp"
#include "libgcm/Logging.hpp"

namespace gcm {
    class BasicCubicMesh;

    class VTKCubicSnapshotWriter : public StructuredVTKSnapshotWriter<BasicCubicMesh>, public Singleton<VTKCubicSnapshotWriter> {
    protected:
        USE_LOGGER;
        void dumpMeshSpecificData(BasicCubicMesh* mesh, vtkSmartPointer<vtkStructuredGrid>& grid, vtkSmartPointer<vtkPoints>& points) const override;
    public:
        VTKCubicSnapshotWriter();
    };
}

#endif
