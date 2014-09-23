#ifndef GCM_VTKSNAPSHOTWRITER_H_
#define GCM_VTKSNAPSHOTWRITER_H_

#include <string>

#include "libgcm/util/Singleton.hpp"
#include "libgcm/snapshot/SnapshotWriter.hpp"
#include "libgcm/Logging.hpp"


namespace gcm {
    class TetrMeshSecondOrder;

    class VTKSnapshotWriter : public SnapshotWriter, public Singleton<VTKSnapshotWriter> {
     protected:
        std::string dumpVTK(std::string filename, TetrMeshSecondOrder* mesh, int step) const;
        USE_LOGGER;
     public:
        VTKSnapshotWriter();

        std::string dump(Mesh* mesh, int step, std::string fileName) const override;
    };
}


#endif
