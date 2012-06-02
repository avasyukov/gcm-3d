#ifndef _GCM_SNAPSHOT_WRITER_H
#define	_GCM_SNAPSHOT_WRITER_H

#include "LoggerUser.h"

class SnapshotWriter: protected LoggerUser {
public:
	SnapshotWriter(){};
	SnapshotWriter(char *param){};
	virtual void parseArgs(int argc, char **argv) = 0;
	virtual void dump(int step) = 0;
	virtual void init() = 0;
};

#endif
