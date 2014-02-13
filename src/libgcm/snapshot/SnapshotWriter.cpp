#include "snapshot/SnapshotWriter.h"

gcm::SnapshotWriter::~SnapshotWriter() {

}

string gcm::SnapshotWriter::getFileName(int cpuNum, int step, string meshId) {
	string filename = fname;
	replaceAll (filename, "%z", t_to_string (cpuNum));
	replaceAll (filename, "%n", t_to_string (step));
	replaceAll (filename, "%m", meshId);
	return filename;
}

void gcm::SnapshotWriter::setFileName(string name) {
	fname = name;
}
