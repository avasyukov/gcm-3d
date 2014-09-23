#ifndef GCM_SNAPSHOTWRITER_H_
#define GCM_SNAPSHOTWRITER_H_

#include <string>

namespace gcm {

    class Mesh;

    class SnapshotWriter {
     public:
        virtual ~SnapshotWriter() = 0;

        virtual std::string dump(Mesh* mesh, int step, std::string fileName) const = 0;
        virtual std::string dump(Mesh* mesh, int step) const;
     protected:
        std::string extension;
        std::string getFileName(int step, std::string meshId) const;
    };
}


#endif
