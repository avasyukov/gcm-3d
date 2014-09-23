#ifndef GCM_VTKCUBICSNAPSHOTWRITER_H_
#define GCM_VTKCUBICSNAPSHOTWRITER_H_

#include <string>

#include "libgcm/util/Singleton.hpp"
#include "libgcm/snapshot/SnapshotWriter.hpp"
#include "libgcm/node/CalcNode.hpp"
#include "libgcm/Logging.hpp"

namespace gcm {
    class BasicCubicMesh;

    class VTKCubicSnapshotWriter : public SnapshotWriter, public Singleton<VTKCubicSnapshotWriter> {
     private:
        std::string dumpVTK(std::string filename, BasicCubicMesh* mesh, int step) const;
        USE_LOGGER;
    public:
        VTKCubicSnapshotWriter();

        std::string dump(Mesh* mesh, int step, std::string fileName) const override;
    };
}


#endif
