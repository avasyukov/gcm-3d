#ifndef GCM_MARKERED_SURFACE_SNAPSHOT_WRITER
#define GCM_MARKERED_SURFACE_SNAPSHOT_WRITER

#include "libgcm/snapshot/SnapshotWriter.hpp"
#include "libgcm/Logging.hpp"
#include "libgcm/util/Singleton.hpp"

namespace gcm
{
    class MarkeredSurfaceSnapshotWriter: public SnapshotWriter, public Singleton<MarkeredSurfaceSnapshotWriter>
    {
    protected:
        USE_LOGGER;
    public:
        MarkeredSurfaceSnapshotWriter();
        std::string dump(Mesh* mesh, int step, std::string fileName) const override;
    };
}

#endif
