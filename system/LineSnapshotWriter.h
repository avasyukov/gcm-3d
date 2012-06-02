#ifndef LINESNAPSHOTWRITER_H
#define	LINESNAPSHOTWRITER_H

#include "SnapshotWriter.h"
#include "Utils.h"
#include <mpi.h>
#include <getopt.h>
#include <stdlib.h>
#include "GCMException.h"
#include "DataBus.h"
#include "TetrMeshSet.h"
#include "../meshtypes/TetrMesh_1stOrder.h"

class LineSnapshotWriter: public SnapshotWriter {
public:
	LineSnapshotWriter(char *param);
	void dump(int step);
	void parseArgs(int argc, char **argv);
	void init();
protected:
	float from[3], fromr[3];
	float to[3];
	float dir[3];
	float len;
	string fname;
	string resultdir;
	float threshold;

};

#endif	/* LINESNAPSHOTWRITER_H */

