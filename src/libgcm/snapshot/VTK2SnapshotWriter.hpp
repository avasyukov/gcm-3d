#ifndef GCM_VTK2SNAPSHOTWRITER_H_
#define GCM_VTK2SNAPSHOTWRITER_H_

#include <string>

#include "libgcm/node/CalcNode.hpp"
#include "libgcm/util/Singleton.hpp"
#include "libgcm/snapshot/SnapshotWriter.hpp"
#include "libgcm/Logging.hpp"

namespace gcm {
    class TetrMeshSecondOrder;

    class VTK2SnapshotWriter : public SnapshotWriter, public Singleton<VTK2SnapshotWriter> {
     private:
        std::string dumpVTK(std::string filename, TetrMeshSecondOrder* mesh, int step) const;
        bool shouldSnapshot(CalcNode& node, TetrMeshSecondOrder* mesh) const;

        USE_LOGGER;
     public:
        VTK2SnapshotWriter();

        std::string dump(Mesh* mesh, int step, std::string fileName) const override;
    };
}


#endif
