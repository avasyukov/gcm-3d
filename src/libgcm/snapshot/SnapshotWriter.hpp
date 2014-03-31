#ifndef GCM_SNAPSHOTWRITER_H_
#define GCM_SNAPSHOTWRITER_H_

#include <string>

#include "libgcm/mesh/Mesh.hpp"
#include "libgcm/Utils.hpp"

using namespace std;

namespace gcm {

    class SnapshotWriter {
    public:
        virtual ~SnapshotWriter() = 0;
        /*
         * Returns snapshot writer type
         */
        virtual string getType() = 0;
        virtual void dump(Mesh* mesh, int step) = 0;

        string getFileName(int cpuNum, int step, string meshId);
        void setFileName(string name);
    protected:
        string fname;
    };
}


#endif
