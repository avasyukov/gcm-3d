#include "libgcm/snapshot/SnapshotWriter.hpp"

gcm::SnapshotWriter::~SnapshotWriter() {

}

string gcm::SnapshotWriter::getFileName(int cpuNum, int step, string meshId) {
    string filename = fname;

    auto replace = [&filename](string from, string to)
    {
        size_t start_pos = 0;
        while((start_pos = filename.find(from, start_pos)) != string::npos) {
            filename.replace(start_pos, from.length(), to);
            start_pos += to.length();
        }
    };

    replace("%z", to_string (cpuNum));
    replace("%n", to_string (step));
    replace("%m", meshId);
    return filename;
}

void gcm::SnapshotWriter::setFileName(string name) {
    fname = name;
}
