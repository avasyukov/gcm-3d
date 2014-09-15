#ifndef GCM_VTKMARKEREDMESHSNAPSHOTWRITER_H_
#define GCM_VTKMARKEREDMESHSNAPSHOTWRITER_H_

#include <string>

#include "libgcm/util/Singleton.hpp"
#include "libgcm/snapshot/SnapshotWriter.hpp"
#include "libgcm/Logging.hpp"

namespace gcm {

    class MarkeredMesh;

    class VTKMarkeredMeshSnapshotWriter : public SnapshotWriter, public Singleton<VTKMarkeredMeshSnapshotWriter>
    {
     protected:
        USE_LOGGER;
        std::string dumpVTK(std::string filename, const MarkeredMesh& mesh, int step) const;
     public:
        VTKMarkeredMeshSnapshotWriter();

        std::string dump(Mesh* mesh, int step, std::string fileName) const override;
    };
}


#endif
